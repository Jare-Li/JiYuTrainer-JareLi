#!/usr/bin/env python3
"""JiYuTrainer Jare-Li refactor regression tests.

Runs the C++ core tests and a set of static/build checks. Prints a per-test
result table and exits non-zero when the pass rate is below 90%.
"""

from __future__ import annotations

import ctypes
import ctypes.wintypes as wt
import json
import re
import struct
import subprocess
import sys
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT
RELEASE = SRC / "Release"
EXE = RELEASE / "JiYuTrainer.exe"
HOOK = RELEASE / "JiYuTrainerHooks.dll"
SCITER = SRC / "JiYuTrainerUI" / "sciter-sdk" / "lib" / "32" / "sciter.dll"
CORE = SRC / "build" / "tests" / "CoreTests.exe"
UI = SRC / "JiYuTrainerUI"

RESULTS: list[tuple[str, bool, str]] = []


def record(name: str, ok: bool, detail: str = "") -> None:
    RESULTS.append((name, bool(ok), detail))
    print(f"{'PASS' if ok else 'FAIL'}  {name}" + (f"  [{detail}]" if detail and not ok else ""))


def check(name: str, fn, fallback_detail: str = "") -> None:
    try:
        value = fn()
        if isinstance(value, tuple):
            ok, detail = value
        else:
            ok, detail = bool(value), ""
        if not ok and not detail:
            detail = fallback_detail
        record(name, ok, str(detail))
    except Exception as exc:  # noqa: BLE001 - test harness must continue
        record(name, False, f"{type(exc).__name__}: {exc}")


def read_text(path: Path, encoding: str = "utf-8") -> str:
    return path.read_text(encoding=encoding, errors="replace")


# --------------------------------------------------------------------------- PE

def pe_info(path: Path) -> tuple[int, int]:
    data = path.read_bytes()
    if data[:2] != b"MZ":
        raise ValueError("not a PE file")
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe:pe + 4] != b"PE\0\0":
        raise ValueError("bad PE signature")
    machine = struct.unpack_from("<H", data, pe + 4)[0]
    subsystem = struct.unpack_from("<H", data, pe + 0x5C)[0]
    return machine, subsystem


def version_strings(path: Path) -> dict[str, str]:
    version = ctypes.WinDLL("version", use_last_error=True)
    version.GetFileVersionInfoSizeW.argtypes = [wt.LPCWSTR, ctypes.POINTER(wt.DWORD)]
    version.GetFileVersionInfoSizeW.restype = wt.DWORD
    version.GetFileVersionInfoW.argtypes = [wt.LPCWSTR, wt.DWORD, wt.DWORD, ctypes.c_void_p]
    version.GetFileVersionInfoW.restype = wt.BOOL
    version.VerQueryValueW.argtypes = [
        ctypes.c_void_p, wt.LPCWSTR, ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(wt.UINT)
    ]
    version.VerQueryValueW.restype = wt.BOOL

    dummy = wt.DWORD(0)
    size = version.GetFileVersionInfoSizeW(str(path), ctypes.byref(dummy))
    if not size:
        raise OSError(f"no version resource: {path}")
    buffer = ctypes.create_string_buffer(size)
    if not version.GetFileVersionInfoW(str(path), 0, size, buffer):
        raise OSError("GetFileVersionInfoW failed")

    ptr = ctypes.c_void_p()
    length = wt.UINT()
    if not version.VerQueryValueW(buffer, r"\VarFileInfo\Translation", ctypes.byref(ptr), ctypes.byref(length)):
        raise OSError("no translation table")
    lang, codepage = struct.unpack("<HH", ctypes.string_at(ptr, 4))
    block = f"{lang:04x}{codepage:04x}"

    result: dict[str, str] = {}
    for key in ("CompanyName", "FileDescription", "FileVersion", "LegalCopyright",
                "OriginalFilename", "ProductName", "ProductVersion"):
        sub = rf"\StringFileInfo\{block}\{key}"
        if version.VerQueryValueW(buffer, sub, ctypes.byref(ptr), ctypes.byref(length)) and length.value:
            result[key] = ctypes.string_at(ptr, length.value * 2).decode("utf-16-le", "replace").rstrip("\0")
    return result


def signature_info(path: Path) -> tuple[str, str]:
    shell = "pwsh" if shutil_which("pwsh") else "powershell"
    command = (
        f"$s = Get-AuthenticodeSignature -LiteralPath '{path}'; "
        "Write-Output $s.Status; Write-Output $s.SignerCertificate.Subject"
    )
    proc = subprocess.run(
        [shell, "-NoProfile", "-NonInteractive", "-Command", command],
        capture_output=True, timeout=60,
    )
    raw = proc.stdout
    for encoding in ("utf-8-sig", "utf-16-le", "mbcs"):
        try:
            text = raw.decode(encoding)
            break
        except Exception:  # noqa: BLE001
            continue
    else:
        text = raw.decode("utf-8", "replace")
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    if len(lines) < 2:
        return "", text
    return lines[0], lines[1]


def shutil_which(name: str) -> str | None:
    import shutil
    return shutil.which(name)


# --------------------------------------------------------------------------- HTML resources

def html_resource(path: Path, resource_id: int) -> bytes:
    kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel32.LoadLibraryExW.argtypes = [wt.LPCWSTR, ctypes.c_void_p, wt.DWORD]
    kernel32.LoadLibraryExW.restype = wt.HMODULE
    kernel32.FindResourceW.argtypes = [wt.HMODULE, ctypes.c_void_p, ctypes.c_void_p]
    kernel32.FindResourceW.restype = ctypes.c_void_p
    kernel32.SizeofResource.argtypes = [wt.HMODULE, ctypes.c_void_p]
    kernel32.SizeofResource.restype = wt.DWORD
    kernel32.LoadResource.argtypes = [wt.HMODULE, ctypes.c_void_p]
    kernel32.LoadResource.restype = ctypes.c_void_p
    kernel32.LockResource.argtypes = [ctypes.c_void_p]
    kernel32.LockResource.restype = ctypes.c_void_p
    kernel32.FreeLibrary.argtypes = [wt.HMODULE]

    module = kernel32.LoadLibraryExW(str(path), None, 0x2)  # LOAD_LIBRARY_AS_DATAFILE
    if not module:
        raise OSError("LoadLibraryEx failed")
    try:
        handle = kernel32.FindResourceW(module, ctypes.c_void_p(resource_id), ctypes.c_void_p(23))
        if not handle:
            raise OSError(f"resource {resource_id} not found")
        size = kernel32.SizeofResource(module, handle)
        data = kernel32.LockResource(kernel32.LoadResource(module, handle))
        return ctypes.string_at(data, size)
    finally:
        kernel32.FreeLibrary(module)


# --------------------------------------------------------------------------- tests

MAIN_IDS = {
    "btn_about", "btn_close", "btn_debug", "btn_kill", "btn_min", "btn_power",
    "btn_protect_stat", "btn_restart", "btn_tab_about", "btn_tab_debug",
    "btn_tab_setting", "btn_top", "check_allow_control", "check_allow_monitor",
    "check_allow_op", "check_allow_top", "check_auto_update", "check_band_op",
    "check_probit_close_window", "check_probit_terminate_process", "cmds_message",
    "cmds_message_close", "commands_help_area", "common_message", "common_message_close",
    "common_message_close2", "common_message_text", "common_message_title", "exit_message",
    "exit_message_close", "exit_message_close2", "exit_message_end_ctl_and_exit",
    "exit_message_kill_and_exit", "extend_area", "hello_message", "input_cmd",
    "isnew_message", "isnew_message_close", "isnew_message_text", "isnew_message_title",
    "link_attact", "link_checkupdate", "link_close_extend_area", "link_close_hello_message",
    "link_close_power_message", "link_exit", "link_github", "link_help", "link_hide",
    "link_locate_jiyu_position", "link_more_cmd", "link_more_settings",
    "link_read_jiyu_password", "link_read_jiyu_password2", "link_reboot", "link_runcmd",
    "link_save_setting", "link_setto_default", "link_shutdown", "link_uninstall",
    "link_unload_netfilter", "page_about", "page_debug", "page_setting", "power_message",
    "status_area", "status_icon", "status_jiyu_path", "status_jiyu_pid", "status_protect",
    "status_text_main", "status_text_more", "title", "title-area", "title-icon",
    "tooltip_fast", "tooltip_top", "update_message", "update_message_close",
    "update_message_newver", "update_message_text", "update_message_update", "wnd",
}

ATTACK_IDS = {
    "input_target_ip", "input_target_port", "input_send_message", "input_send_cmd",
    "btn_scan_ip", "btn_scan_port", "btn_send_msg", "btn_send_cmd",
    "btn_send_shutdown", "btn_send_reboot", "result_list",
}


def main() -> int:
    print("=" * 78)
    print("JiYuTrainer 1.7.6 - Jare-Li Refactor regression tests")
    print("=" * 78)

    # Core C++ tests -------------------------------------------------------
    if CORE.exists():
        proc = subprocess.run([str(CORE)], capture_output=True, timeout=60)
        text = proc.stdout.decode("utf-8", "replace")
        core_lines = [line for line in text.splitlines() if line.startswith("TEST|")]
        for line in core_lines:
            _, name, status, detail = (line.split("|", 3) + ["", "", "", ""])[:4]
            record(f"core/{name}", status == "PASS", detail)
        if not core_lines:
            record("core/executable_output", False, "no TEST lines")
        summary = next((line for line in text.splitlines() if line.startswith("SUMMARY|")), "")
        if summary:
            _, passed, failed = (summary.split("|") + ["0", "0"])[:3]
            record("core/summary", int(failed) == 0, f"{passed} passed, {failed} failed")
        else:
            record("core/summary", False, "missing SUMMARY")
    else:
        record("core/executable_exists", False, f"missing {CORE}; run build.ps1 first")

    # Artifacts ------------------------------------------------------------
    check("artifact/exe_exists", lambda: (EXE.exists() and EXE.stat().st_size > 4_000_000,
                                          f"{EXE.stat().st_size if EXE.exists() else 0} bytes"))
    check("artifact/hook_exists", lambda: (HOOK.exists() and HOOK.stat().st_size > 500_000,
                                           f"{HOOK.stat().st_size if HOOK.exists() else 0} bytes"))
    check("artifact/sciter_exists", lambda: (SCITER.exists() and SCITER.stat().st_size > 5_000_000,
                                             f"{SCITER.stat().st_size if SCITER.exists() else 0} bytes"))
    check("artifact/build_script", lambda: (ROOT.joinpath("build.ps1").exists(), "build.ps1"))
    check("artifact/test_script", lambda: (Path(__file__).exists(), "tests/run_tests.py"))

    # PE -------------------------------------------------------------------
    def pe_x86():
        machine, subsystem = pe_info(EXE)
        return machine == 0x14C and subsystem == 2, f"machine=0x{machine:04X} subsystem={subsystem}"

    check("pe/exe_x86_gui", pe_x86)

    def hook_pe():
        machine, _ = pe_info(HOOK)
        return machine == 0x14C, f"machine=0x{machine:04X}"

    check("pe/hook_x86", hook_pe)

    # Version info ---------------------------------------------------------
    try:
        version = version_strings(EXE)
    except Exception as exc:  # noqa: BLE001
        version = {}
        record("version/readable", False, str(exc))
    else:
        record("version/readable", True, "")

    check("version/company_jareli", lambda: version.get("CompanyName") == "Jare-Li",
          version.get("CompanyName", ""))
    check("version/description_jareli", lambda: "Jare-Li" in version.get("FileDescription", ""),
          version.get("FileDescription", ""))
    check("version/copyright", lambda: ("imengyu" in version.get("LegalCopyright", "") and
                                        "Jare-Li" in version.get("LegalCopyright", "")),
          version.get("LegalCopyright", ""))
    check("version/file_version", lambda: version.get("FileVersion", "").startswith("1.7.6.0515"),
          version.get("FileVersion", ""))
    check("version/product_name", lambda: "Jare-Li" in version.get("ProductName", ""),
          version.get("ProductName", ""))

    # Signature ------------------------------------------------------------
    try:
        status, subject = signature_info(EXE)
    except Exception as exc:  # noqa: BLE001
        status, subject = "", str(exc)
    check("signature/present", lambda: bool(status) and status != "NotSigned", status)
    check("signature/subject_jareli", lambda: "CN=Jare-Li" in subject, subject)

    # Main UI --------------------------------------------------------------
    main_html = read_text(UI / "MainWindow.html")
    main_ids = re.findall(r'id="([^"]+)"', main_html)
    check("ui/main_ids_complete", lambda: MAIN_IDS.issubset(set(main_ids)),
          f"missing={sorted(MAIN_IDS - set(main_ids))}")
    check("ui/main_no_duplicate_ids", lambda: len(main_ids) == len(set(main_ids)),
          f"{len(main_ids)} ids")
    check("ui/main_version_label", lambda: "v1.7.6" in main_html, "v1.7.6")
    check("ui/main_window_title", lambda: "<title>JiYu Trainer Main Window</title>" in main_html,
          "must match FindWindow(MAIN_WND_NAME)")
    check("ui/window_16x9_cpp", lambda: "960, 540" in read_text(UI / "MainWindow.cpp", "gbk"),
          "MainWindow must create a 960x540 client window")
    check("ui/window_16x9_html", lambda: "width: 960px; height: 540px" in main_html,
          "card must be fixed 16:9")
    check("ui/attack_window_size", lambda: "896, 760" in read_text(UI / "AttackWindow.cpp", "gbk"),
          "UDP window should be 896x760")
    check("ui/scan_window_size", lambda: "580, 700" in read_text(UI / "ScanIPWindow.cpp", "gbk"),
          "scan window should be 580x700")

    res_refs = set(re.findall(r"res:([^\"')\s]+)", main_html + read_text(UI / "AttackWindow.html")))
    missing_res = [ref for ref in res_refs if not (UI / "res" / ref).exists()]
    check("ui/main_res_refs", lambda: not missing_res, f"missing={missing_res}")

    attack_html = read_text(UI / "AttackWindow.html")
    attack_ids = set(re.findall(r'id="([^"]+)"', attack_html))
    check("ui/attack_ids_complete", lambda: ATTACK_IDS.issubset(attack_ids),
          f"missing={sorted(ATTACK_IDS - attack_ids)}")
    check("ui/attack_warning", lambda: "仅限授权测试" in attack_html, "")
    check("ui/attack_preserved_label", lambda: "原样保留" in attack_html, "")

    # Feature retention ----------------------------------------------------
    main_cpp = read_text(UI / "MainWindow.cpp", "gbk")
    features = [
        "btn_kill", "btn_restart", "link_attact", "jypasswd", "unload_netfilter",
        "link_read_jiyu_password2", "btn_protect_stat", "check_allow_monitor",
        "TrainerWorkerOpVirusBoom", "KillStAuto",
    ]
    app_cpp = read_text(SRC / "JiYuTrainer" / "App.cpp", "gbk")
    feature_source = main_cpp + "\n" + app_cpp
    missing_features = [item for item in features if item not in feature_source]
    check("feature/handlers_present", lambda: not missing_features, f"missing={missing_features}")

    attack_cpp = read_text(SRC / "JiYuTrainer" / "JyUdpAttack.cpp", "gbk")
    packet_calls = [
        "JyPacket::Build(JyPacket::Kind::Message",
        "JyPacket::Build(JyPacket::Kind::Command",
        "JyPacket::Build(JyPacket::Kind::Shutdown",
        "JyPacket::Build(JyPacket::Kind::Reboot",
    ]
    check("attack/packet_calls", lambda: all(call in attack_cpp for call in packet_calls), "")
    check("attack/window_methods", lambda: all(name in read_text(UI / "AttackWindow.cpp", "gbk")
                                                for name in ("SendCommand", "SendText", "SendShutdown", "SendReboot")))

    # Resources ------------------------------------------------------------
    try:
        main_res = html_resource(EXE, 103)
        attack_res = html_resource(EXE, 150)
    except Exception as exc:  # noqa: BLE001
        record("resource/html_embedded", False, str(exc))
    else:
        check("resource/main_html", lambda: len(main_res) > 30_000 and b"JiYu Trainer" in main_res,
              f"{len(main_res)} bytes")
        check("resource/attack_html", lambda: len(attack_res) > 5_000 and "UDP".encode() in attack_res,
              f"{len(attack_res)} bytes")

    # License / attribution ------------------------------------------------
    license_text = read_text(SRC / "LICENSE")
    readme = read_text(ROOT / "README-REFACTOR.md")
    notice = read_text(SRC / "NOTICE")
    check("legal/mit_license", lambda: "MIT License" in license_text and "imengyu" in license_text, "")
    check("legal/readme_attribution", lambda: "imengyu" in readme and "Jare-Li" in readme, "")
    check("legal/notice_attribution", lambda: "imengyu" in notice and "Jare-Li" in notice, "")

    # Version consistency --------------------------------------------------
    app_public = read_text(SRC / "JiYuTrainer" / "AppPublic.h", "gbk")
    check("version/current_constant", lambda: 'CURRENT_VERSION "1.7.6.0515"' in app_public, "")

    # Summary --------------------------------------------------------------
    total = len(RESULTS)
    passed = sum(1 for _, ok, _ in RESULTS if ok)
    failed = total - passed
    rate = (passed / total * 100.0) if total else 0.0

    print("-" * 78)
    print(f"RESULT: {passed}/{total} passed, {failed} failed, {rate:.1f}%")
    print("-" * 78)

    report = [
        "# Test Report",
        "",
        f"- Date: {datetime.now().isoformat(timespec='seconds')}",
        f"- Passed: {passed}",
        f"- Failed: {failed}",
        f"- Total: {total}",
        f"- Pass rate: {rate:.1f}%",
        "",
        "| Test | Result | Detail |",
        "|---|---|---|",
    ]
    for name, ok, detail in RESULTS:
        safe_detail = detail.replace("|", "\\|")
        report.append(f"| `{name}` | {'PASS' if ok else 'FAIL'} | {safe_detail} |")
    (ROOT / "test-report.md").write_text("\n".join(report) + "\n", encoding="utf-8")
    (ROOT / "test-results.json").write_text(
        json.dumps(
            {
                "date": datetime.now().isoformat(timespec="seconds"),
                "passed": passed,
                "failed": failed,
                "total": total,
                "pass_rate": round(rate, 1),
                "tests": [{"name": n, "ok": o, "detail": d} for n, o, d in RESULTS],
            },
            ensure_ascii=False,
            indent=2,
        ) + "\n",
        encoding="utf-8",
    )

    return 0 if rate >= 90.0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
