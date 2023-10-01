#pragma once

#include <Common.hh>

namespace Item {

class PowManager {
public:
    static PowManager *Instance();

    REPLACE void unk(u32 playerIndex);

    void REPLACED(unk)(u32 playerIndex);

private:
    u8 _00[0x28 - 0x00];

    static PowManager *s_instance;
};
static_assert(sizeof(PowManager) == 0x28);
} // namespace Item