#pragma once

#include <string>
#include <vector>

namespace IpUtils {

// Strict IPv4 text check: exactly four decimal octets, each 0..255.
// Surrounding ASCII whitespace is ignored; inner whitespace is rejected.
bool IsValidIPv4(const std::wstring& text);

// Splits a comma-separated target list, trims ASCII whitespace and drops
// empty entries. The original UI used commas only, so this is compatible.
std::vector<std::wstring> SplitIpList(const std::wstring& text);

// Trims ASCII whitespace from both ends.
std::wstring Trim(const std::wstring& text);

// Builds the inclusive host range base.start .. base.end, e.g.
// BuildScanRange(L"192.168.1", 0, 255) yields 256 addresses.
// Invalid/negative octets are clamped; an inverted range yields an empty list.
std::vector<std::wstring> BuildScanRange(const std::wstring& base, int start, int end);

} // namespace IpUtils
