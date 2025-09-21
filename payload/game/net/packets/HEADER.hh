#pragma once

#include <Common.hh>

namespace Net {

struct HEADERPacket {
    u32 magic;
    u32 crc32;
    u8 packetSizes[8];
};
static_assert(sizeof(HEADERPacket) == 0x10);

} // namespace Net
