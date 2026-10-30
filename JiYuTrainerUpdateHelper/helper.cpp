// JiYuTrainerUpdateHelper - independent elevated update helper.
// Usage:
//   JiYuTrainerUpdateHelper.exe --zip <zip> --dir <install-dir> --pid <pid>
//       --hash <sha256> --restart <exe> [--log <log>]

#include <windows.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <cstdio>

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

using std::wstring;
using std::vector;

struct Args {
    wstring zip;
    wstring dir;
    wstring restart;
    wstring hash;
    wstring log;
    DWORD pid = 0;
};

static FILE* g_log = nullptr;

static void Log(const wchar_t* fmt, ...) {
    if (!g_log) return;
    va_list ap;
    va_start(ap, fmt);
    vfwprintf_s(g_log, fmt, ap);
    va_end(ap);
    fwprintf_s(g_log, L"\n");
    fflush(g_log);
}

static wstring GetTempDir() {
    wchar_t buf[MAX_PATH + 1] = { 0 };
    DWORD n = GetTempPathW(MAX_PATH, buf);
    if (!n) return L".";
    return wstring(buf, n);
}

static wstring Join(const wstring& a, const wstring& b) {
    if (a.empty()) return b;
    wchar_t last = a[a.size() - 1];
    if (last == L'\\' || last == L'/') return a + b;
    return a + L"\\" + b;
}

static bool FileExists(const wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

static bool DirExists(const wstring& path) {
    DWORD attr = GetFileAttributesW(path.c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

static bool EnsureDir(const wstring& path) {
    if (DirExists(path)) return true;
    wstring parent = path;
    size_t pos = parent.find_last_of(L"\\/");
    if (pos != wstring::npos && pos > 2) {
        if (!EnsureDir(parent.substr(0, pos))) return false;
    }
    return CreateDirectoryW(path.c_str(), nullptr) != FALSE ||
           GetLastError() == ERROR_ALREADY_EXISTS;
}

static bool Sha256File(const wstring& path, wstring& outHex) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    bool ok = false;
    DWORD objLen = 0, cb = 0;
    vector<BYTE> obj, digest(32);
    BYTE buf[64 * 1024];
    wchar_t hex[3] = { 0 };

    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) goto done;
    if (BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objLen, sizeof(objLen), &cb, 0) < 0) goto done;
    obj.resize(objLen);
    if (BCryptCreateHash(alg, &hash, obj.data(), objLen, nullptr, 0, 0) < 0) goto done;

    for (;;) {
        DWORD read = 0;
        if (!ReadFile(file, buf, sizeof(buf), &read, nullptr)) goto done;
        if (!read) break;
        if (BCryptHashData(hash, buf, read, 0) < 0) goto done;
    }
    if (BCryptFinishHash(hash, digest.data(), (ULONG)digest.size(), 0) < 0) goto done;

    outHex.clear();
    for (BYTE b : digest) {
        swprintf_s(hex, L"%02X", b);
        outHex += hex;
    }
    ok = true;

done:
    if (hash) BCryptDestroyHash(hash);
    if (alg) BCryptCloseAlgorithmProvider(alg, 0);
    CloseHandle(file);
    return ok;
}

static bool WaitForProcess(DWORD pid, DWORD timeoutMs) {
    if (!pid) return true;
    HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) {
        Log(L"OpenProcess(%lu) failed: %lu", pid, GetLastError());
        return true; // process already gone or inaccessible; continue
    }
    DWORD r = WaitForSingleObject(h, timeoutMs);
    CloseHandle(h);
    if (r == WAIT_OBJECT_0) return true;
    Log(L"wait pid %lu timeout/result=%lu", pid, r);
    return false;
}

static bool RunAndWait(const wstring& exe, const wstring& cmd, const wstring& workDir) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    vector<wchar_t> mutableCmd(cmd.begin(), cmd.end());
    mutableCmd.push_back(L'\0');
    BOOL ok = CreateProcessW(exe.c_str(), mutableCmd.data(), nullptr, nullptr, FALSE,
                             CREATE_NO_WINDOW, nullptr, workDir.empty() ? nullptr : workDir.c_str(),
                             &si, &pi);
    if (!ok) {
        Log(L"CreateProcess failed: %s (err=%lu)", exe.c_str(), GetLastError());
        return false;
    }
    WaitForSingleObject(pi.hProcess, 120000);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    Log(L"process exit: %s code=%lu", exe.c_str(), code);
    return code == 0;
}

static bool ExtractZip(const wstring& zip, const wstring& dest) {
    if (!EnsureDir(dest)) return false;

    wchar_t sys[MAX_PATH] = { 0 };
    GetSystemDirectoryW(sys, MAX_PATH);
    wstring tar = Join(sys, L"tar.exe");
    if (FileExists(tar)) {
        wstring cmd = L"\"" + tar + L"\" -xf \"" + zip + L"\" -C \"" + dest + L"\"";
        if (RunAndWait(tar, cmd, dest)) return true;
        Log(L"tar extraction failed, trying PowerShell");
    }

    wstring ps = Join(sys, L"WindowsPowerShell\\v1.0\\powershell.exe");
    if (!FileExists(ps)) ps = L"powershell.exe";
    wstring z = zip, d = dest;
    // PowerShell single-quoted escaping.
    for (size_t i = 0; i < z.size(); ++i) if (z[i] == L'\'') z.insert(i++, 1, L'\'');
    for (size_t i = 0; i < d.size(); ++i) if (d[i] == L'\'') d.insert(i++, 1, L'\'');
    wstring cmd = L"\"" + ps + L"\" -NoProfile -ExecutionPolicy Bypass -Command "
                  L"\"Expand-Archive -LiteralPath '" + z + L"' -DestinationPath '" + d + L"' -Force\"";
    return RunAndWait(ps, cmd, dest);
}

struct BackupItem {
    wstring dest;
    wstring backup;
    bool existed = false;
};

static bool CopyTree(const wstring& src, const wstring& destRoot, const wstring& backupRoot,
                     vector<BackupItem>& backups) {
    wstring pattern = Join(src, L"*");
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return false;

    bool ok = true;
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
        wstring s = Join(src, fd.cFileName);
        wstring d = Join(destRoot, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!EnsureDir(d) || !CopyTree(s, d, Join(backupRoot, fd.cFileName), backups)) {
                ok = false;
                break;
            }
        } else {
            if (wcscmp(fd.cFileName, L"JiYuTrainer.ini") == 0 ||
                wcscmp(fd.cFileName, L"JiYuTrainer.log") == 0) {
                continue;
            }
            BackupItem item;
            item.dest = d;
            item.existed = FileExists(d);
            if (item.existed) {
                item.backup = Join(backupRoot, fd.cFileName);
                if (!EnsureDir(backupRoot) || !CopyFileW(d.c_str(), item.backup.c_str(), FALSE)) {
                    Log(L"backup failed: %s (err=%lu)", d.c_str(), GetLastError());
                    ok = false;
                    break;
                }
            }
            if (!CopyFileW(s.c_str(), d.c_str(), FALSE)) {
                Log(L"copy failed: %s -> %s (err=%lu)", s.c_str(), d.c_str(), GetLastError());
                ok = false;
                break;
            }
            backups.push_back(item);
        }
    } while (FindNextFileW(h, &fd));

    FindClose(h);
    return ok;
}

static void Rollback(const vector<BackupItem>& backups) {
    for (auto it = backups.rbegin(); it != backups.rend(); ++it) {
        if (it->existed) {
            CopyFileW(it->backup.c_str(), it->dest.c_str(), FALSE);
        } else {
            DeleteFileW(it->dest.c_str());
        }
    }
}

static bool ParseArgs(Args& a) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return false;
    for (int i = 1; i < argc; ++i) {
        wstring k = argv[i];
        if (i + 1 >= argc) break;
        wstring v = argv[++i];
        if (k == L"--zip") a.zip = v;
        else if (k == L"--dir") a.dir = v;
        else if (k == L"--restart") a.restart = v;
        else if (k == L"--hash") a.hash = v;
        else if (k == L"--log") a.log = v;
        else if (k == L"--pid") a.pid = (DWORD)_wtoi(v.c_str());
    }
    LocalFree(argv);
    return !a.zip.empty() && !a.dir.empty() && !a.hash.empty() && a.pid != 0;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
    Args a;
    wstring logPath = Join(GetTempDir(), L"JiYuTrainerUpdateHelper.log");
    if (!ParseArgs(a)) {
        MessageBoxW(nullptr, L"更新参数不完整。", L"JiYuTrainer 更新", MB_ICONERROR);
        return 2;
    }
    if (!a.log.empty()) logPath = a.log;
    _wfopen_s(&g_log, logPath.c_str(), L"a, ccs=UTF-8");
    Log(L"=== helper start pid=%lu ===", a.pid);
    Log(L"zip=%s", a.zip.c_str());
    Log(L"dir=%s", a.dir.c_str());

    if (!WaitForProcess(a.pid, 90000)) {
        MessageBoxW(nullptr, L"等待主程序退出超时，更新已取消。", L"JiYuTrainer 更新", MB_ICONERROR);
        return 3;
    }

    wstring actual;
    if (!Sha256File(a.zip, actual)) {
        MessageBoxW(nullptr, L"无法读取更新包。", L"JiYuTrainer 更新", MB_ICONERROR);
        return 4;
    }
    if (_wcsicmp(actual.c_str(), a.hash.c_str()) != 0) {
        Log(L"hash mismatch: actual=%s expected=%s", actual.c_str(), a.hash.c_str());
        MessageBoxW(nullptr, L"更新包 SHA-256 校验失败，已取消。", L"JiYuTrainer 更新", MB_ICONERROR);
        return 5;
    }

    wstring temp = Join(GetTempDir(), L"JiYuTrainerUpdate_" + std::to_wstring(a.pid));
    if (DirExists(temp)) {
        // best effort cleanup
        RunAndWait(L"cmd.exe", L"cmd.exe /c rmdir /s /q \"" + temp + L"\"", GetTempDir());
    }
    if (!ExtractZip(a.zip, temp)) {
        MessageBoxW(nullptr, L"解压更新包失败。", L"JiYuTrainer 更新", MB_ICONERROR);
        return 6;
    }
    if (!FileExists(Join(temp, L"JiYuTrainer.exe"))) {
        MessageBoxW(nullptr, L"更新包内容不完整（缺少 JiYuTrainer.exe）。", L"JiYuTrainer 更新", MB_ICONERROR);
        return 7;
    }

    wchar_t stamp[32];
    SYSTEMTIME st;
    GetLocalTime(&st);
    swprintf_s(stamp, L"%04d%02d%02d-%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    wstring backup = Join(a.dir, wstring(L"backup-") + stamp);
    vector<BackupItem> backups;

    if (!CopyTree(temp, a.dir, backup, backups)) {
        Log(L"apply failed; rolling back %zu files", backups.size());
        Rollback(backups);
        MessageBoxW(nullptr, L"替换文件失败，已尝试回滚。请查看更新日志。", L"JiYuTrainer 更新", MB_ICONERROR);
        return 8;
    }

    Log(L"update applied, files=%zu, backup=%s", backups.size(), backup.c_str());
    if (!a.restart.empty() && FileExists(a.restart)) {
        ShellExecuteW(nullptr, L"open", a.restart.c_str(), nullptr, a.dir.c_str(), SW_SHOWNORMAL);
    }
    Log(L"=== helper done ===");
    return 0;
}
