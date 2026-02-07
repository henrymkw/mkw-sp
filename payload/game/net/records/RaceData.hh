#pragma once

#include <Common.hh>

namespace Net {

class RaceDataRecord {
    u8 _00[0x40 - 0x00];
};
static_assert(sizeof(RaceDataRecord) == 0x40);

} // namespace Net
