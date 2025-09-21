#pragma once

#include <Common.hh>

namespace Net {

struct EVENTPacket {
    u8 _00[0xf8 - 0x00];
};
static_assert(sizeof(EVENTPacket) == 0xf8);

class EVENTHandler {
private:
    u8 _0000[0x2b84 - 0x0000];
    u32 _unk2b88;

    static EVENTHandler *s_instance;
};
static_assert(sizeof(EVENTHandler) == 0x2b88);

} // namespace Net
