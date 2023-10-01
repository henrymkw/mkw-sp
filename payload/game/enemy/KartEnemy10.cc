#include "KartEnemy10.hh"

#include "game/system/SaveManager.hh"
#include "sp/settings/ClientSettings.hh"

#include <Common.hh>

namespace Enemy {

void KartEnemy10::unk() {
    if (System::SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::HopDodgePractice>() ==
            SP::ClientSettings::HopDodgePractice::Disable) {
        REPLACED(unk)();
    }
}

} // namespace Enemy