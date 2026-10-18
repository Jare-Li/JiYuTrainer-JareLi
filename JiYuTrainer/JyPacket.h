#pragma once

#include <cstddef>
#include <string>

// Pure packet construction for the original JiYu UDP replay module.
// The byte layout is intentionally identical to JiYuTrainer 1.7.6.
namespace JyPacket {

constexpr int kBasePacketSize = 128;
constexpr int kSendBufferSize = 1024;

enum class Kind {
    Message = 0,
    Command = 1,
    Reboot = 2,
    Shutdown = 3,
};

const unsigned char* BasePacket(Kind kind);

// Offset of the UTF-16LE payload inside the 1024-byte datagram.
// Message: 56, Command: 100, control packets: -1.
int PayloadOffset(Kind kind);

// Maximum number of wchar_t characters that fit in the datagram.
int MaxPayloadChars(Kind kind);

// Zero-fills outSize bytes, copies the 128-byte base packet and the payload.
// Payload is silently capped to the remaining space; never overflows.
void Build(Kind kind, const std::wstring& payload, unsigned char* out, size_t outSize);

} // namespace JyPacket
