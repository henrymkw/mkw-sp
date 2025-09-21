#pragma once

#include <Common.hh>

namespace Net {

class SELECTHandler {
public:
    struct SELECTPacket {
        u8 _00[0x28 - 0x00];
        u8 m_playerIdToAidMapping[12];
        u8 _34[0x38 - 0x34];
    };
    static_assert(sizeof(SELECTPacket) == 0x38);

private:
    u8 _000[0x008 - 0x000];
    SELECTPacket m_sendPacket;
    SELECTPacket m_recvPackets[12];
    u8 _2e0[0x3f8 - 0x2e0];

    static SELECTHandler *s_instance;
};
static_assert(sizeof(SELECTHandler) == 0x3f8);

} // namespace Net
