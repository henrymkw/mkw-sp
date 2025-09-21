#pragma once

#include <Common.hh>

namespace Net {

class PacketHolder {
public:
    void reset();

    void copy(void *src, u32 len);

    // Helper to patch magic and aid for race packets sent to MKW server
    void PatchPacketForMKWServer(u8 aid);

    char *getPacket() {
        return m_packet;
    }

private:
    // TODO: Could be a generic type?
    char *m_packet;
    u32 m_bufferSize;
    u32 m_packetSize;
};

} // namespace Net
