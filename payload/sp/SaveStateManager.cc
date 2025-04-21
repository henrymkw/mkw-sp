#include "SaveStateManager.hh"

extern "C" {
#include "revolution.h"
}

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
    m_kartSaveState = Kart::KartSaveState(accessor, physics, item);
}

void SaveStateManager::reload() {
    if (m_kartSaveState->doesSaveStateExist()) {
        auto [accessor, physics, item] = GetKartState();
        m_kartSaveState->reload(accessor, physics, item);
    } else {
        // SP_LOG("SaveStateManager: Reload requested without save!");
    }
}

void SaveStateManager::processInput(bool isLoadButtonPressed, bool isSaveButtonPressed) {
    // If the load state button is pressed, we rewind the frame.
    // If a save state exists but the button isn't pressed, we add the current frame to the buffer. 
    
    if (isSaveButtonPressed) {
        save();
        return;
    }
    
    if (isLoadButtonPressed && m_kartSaveState->doesSaveStateExist()) {
        auto [accessor, physics, item] = GetKartState();    
        m_kartSaveState->rewindFrame(accessor, physics);
    } else if (!isLoadButtonPressed && m_kartSaveState->doesSaveStateExist()) {
        auto [accessor, physics, item] = GetKartState();
        
        /* SP_LOG("SaveStateManager: Adding frame to buffer. Current position: %f, %f, %f. Size after pushing head: %d",
            physics->pos()->x,
            physics->pos()->y, 
            physics->pos()->z, 
            m_kartSaveState->getBufferSize()
        ); */
        m_kartSaveState->addFrameToBuffer(accessor, physics);
    }

}

} // namespace SP
