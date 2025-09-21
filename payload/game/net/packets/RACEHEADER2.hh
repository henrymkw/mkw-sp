#pragma once

#include <Common.hh>

namespace Net {

struct RACEHEADER2Packet {
    u8 _00[0x28 - 0x00];
};
static_assert(sizeof(RACEHEADER2Packet) == 0x28);

} // namespace Net
