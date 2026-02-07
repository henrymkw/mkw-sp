#pragma once

#include <Common.hh>

namespace Net {

struct Header {
    u8 magic;
    u8 _01[0x03 - 0x01];
    u8 aid;
    u32 crc32;
    u8 packetSizes[8];

    u32 getSize() const {
        u32 totalSize = 0;
        for (u8 i = 0; i < std::size(packetSizes); i++) {
            totalSize += packetSizes[i];
        }
        return totalSize;
    }
};
static_assert(sizeof(Header) == 0x10);

} // namespace Net
