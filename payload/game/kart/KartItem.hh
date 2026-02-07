#pragma once

#include <Common.hh>

#include "game/item/ItemInventory.hh"

namespace Kart {

// pretend that it doesnt inherit KartObjectProxy for now
class KartItem {
public:
    void REPLACED(setItem)(u32 r4, u32 r5, u32 r6);
    REPLACE void setItem(u32 r4, u32 r5, u32 r6);

    Item::ItemId nextItem();

    void calcItemWheel(bool isPressed);

private:
    u8 _000[0x018 - 0x000];
    u8 m_playerId;
    u8 _019[0x088 - 0x019];
    Item::ItemInventory m_inventory;
    u8 _0b4[0x248 - 0x0b4];
};
static_assert(sizeof(KartItem) == 0x248);

} // namespace Kart
