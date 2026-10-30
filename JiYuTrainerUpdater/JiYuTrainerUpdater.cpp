// JiYuTrainerUpdater.cpp - GitHub Releases updater.
// Downloads the release zip + helper + SHA256SUMS.txt, verifies them,
// then launches the elevated update helper.

#include "stdafx.h"
#include "JiYuTrainerUpdater.h"
#include "../JiYuTrainer/AppPublic.h"
#include "../JiYuTrainer/JiYuTrainer.h"
#include "../JiYuTrainer/StringHlp.h"
#include "../JiYuTrainer/PathHelper.h"
#include "../JiYuTrainer/SysHlp.h"
#include "NetUtils.h"
#include <bcrypt.h>
#include <time.h>
#include <Wininet.h>
#include <shellapi.h>
#include <vector>
#include <string>
#include <cstdio>

#pragma comment(lib, "bcrypt.lib")

using namespace std;

#define GITHUB_API "https://api.github.com/repos/Jare-Li/JiYuTrainer-JareLi/releases/latest"

JTApp* appCurrent = nullptr;
Logger* appLogger = nullptr;
WCHAR newVer[64];

static bool g_releaseFetched = false;
static string g_tag, g_body, g_zipUrl, g_helperUrl, g_sumsUrl;
static wstring g_zipPath, g_helperPath, g_sumsPath, g_expectedHash, g_helperHash;

static string JsonString(const string& js, const string& key) {
    size_t k = js.find("\"" + key + "\"");
    if (k == string::npos) return "";
    size_t c = js.find(':', k);
    if (c == string::npos) return "";
    size_t q = js.find('"', c + 1);
    if (q == string::npos) return "";
    string out;
    for (size_t i = q + 1; i < js.size(); ++i) {
        char ch = js[i];
        if (ch == '"') break;
        if (ch == '\\' && i + 1 < js.size()) {
            char e = js[++i];
            switch (e) {
            case 'n': out += '\n'; break;
            case 'r': out += '\r'; break;
            case 't': out += '\t'; break;
            case '"': out += '"'; break;
            case '\\': out += '\\'; break;
            case '/': out += '/'; break;
            default: out += e; break;
            }
        } else {
            out += ch;
        }
    }
    return out;
}

static string FindAsset(const string& js, const string& needle) {
    size_t pos = 0;
    while ((pos = js.find("\"browser_download_url\"", pos)) != string::npos) {
        size_t c = js.find(':', pos);
        if (c == string::npos) break;
        size_t q = js.find('"', c + 1);
        if (q == string::npos) break;
        size_t e = js.find('"', q + 1);
        if (e == string::npos) break;
        string url = js.substr(q + 1, e - q - 1);
        if (url.find(needle) != string::npos)
            return url;
        pos = e + 1;
    }
    return "";
}

static vector<int> ParseVersion(const string& s) {
    vector<int> v;
    int n = 0;
    bool digit = false;
    for (char c : s) {
        if (c >= '0' && c <= '9') {
            n = n * 10 + (c - '0');
            digit = true;
        } else if (c == '.') {
            if (digit) { v.push_back(n); n = 0; digit = false; }
        } else if (digit) {
            break;
        }
    }
    if (digit) v.push_back(n);
    while (v.size() < 4) v.push_back(0);
    return v;
}

static int CompareVersion(const string& a, const string& b) {
    auto x = ParseVersion(a), y = ParseVersion(b);
    for (size_t i = 0; i < 4; ++i) {
        if (x[i] < y[i]) return -1;
        if (x[i] > y[i]) return 1;
    }
    return 0;
}

static bool FetchRelease() {
    if (g_releaseFetched) return true;
    string js;
    CURLcode res = curl_get_req(GITHUB_API, js);
    if (res != CURLE_OK || js.empty()) {
        if (appLogger) appLogger->LogError(L"GitHub release request failed: %S", curl_easy_strerror(res));
        return false;
    }
    g_tag = JsonString(js, "tag_name");
    g_body = JsonString(js, "body");
    g_zipUrl = FindAsset(js, "JiYuTrainer-JareLi");
    g_helperUrl = FindAsset(js, "JiYuTrainerUpdateHelper.exe");
    g_sumsUrl = FindAsset(js, "SHA256SUMS.txt");
    if (g_tag.empty() || g_zipUrl.empty() || g_helperUrl.empty() || g_sumsUrl.empty()) {
        if (appLogger) appLogger->LogError(L"GitHub release assets incomplete. tag=%S zip=%S helper=%S sums=%S",
            g_tag.c_str(), g_zipUrl.c_str(), g_helperUrl.c_str(), g_sumsUrl.c_str());
        return false;
    }
    g_releaseFetched = true;
    return true;
}

static bool Sha256File(const wstring& path, wstring& hex) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    DWORD objLen = 0, cb = 0;
    vector<BYTE> obj, digest(32);
    BYTE buf[65536];
    bool ok = false;
    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) goto done;
    if (BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objLen, sizeof(objLen), &cb, 0) < 0) goto done;
    obj.resize(objLen);
    if (BCryptCreateHash(alg, &hash, obj.data(), objLen, nullptr, 0, 0) < 0) goto done;
    for (;;) {
        DWORD read = 0;
        if (!ReadFile(f, buf, sizeof(buf), &read, nullptr)) goto done;
        if (!read) break;
        if (BCryptHashData(hash, buf, read, 0) < 0) goto done;
    }
    if (BCryptFinishHash(hash, digest.data(), (ULONG)digest.size(), 0) < 0) goto done;
    hex.clear();
    wchar_t part[3];
    for (BYTE b : digest) { swprintf_s(part, L"%02X", b); hex += part; }
    ok = true;
done:
    if (hash) BCryptDestroyHash(hash);
    if (alg) BCryptCloseAlgorithmProvider(alg, 0);
    CloseHandle(f);
    return ok;
}

static wstring Join(const wstring& a, const wstring& b) {
    if (a.empty()) return b;
    wchar_t c = a[a.size() - 1];
    return (c == L'\\' || c == L'/') ? a + b : a + L"\\" + b;
}

static bool HttpDownload(const string& url, const wstring& path) {
    if (url.empty()) return false;
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    FILE* f = nullptr;
    _wfopen_s(&f, path.c_str(), L"wb");
    if (!f) { curl_easy_cleanup(curl); return false; }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "JiYuTrainer-Updater/1.7.9");
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 180L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
    CURLcode res = curl_easy_perform(curl);
    fclose(f);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        if (appLogger) appLogger->LogError(L"download failed: %S (%S)", url.c_str(), curl_easy_strerror(res));
        DeleteFileW(path.c_str());
        return false;
    }
    return true;
}

static bool LoadSums() {
    FILE* f = nullptr;
    _wfopen_s(&f, g_sumsPath.c_str(), L"rb");
    if (!f) return false;
    char line[512];
    bool zip = false, helper = false;
    while (fgets(line, sizeof(line), f)) {
        char h[65] = { 0 }, n[256] = { 0 };
        if (sscanf_s(line, "%64s %255s", h, (unsigned)sizeof(h), n, (unsigned)sizeof(n)) != 2) continue;
        if (n[0] == '*') memmove(n, n + 1, strlen(n));
        wstring name = StringHlp::AnsiToUnicode(n);
        if (_wcsicmp(name.c_str(), L"JiYuTrainer-JareLi-1.7.9.zip") == 0 ||
            (wcsstr(name.c_str(), L"JiYuTrainer-JareLi-") && wcsstr(name.c_str(), L".zip"))) {
            g_expectedHash = StringHlp::AnsiToUnicode(h);
            zip = true;
        } else if (_wcsicmp(name.c_str(), L"JiYuTrainerUpdateHelper.exe") == 0) {
            g_helperHash = StringHlp::AnsiToUnicode(h);
            helper = true;
        }
    }
    fclose(f);
    return zip && helper;
}

// ---------------------------------------------------------------------------

UPEXPORT_CFUNC(BOOL) JUpdater_CheckInternet() {
    return InternetGetConnectedState(NULL, 0);
}

UPEXPORT_CFUNC(BOOL) JUpdater_CheckUpdate(bool byUser) {
    appCurrent = (JTApp*)GetApp();
    appLogger = appCurrent->GetLogger();
    if (!byUser && CheckLastUpdateDate(appCurrent->GetPartFullPath(PART_INI)))
        return UPDATE_STATUS_CHECKED;
    return CheckServerForUpdateInfo(byUser);
}

UPEXPORT_CFUNC(BOOL) JUpdater_GetUpdateNew(LPCSTR buffer, size_t maxCount) {
    return CheckServerForUpdateNew(buffer, maxCount);
}

UPEXPORT_CFUNC(LPCWSTR) JUpdater_GetUpdateNewVer() {
    CheckServerForUpdateVer();
    return newVer;
}

int CheckServerForUpdateInfo(bool byUser) {
    if (!FetchRelease()) return UPDATE_STATUS_COULD_NOT_CONNECT;
    if (CompareVersion(g_tag, CURRENT_VERSION) > 0) return UPDATE_STATUS_HAS_UPDATE;
    return UPDATE_STATUS_LATEST;
}

BOOL CheckServerForUpdateNew(LPCSTR buffer, size_t maxCount) {
    if (!FetchRelease()) return FALSE;
    string text = g_body.empty() ? "发现新版本。" : g_body;
    strncpy_s((char*)buffer, maxCount, text.c_str(), _TRUNCATE);
    return TRUE;
}

BOOL CheckServerForUpdateVer() {
    if (!FetchRelease()) return FALSE;
    LPCWSTR w = StringHlp::AnsiToUnicode(g_tag.c_str());
    wcscpy_s(newVer, 64, w);
    FreeStringPtr(w);
    return TRUE;
}

bool CheckLastUpdateDate(LPCWSTR iniPath) {
    if (Path::Exists(iniPath)) {
        WCHAR w[32], w2[32], wnow[32];
        GetPrivateProfileString(L"JTSettings", L"LastCheckUpdateTime", L"00/00", w, 32, iniPath);
        GetPrivateProfileString(L"JTSettings", L"AlwaysCheckUpdate", L"FALSE", w2, 32, iniPath);
        time_t tt = time(NULL);
        tm t;
        localtime_s(&t, &tt);
        swprintf_s(wnow, L"%00d/%00d", t.tm_mon + 1, t.tm_mday);
        if (StrEqual(w, wnow)) {
            if (StrEqual(w2, L"TRUE")) return false;
            return true;
        }
        WritePrivateProfileString(L"JTSettings", L"LastCheckUpdateTime", wnow, iniPath);
    }
    return false;
}

// ---------------------------------------------------------------------------

HANDLE updateThread = NULL;
bool updateing = false;
bool updateCanceled = false;
UpdateDownloadCallback callBack = nullptr;
LPARAM callBackLparam = NULL;
WCHAR updateFilePath[MAX_PATH] = { 0 };
string downloadFileUrl;
FILE* updateFile = NULL;

UPEXPORT_CFUNC(BOOL) JUpdater_DownLoadUpdateFile(UpdateDownloadCallback cb, LPARAM lparam) {
    ::callBack = cb;
    callBackLparam = lparam;
    updateCanceled = false;
    if (!FetchRelease()) {
        cb(0, lparam, UPDATE_STATUS_COULD_NOT_CONNECT);
        return FALSE;
    }

    wchar_t tmp[MAX_PATH] = { 0 };
    GetTempPathW(MAX_PATH, tmp);
    wstring dir = Join(tmp, L"JiYuTrainerUpdate");
    CreateDirectoryW(dir.c_str(), nullptr);
    g_zipPath = Join(dir, L"JiYuTrainer-update.zip");
    g_helperPath = Join(dir, L"JiYuTrainerUpdateHelper.exe");
    g_sumsPath = Join(dir, L"SHA256SUMS.txt");
    wcsncpy_s(updateFilePath, g_zipPath.c_str(), _TRUNCATE);
    downloadFileUrl = g_zipUrl;

    updateThread = CreateThread(NULL, 0, UpdateDownloadThread, NULL, 0, NULL);
    return updateThread != NULL;
}

UPEXPORT_CFUNC(BOOL) JUpdater_Updatering() {
    return updateing;
}

UPEXPORT_CFUNC(BOOL) JUpdater_CancelDownLoadUpdateFile() {
    if (!updateing) return FALSE;
    if (updateThread) {
        TerminateThread(updateThread, 0);
        updateThread = NULL;
    }
    updateCanceled = true;
    if (updateFile) { fclose(updateFile); updateFile = NULL; }
    if (Path::Exists(updateFilePath)) DeleteFileW(updateFilePath);
    return TRUE;
}

UPEXPORT_CFUNC(BOOL) JUpdater_RunInstallion() {
    if (!Path::Exists(g_helperPath) || !Path::Exists(g_zipPath)) {
        MessageBoxW(nullptr, L"更新组件不完整，无法启动安装。", L"JiYuTrainer 更新", MB_ICONERROR);
        return FALSE;
    }
    wstring dir = appCurrent->GetCurrentDir();
    wstring exe = appCurrent->GetFullPath();
    wstring args = L"--zip \"" + g_zipPath + L"\" --dir \"" + dir +
        L"\" --pid " + to_wstring(GetCurrentProcessId()) +
        L" --hash " + g_expectedHash + L" --restart \"" + exe + L"\"";
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = g_helperPath.c_str();
    sei.lpParameters = args.c_str();
    sei.nShow = SW_HIDE;
    if (!ShellExecuteExW(&sei)) {
        DWORD err = GetLastError();
        if (err != ERROR_CANCELLED)
            MessageBoxW(nullptr, L"无法启动更新程序，请尝试以管理员身份运行。", L"JiYuTrainer 更新", MB_ICONERROR);
        return FALSE;
    }
    if (sei.hProcess) CloseHandle(sei.hProcess);
    return TRUE;
}

DWORD WINAPI UpdateDownloadThread(LPVOID) {
    updateing = true;
    Sleep(500);
    bool ok = false;
    string zipUrl = downloadFileUrl;

    CURL* curl = curl_easy_init();
    FILE* f = nullptr;
    if (curl) {
        _wfopen_s(&f, updateFilePath, L"wb");
        if (f) {
            curl_easy_setopt(curl, CURLOPT_URL, zipUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "JiYuTrainer-Updater/1.7.9");
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 180L);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, UpdateProgressFunc);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
            CURLcode res = curl_easy_perform(curl);
            fclose(f);
            f = nullptr;
            ok = (res == CURLE_OK);
        }
        curl_easy_cleanup(curl);
    }

    if (ok) {
        ok = HttpDownload(g_helperUrl, g_helperPath) &&
             HttpDownload(g_sumsUrl, g_sumsPath) &&
             LoadSums();
        if (ok) {
            wstring h;
            ok = Sha256File(g_zipPath, h) && _wcsicmp(h.c_str(), g_expectedHash.c_str()) == 0;
            if (ok) {
                wstring hh;
                ok = Sha256File(g_helperPath, hh) && _wcsicmp(hh.c_str(), g_helperHash.c_str()) == 0;
            }
        }
    }

    if (!ok) {
        if (callBack) callBack(0, callBackLparam, UPDATE_STATUS_COULD_NOT_CONNECT);
    } else {
        if (callBack) callBack(L"100%", callBackLparam, UPDATE_STATUS_FINISHED);
    }
    updateing = false;
    updateThread = NULL;
    return 0;
}

int UpdateProgressFunc(void*, double total, double now, double, double) {
    wchar_t text[10];
    double p = total > 0 ? now / total : 0;
    if (p >= 1) wcscpy_s(text, L"100%");
    else swprintf_s(text, L"%3.0f%%", p * 100);
    if (callBack) callBack(text, callBackLparam, UPDATE_STATUS_DWONLAODING);
    return 0;
}
