#pragma once

#include <Common.hh>

namespace Net {

struct Header {
    u8 magic;
    u8 _01[0x03 - 0x01];
    u8 aid;
    u32 crc32;
    u8 recordSizes[8];

    u8 size(u8 recordId) const {
        return recordSizes[recordId];
    }

    void setRecordSize(u8 recordId, u8 size) {
        recordSizes[recordId] = size;
    }

    u32 getSize() const {
        u32 totalSize = 0;
        for (u8 i = 0; i < std::size(recordSizes); i++) {
            totalSize += recordSizes[i];
        }
        return totalSize;
    }
};
static_assert(sizeof(Header) == 0x10);

} // namespace Net
