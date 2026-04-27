#pragma once

#include <Common.hh>
namespace MKWServer {
#define MKW_SERVER_INFO 0x4D4B5753 // "MKWS"

// Pack since wfc-server sends exactly 6 bytes
struct MKWServerInfoPacket {
    u32 address;
    u16 port;
} __attribute__((packed));
static_assert(sizeof(MKWServerInfoPacket) == 0x6);

void processMKWServerInfoPacket(u8 *data);
} // namespace MKWServer