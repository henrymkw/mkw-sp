#pragma once

#include <Common.hh>

namespace System {

class SaveGhostManager {
private:
    REPLACE void calc();
    void REPLACED(calc)();

    u8 _00[0x008 - 0x000];
    s32 m_lastRequestId;
    u8 _0C[0x014 - 0x00C];
    bool m_disableSaving;
    u8 _15[0x128 - 0x015];
};
static_assert(sizeof(SaveGhostManager) == 0x128);

} // namespace System
