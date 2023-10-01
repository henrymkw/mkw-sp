#include "ItemSlotTable.hh"

#include "game/system/SaveManager.hh"
#include "sp/settings/ClientSettings.hh"

namespace Item {

void ItemSlotTable::resetPowTimer() {
    // auto saveManager = System::SaveManager::Instance();
    // auto hopDodge = saveManager->getSetting<SP::ClientSettings::HopDodgePractice>();
    if (System::SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::HopDodgePractice>() ==
            SP::ClientSettings::HopDodgePractice::Enable) {
        powTimer = 60;
    } else {
        REPLACED(resetPowTimer)();
    }
}

} // namespace Item