#pragma once

#include <Common.hh>

namespace Net {

struct Header {
    u8 magic;       // Added
    u8 myAid;       // Added
    u16 sendToAids; // Added. TODO: Change type to BitField
    u32 crc32;
    u8 recordSizes[8];

    void set(u8 aid) {
        sendToAids |= (1 << aid);
    }

    void setSendAids(u16 aids) {
        sendToAids = aids;
    }

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
