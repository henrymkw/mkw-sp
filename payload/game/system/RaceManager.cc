#include "RaceManager.hh"

#include "game/item/ItemDirector.hh"
#include "game/system/RaceConfig.hh"

#include <sp/SaveStateManager.hh>

extern "C" {
#include <revolution.h>
#include <revolution/kpad.h>
}

namespace System {

PadProxy *RaceManager::Player::padProxy() {
    return m_padProxy;
}

void RaceManager::Player::calc() {
    REPLACED(calc)();

    // Check if user wants to load state, rotate items, or use pow.
    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    // These settings are exclusive to TTs. We check for playerId to prevent unintended behavior
    // from ghosts.
    if (gameMode == System::RaceConfig::GameMode::TimeAttack && m_playerId == 0) {
        auto *playerPadProxy = System::RaceManager::Instance()->player(0)->padProxy();
        auto buttons = playerPadProxy->currentRaceInputState().rawButtons;
        auto controller = playerPadProxy->pad()->getControllerId();

        bool shouldUpdateItem = false;
        bool isLoadButtonPressed = false;
        switch (controller) {
        case Registry::Controller::WiiWheel:
            shouldUpdateItem = (buttons & PAD_BUTTON_START) == PAD_BUTTON_START;
            isLoadButtonPressed = (buttons & WPAD_CL_BUTTON_HOME) == WPAD_CL_BUTTON_HOME;
            break;
        case Registry::Controller::WiiRemoteAndNunchuck:
            shouldUpdateItem = (buttons & WPAD_BUTTON_DOWN) == WPAD_BUTTON_DOWN;
            isLoadButtonPressed = (buttons & WPAD_CL_BUTTON_DOWN) == WPAD_CL_BUTTON_DOWN;
            break;
        case Registry::Controller::Classic:
            shouldUpdateItem = (buttons & KPAD_CL_TRIGGER_ZL) == KPAD_CL_TRIGGER_ZL;
            isLoadButtonPressed = (buttons & KPAD_CL_TRIGGER_ZR) == KPAD_CL_TRIGGER_ZR;

            break;
        case Registry::Controller::GameCube:
            shouldUpdateItem = (buttons & PAD_BUTTON_Y) == PAD_BUTTON_Y;
            isLoadButtonPressed = (buttons & PAD_TRIGGER_Z) == PAD_TRIGGER_Z;
            break;
        case Registry::Controller::None:
            return;
        }

        if (auto *itemDirector = Item::ItemDirector::Instance()) {
            itemDirector->processInput(shouldUpdateItem);
        }

        if (auto *saveStateManager = SP::SaveStateManager::Instance()) {
            saveStateManager->processInput(isLoadButtonPressed);
        }
    }
}

RaceManager::Player *RaceManager::player(u32 playerId) {
    return m_players[playerId];
}

RaceManager *RaceManager::CreateInstance() {
    s_instance = REPLACED(CreateInstance)();
    return s_instance;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

} // namespace System
