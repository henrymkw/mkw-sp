#include "OutgoingPacket.hh"

extern "C" {
#include <revolution.h>
}

#include <sp/net/mkw_server/MKWServer.hh>

namespace MKWServer {

void Packet::addRecipient(u8 aid) {
    receivingAids.set(aid);
    data->record()->setSendAids(receivingAids);
}

bool OutgoingRacePackets::push(Net::RecordHolder<Net::Header> *data, u32 headerSizes, u8 aid,
        u8 myAid) {
    auto &packet = m_outgoingPackets[m_count];
    if (!applyMKWServerHeader(data->record(), myAid)) {
        SP_LOG("Applying MKW-Server header failed!");
        return false;
    }
    packet.data = data;
    packet.headerSizes = headerSizes;
    packet.receivingAids = BitField<u16, 12>{};
    packet.addRecipient(aid);
    m_count++;
    return true;
}

s32 OutgoingRacePackets::lookup(u32 sizesMask) {
    for (u8 i = 0; i < m_count; i++) {
        if (m_outgoingPackets[i].headerSizes == sizesMask) {
            return i;
        }
    }
    return -1;
}

void OutgoingRacePackets::setRecipient(s32 idx, u8 aid) {
    m_outgoingPackets[idx].addRecipient(aid);
}

void OutgoingRacePackets::reset() {
    m_count = 0;
}

u32 OutgoingRacePackets::count() {
    return m_count;
}

} // namespace MKWServer
