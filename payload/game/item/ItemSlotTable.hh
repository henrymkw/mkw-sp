#pragma once

#include <Common.hh>

namespace Item {

class ItemSlotTable {
public:
    REPLACE void resetPowTimer();

    void REPLACED(resetPowTimer)();

private:
    u8 _00[0x40 - 0x00];
    s32 powTimer;
    u32 playerCount;
};
static_assert(sizeof(ItemSlotTable) == 0x48);
} // namespace Item