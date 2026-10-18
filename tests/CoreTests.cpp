// Core regression tests for the Jare-Li refactor.
// Prints machine-readable lines consumed by tests/run_tests.py.
#include "IpUtils.h"
#include "JyPacket.h"

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>

namespace {

int g_pass = 0;
int g_fail = 0;

void Check(const char* name, bool ok, const std::string& detail = std::string()) {
    if (ok) ++g_pass; else ++g_fail;
    std::printf("TEST|%s|%s|%s\n", name, ok ? "PASS" : "FAIL", detail.c_str());
}

std::uint32_t Fnv1a(const unsigned char* data, size_t size) {
    std::uint32_t hash = 2166136261u;
    for (size_t i = 0; i < size; ++i) {
        hash ^= data[i];
        hash *= 16777619u;
    }
    return hash;
}

std::vector<unsigned char> BuildPacket(JyPacket::Kind kind, const std::wstring& payload) {
    std::vector<unsigned char> buffer(static_cast<size_t>(JyPacket::kSendBufferSize), 0xCC);
    JyPacket::Build(kind, payload, buffer.data(), buffer.size());
    return buffer;
}

bool BytesEqual(const std::vector<unsigned char>& buffer, size_t offset,
                const unsigned char* expected, size_t size) {
    if (offset + size > buffer.size()) return false;
    for (size_t i = 0; i < size; ++i)
        if (buffer[offset + i] != expected[i]) return false;
    return true;
}

} // namespace

int main() {
    // IPv4 validation
    Check("ip_valid_basic", IpUtils::IsValidIPv4(L"192.168.1.1"));
    Check("ip_valid_bounds", IpUtils::IsValidIPv4(L"0.0.0.0") && IpUtils::IsValidIPv4(L"255.255.255.255"));
    Check("ip_invalid_256", !IpUtils::IsValidIPv4(L"192.168.1.256"));
    Check("ip_invalid_short", !IpUtils::IsValidIPv4(L"192.168.1"));
    Check("ip_invalid_five", !IpUtils::IsValidIPv4(L"1.2.3.4.5"));
    Check("ip_invalid_letters", !IpUtils::IsValidIPv4(L"192.168.1.a"));
    Check("ip_invalid_empty", !IpUtils::IsValidIPv4(L""));
    Check("ip_invalid_inner_space", !IpUtils::IsValidIPv4(L"192.168 .1.1"));
    Check("ip_trim_ok", IpUtils::IsValidIPv4(L"  10.0.0.1\t"));
    Check("ip_leading_zero_ok", IpUtils::IsValidIPv4(L"010.001.001.001"));

    // Target list splitting
    const auto two = IpUtils::SplitIpList(L"10.0.0.1, 10.0.0.2");
    Check("split_two", two.size() == 2 && two[0] == L"10.0.0.1" && two[1] == L"10.0.0.2");
    const auto messy = IpUtils::SplitIpList(L" 10.0.0.1 ,, 10.0.0.2 , ");
    Check("split_spaces_and_empty", messy.size() == 2 && messy[0] == L"10.0.0.1" && messy[1] == L"10.0.0.2");
    Check("split_empty", IpUtils::SplitIpList(L"   ").empty());
    Check("trim", IpUtils::Trim(L" \t abc \r\n") == L"abc");

    // Base packet identities
    const unsigned char header[8] = {0x44, 0x4d, 0x4f, 0x43, 0x00, 0x00, 0x01, 0x00};
    for (int i = 0; i < 4; ++i) {
        const auto kind = static_cast<JyPacket::Kind>(i);
        const unsigned char* base = JyPacket::BasePacket(kind);
        Check((std::string("basepack_header_") + std::to_string(i)).c_str(),
              base != nullptr && BytesEqual(std::vector<unsigned char>(base, base + 128), 0, header, 8));
    }

    const std::uint32_t hashes[4] = {0x28135a31u, 0x20fab8f6u, 0xcd22edd4u, 0xddcac8d6u};
    for (int i = 0; i < 4; ++i) {
        const auto kind = static_cast<JyPacket::Kind>(i);
        const unsigned char* base = JyPacket::BasePacket(kind);
        Check((std::string("basepack_hash_") + std::to_string(i)).c_str(),
              base != nullptr && Fnv1a(base, 128) == hashes[i]);
    }

    Check("packet_control_offsets", JyPacket::PayloadOffset(JyPacket::Kind::Reboot) == -1 &&
                                        JyPacket::PayloadOffset(JyPacket::Kind::Shutdown) == -1);

    // Message packet: payload starts at byte 56.
    {
        const auto packet = BuildPacket(JyPacket::Kind::Message, L"Hello");
        const unsigned char expected[10] = {'H', 0, 'e', 0, 'l', 0, 'l', 0, 'o', 0};
        Check("packet_message_offset", BytesEqual(packet, 56, expected, sizeof(expected)));
        const auto base = BuildPacket(JyPacket::Kind::Message, L"");
        Check("packet_message_prefix", BytesEqual(packet, 0, base.data(), 56));
        Check("packet_message_zero_tail", packet[66] == 0 && packet[67] == 0);
    }

    // Command packet: payload starts at byte 100.
    {
        const auto packet = BuildPacket(JyPacket::Kind::Command, L"cmd /c calc");
        const unsigned char expected[12] = {'c', 0, 'm', 0, 'd', 0, ' ', 0, '/', 0, 'c', 0};
        Check("packet_command_offset", BytesEqual(packet, 100, expected, sizeof(expected)));
        const auto base = BuildPacket(JyPacket::Kind::Command, L"");
        Check("packet_command_prefix", BytesEqual(packet, 0, base.data(), 100));
    }

    // UTF-16LE encoding.
    {
        const auto packet = BuildPacket(JyPacket::Kind::Message, L"\u4F60\u597D");
        const unsigned char expected[4] = {0x60, 0x4F, 0x7D, 0x59};
        Check("packet_unicode_utf16le", BytesEqual(packet, 56, expected, sizeof(expected)));
    }

    // Control packets ignore payload.
    {
        const auto packet = BuildPacket(JyPacket::Kind::Shutdown, L"ignored");
        const auto base = BuildPacket(JyPacket::Kind::Shutdown, L"");
        Check("packet_shutdown_ignores_payload", packet == base);
    }

    // Long payload is capped, never overflows.
    {
        std::wstring longText(600, L'A');
        const auto packet = BuildPacket(JyPacket::Kind::Command, longText);
        const int maxChars = JyPacket::MaxPayloadChars(JyPacket::Kind::Command);
        const size_t last = 100 + static_cast<size_t>(maxChars - 1) * 2;
        Check("packet_command_max_chars", maxChars == 462);
        Check("packet_command_capped", packet[last] == 'A' && packet[last + 1] == 0);
    }

    Check("packet_sizes", JyPacket::kBasePacketSize == 128 && JyPacket::kSendBufferSize == 1024);

    std::printf("SUMMARY|%d|%d\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
