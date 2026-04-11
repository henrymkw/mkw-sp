#pragma once

#include <Common.hh>

#include <common/BitField.hh>

#include <game/net/RecordHolder.hh>

#include <array>

namespace SP {

struct Packet {
    Net::RecordHolder<void> *data;
    BitField<12> receivingAids;
    u32 mask;

    void setHeaderBitmap();
};

class OutgoingRacePackets {
public:
    bool push(Net::RecordHolder<void> *data, u32 headerSizesMask, u8 aid, u8 myAid);

    s32 maskIdx(u32 headerSizesMask);

    void setRecvAid(s32 idx, u8 aid);

    void reset();

    size_t count();

    const Packet *operator[](u8 i) const {
        if (i >= m_count) {
            return nullptr;
        }
        return &m_outgoingPackets[i];
    }

private:
    void setHeader(u32 idx, u8 aid);

    std::array<Packet, MAX_PLAYER_COUNT> m_outgoingPackets;
    size_t m_count;
};

} // namespace SP
