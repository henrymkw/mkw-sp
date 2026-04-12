#include "OutgoingPacket.hh"

#include <game/net/records/Header.hh>

extern "C" {
#include <revolution.h>
#include <sp/net/mkw_server/MKW-Server.h>
}

namespace SP {

void Packet::setHeaderBitmap() {
    u16 recipients = static_cast<u16>(receivingAids.field());
    Net::Header *header = reinterpret_cast<Net::Header *>(data->record());
    header->setSendAids(recipients);
}

bool OutgoingRacePackets::push(Net::RecordHolder<void> *data, u32 headerSizesMask, u8 aid,
        u8 myAid) {
    auto &packet = m_outgoingPackets[m_count++];

    if (!applyMKWServerHeader(data->record(), myAid)) {
        SP_LOG("Applying MKW-Server header failed!");
        return false;
    }

    packet.data = data;
    packet.mask = headerSizesMask;
    packet.receivingAids = BitField<12>{};
    packet.receivingAids.set(aid);
    packet.setHeaderBitmap();
    return true;
}

s32 OutgoingRacePackets::maskIdx(u32 headerSizesMask) {
    for (u8 i = 0; i < m_count; i++) {
        if (m_outgoingPackets[i].mask == headerSizesMask) {
            return i;
        }
    }
    return -1;
}

void OutgoingRacePackets::setRecvAid(s32 idx, u8 aid) {
    m_outgoingPackets[idx].receivingAids.set(aid);
    m_outgoingPackets[idx].setHeaderBitmap();
}

void OutgoingRacePackets::reset() {
    m_count = 0;
}

u32 OutgoingRacePackets::count() {
    return m_count;
}

void OutgoingRacePackets::setHeader(u32 idx, u8 aid) {
    Net::Header *header = reinterpret_cast<Net::Header *>(m_outgoingPackets[idx].data->record());
    header->set(aid);
}

} // namespace SP
