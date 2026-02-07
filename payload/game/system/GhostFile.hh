#pragma once

#include <Common.hh>

namespace System {

class GhostFile {
private:
    u8 _00[0xd8 - 0x00];
};
static_assert(sizeof(GhostFile) == 0xd8);

} // namespace System
