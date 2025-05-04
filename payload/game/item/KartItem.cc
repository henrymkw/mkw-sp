#include "KartItem.hh"

#include "game/item/PowManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include "game/util/Registry.hh"

#include <sp/cs/RoomClient.hh>

#include <algorithm>

extern "C" {
#include <revolution/kpad.h>
}

namespace Item {

void KartItem::setItem(u32 r4, u32 r5, u32 r6) {
    if (auto *roomClient = SP::RoomClient::Instance();
            roomClient && roomClient->isPlayerRemote(m_playerId)) {
        return;
    }

    REPLACED(setItem)(r4, r5, r6);
}

ItemId KartItem::nextItem() {
    const std::array<ItemId, 10> itemCycle{ItemId::Shroom, ItemId::Golden, ItemId::Star,
            ItemId::Mega, ItemId::Bill, ItemId::Nana, ItemId::FIB, ItemId::Green, ItemId::Bomb,
            ItemId::TC};

    auto it = std::find(std::begin(itemCycle), std::end(itemCycle), m_inventory.getCurrentItem());
    if (it != itemCycle.end() && it != itemCycle.begin() + (itemCycle.size() - 1)) {
        return *(it + 1);
    }
    return itemCycle.front();
}

void KartItem::calcItemWheel(bool isPressed) {
    if (!isPressed) {
        m_inventory.setItemWheelPressed(false);
    } else if (!m_inventory.getItemWheelPressed()) {
        m_inventory.setItem(nextItem());
        m_inventory.setItemWheelPressed(true);
    }
}

} // namespace Item
