#include "stdafx.h"
#include "IpUtils.h"

namespace IpUtils {

namespace {

bool IsAsciiSpace(wchar_t c) {
    return c == L' ' || c == L'\t' || c == L'\r' || c == L'\n' || c == L'\v' || c == L'\f';
}

} // namespace

std::wstring Trim(const std::wstring& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && IsAsciiSpace(text[begin])) ++begin;
    while (end > begin && IsAsciiSpace(text[end - 1])) --end;
    return text.substr(begin, end - begin);
}

bool IsValidIPv4(const std::wstring& text) {
    const std::wstring value = Trim(text);
    if (value.empty()) return false;

    int octets = 0;
    size_t pos = 0;
    while (pos <= value.size()) {
        const size_t dot = value.find(L'.', pos);
        const size_t end = (dot == std::wstring::npos) ? value.size() : dot;
        if (end == pos) return false;              // empty octet
        if (end - pos > 3) return false;           // too many digits

        int number = 0;
        for (size_t i = pos; i < end; ++i) {
            const wchar_t c = value[i];
            if (c < L'0' || c > L'9') return false;
            number = number * 10 + static_cast<int>(c - L'0');
        }
        if (number > 255) return false;

        ++octets;
        if (dot == std::wstring::npos) break;
        pos = dot + 1;
    }

    return octets == 4;
}

std::vector<std::wstring> SplitIpList(const std::wstring& text) {
    std::vector<std::wstring> result;
    size_t pos = 0;
    while (pos <= text.size()) {
        const size_t comma = text.find(L',', pos);
        const size_t end = (comma == std::wstring::npos) ? text.size() : comma;
        std::wstring item = Trim(text.substr(pos, end - pos));
        if (!item.empty()) result.push_back(item);
        if (comma == std::wstring::npos) break;
        pos = comma + 1;
    }
    return result;
}

std::vector<std::wstring> BuildScanRange(const std::wstring& base, int start, int end) {
    std::vector<std::wstring> result;
    if (start < 0) start = 0;
    if (end > 255) end = 255;
    if (end < start) return result;

    result.reserve(static_cast<size_t>(end - start + 1));
    for (int i = start; i <= end; ++i) {
        wchar_t suffix[8] = {0};
        swprintf_s(suffix, L".%d", i);
        result.push_back(base + suffix);
    }
    return result;
}

} // namespace IpUtils
