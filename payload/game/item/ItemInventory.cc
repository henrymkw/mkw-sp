#include "ItemInventory.hh"

#include "game/system/RaceConfig.hh"

extern "C" {
#include <revolution.h>
}

namespace Item {

void ItemInventory::resetItem() {
    auto *raceConfig = System::RaceConfig::Instance();
    if (raceConfig->raceScenario().gameMode != System::RaceConfig::GameMode::TimeAttack) {
        REPLACED(resetItem)();
    }
}

void ItemInventory::resetHeldItem() {
    auto *raceConfig = System::RaceConfig::Instance();
    if (raceConfig->raceScenario().gameMode != System::RaceConfig::GameMode::TimeAttack) {
        REPLACED(resetHeldItem)();
    }
}

ItemId ItemInventory::getCurrentItem() const {
    return m_currentItemID;
}

void ItemInventory::setItemWheelPressed(bool pressed) {
    m_itemWheelPressed = pressed;
}

bool ItemInventory::getItemWheelPressed() const {
    return m_itemWheelPressed;
}

} // namespace Item
