#pragma once

#include <Common.hh>

#include "game/net/PacketHolder.hh"

namespace Net {

class RACEPacketHolder {
public:
    PacketHolder *getPacketHolder(u32 idx) {
        return m_packets[idx];
    }

private:
    PacketHolder *m_packets[8];
};
static_assert(sizeof(RACEPacketHolder) == 0x20);

} // namespace Net
