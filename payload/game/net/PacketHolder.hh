#pragma once

#include <Common.hh>

#include "game/net/records/Header.hh"

#include <cstring>

namespace Net {

template <typename T>
class PacketHolder {
public:
    void reset() {
        memset(m_packet, 0, m_bufferSize);
        m_packetSize = 0;
    }

    void copy(void *src, u32 len) {
        memcpy(m_packet, src, len);
        m_packetSize = len;
    }

    void append(void *src, u32 len) {
        memcpy(reinterpret_cast<u8 *>(m_packet) + m_packetSize, src, len);
        m_packetSize += len;
    }

    T *packet() {
        return m_packet;
    }

    u32 packetSize() const {
        return m_packetSize;
    }

private:
    T *m_packet;
    u32 m_bufferSize;
    u32 m_packetSize;
};

} // namespace Net
