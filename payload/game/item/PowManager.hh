#pragma once

#include <Common.hh>

namespace Item {

class PowManager {
public:
    static PowManager *Instance();

    REPLACE void beginPow(u32 playerIndex);

    void REPLACED(beginPow)(u32 playerIndex);

private:
    REPLACE void calc();
    void REPLACED(calc)();

    u32 m_powTimer;
    u8 _04[0x08 - 0x04];
    u8 m_powUserPlayerId;
    bool m_playerIdxAffectedByPow[12];
    u8 _15[0x28 - 0x15];

    static PowManager *s_instance;
};
static_assert(sizeof(PowManager) == 0x28);
} // namespace Item