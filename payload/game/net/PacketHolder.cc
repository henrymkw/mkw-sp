#include "PacketHolder.hh"

namespace Net {

void PacketHolder::PatchPacketForMKWServer(u8 aid) {
    m_packet[0] = 0xb;
    m_packet[3] = aid;
}

} // namespace Net
