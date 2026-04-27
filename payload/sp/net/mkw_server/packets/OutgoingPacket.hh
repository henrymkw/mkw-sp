#pragma once

#include <Common.hh>

#include <common/BitField.hh>

#include <game/net/RecordHolder.hh>
#include <game/net/records/Header.hh>

#include <array>

namespace MKWServer {

struct Packet {
    // The packet to be sent
    Net::RecordHolder<Net::Header> *data;

    // Aids that will receive this packet
    BitField<u16, 12> receivingAids;

    // bit mask of the header sizes. funcitonally a unique id
    u32 headerSizes;

    // Adds aid to receivingAids, updates the header accordingly
    void addRecipient(u8 aid);
};

class OutgoingRacePackets {
public:
    bool push(Net::RecordHolder<Net::Header> *data, u32 headerSizes, u8 aid, u8 myAid);

    s32 lookup(u32 sizesMask);

    void setRecipient(s32 idx, u8 aid);

    void reset();

    size_t count();

    const Packet *operator[](u8 i) const {
        if (i >= m_count) {
            return nullptr;
        }
        return &m_outgoingPackets[i];
    }

private:
    std::array<Packet, MAX_PLAYER_COUNT> m_outgoingPackets;
    size_t m_count;
};

} // namespace MKWServer
