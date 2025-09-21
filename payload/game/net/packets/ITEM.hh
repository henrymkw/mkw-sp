#pragma once

#include <Common.hh>

namespace Net {

class ITEMHandler {
public:
    struct ITEMPacket {
        u8 _0[0x8 - 0x0];
    };
    static_assert(sizeof(ITEMPacket) == 0x8);

private:
    u8 _000[0x184 - 0x000];

    static ITEMHandler *s_instance;
};
static_assert(sizeof(ITEMHandler) == 0x184);

} // namespace Net
