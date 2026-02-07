#include "RaceScene.hh"

#include <sp/SaveStateManager.hh>

namespace Scene {

void RaceScene::destroySubsystems() {
    REPLACED(destroySubsystems)();
    SP::SaveStateManager::DestroyInstance();
}

void RaceScene::createSubsystems() {
    REPLACED(createSubsystems)();
    SP::SaveStateManager::CreateInstance();
}

} // namespace Scene
