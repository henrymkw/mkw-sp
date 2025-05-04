#include "SaveStateManager.hh"

extern "C" {
#include "revolution.h"
}
#include <tuple>

namespace SP {

SaveStateManager *SaveStateManager::s_instance = nullptr;

void SaveStateManager::CreateInstance() {
    if (s_instance) {
        return;
    }

    s_instance = new SaveStateManager;
}

void SaveStateManager::DestroyInstance() {
    if (!s_instance) {
        return;
    }

    delete s_instance;
    s_instance = nullptr;
}

auto SaveStateManager::GetKartState() {
    auto kartObjectManager = Kart::KartObjectManager::Instance();
    auto kartObject = kartObjectManager->object(0);
    auto physics = kartObject->getVehiclePhysics();

    auto item = s_itemDirector->m_kartItems;

    return std::make_tuple(kartObject->m_accessor, physics, item);
}

void SaveStateManager::save() {
    auto [accessor, physics, item] = GetKartState();
    m_kartSaveState.emplace(accessor, physics, item);
}

void SaveStateManager::reload() {
    if (m_kartSaveState.has_value()) {
        auto [accessor, physics, item] = GetKartState();
        (*m_kartSaveState).reload(accessor, physics, item);
    } else {
        SP_LOG("SaveStateManager: Reload requested without save!");
    }
}

} // namespace SP
