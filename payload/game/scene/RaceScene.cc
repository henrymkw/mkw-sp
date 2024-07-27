#include "RaceScene.hh"

#include "game/battle/CoinManager.hh"
#include "game/effect/EffectManager.hh"
#include "game/enemy/EnemyManager.hh"
#include "game/gfx/CameraManager.hh"
#include "game/item/ItemManager.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/obj/ObjDirector.hh"
#include "game/race/BoxColManager.hh"
#include "game/race/DriverManager.hh"
#include "game/race/JugemManager.hh"
#include "game/system/HBMManager.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <features/save_states/SaveStates.hh>
#include <sp/SaveStateManager.hh>
#include <sp/cs/RaceClient.hh>

namespace Scene {

extern "C" {
bool unk_8078ddb4();

extern u32 unk_809c1874;
extern bool unk_809c19a0;
extern bool unk_809c21d8;
extern bool unk_809c2f3c;
}

void RaceScene::destroySubsystems() {
    REPLACED(destroySubsystems)();
#if ENABLE_SAVE_STATES
    SP::SaveStateManager::DestroyInstance();
#endif
}

void RaceScene::createSubsystems() {
    REPLACED(createSubsystems)();
#if ENABLE_SAVE_STATES
    SP::SaveStateManager::CreateInstance();
#endif
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::YButton>();
    saveManager->m_usedItemWheel = setting == SP::ClientSettings::YButton::ItemWheel;
}

} // namespace Scene
