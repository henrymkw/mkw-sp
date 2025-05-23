#include "RaceManager.hh"

#include "game/gfx/CameraManager.hh"
#include "game/item/ItemDirector.hh"
#include "game/item/PowManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

extern "C" {
    #include <revolution/kpad.h>
}

#include <sp/SaveStateManager.hh>
#include <sp/ThumbnailManager.hh>
#include <sp/cs/RaceManager.hh>

namespace System {

u8 RaceManager::Player::rank() const {
    return m_rank;
}

u16 RaceManager::Player::battleScore() const {
    return m_battleScore;
}

u8 RaceManager::Player::maxLap() const {
    return m_maxLap;
}

bool RaceManager::Player::hasFinished() const {
    return m_hasFinished;
}

PadProxy *RaceManager::Player::padProxy() {
    return m_padProxy;
}

void RaceManager::Player::setExtraGhostPadProxy() {
    m_padProxy = InputManager::Instance()->extraGhostProxy(m_playerId);
}

void RaceManager::Player::calc() {
    REPLACED(calc)();
    
    // Check if user wants to load state, rotate items, or use pow. 
    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    auto *saveManager = System::SaveManager::Instance();

    // These settings are exclusive to TTs. We check for playerId to prevent unintended behavior from ghosts.
    if (gameMode == System::RaceConfig::GameMode::TimeAttack && m_playerId == 0) {
        auto *playerPadProxy = System::RaceManager::Instance()->player(0)->padProxy();
        auto buttons = playerPadProxy->currentRaceInputState().rawButtons;
        auto controller = playerPadProxy->pad()->getControllerId();
        
        bool shouldUsePow = false;
        bool shouldUpdateItem = false;
        bool shouldLoadState = false;
        switch (controller) {
        case Registry::Controller::WiiWheel:
            shouldUsePow = (buttons & PAD_BUTTON_START) == PAD_BUTTON_START;
            shouldUpdateItem = (buttons & PAD_BUTTON_START) == PAD_BUTTON_START;
            shouldLoadState = (buttons & WPAD_CL_BUTTON_HOME) == WPAD_CL_BUTTON_HOME;
            break;
        case Registry::Controller::WiiRemoteAndNunchuck:
            shouldUsePow = ((buttons & WPAD_BUTTON_LEFT) == WPAD_BUTTON_LEFT) || ((buttons & WPAD_BUTTON_RIGHT) == WPAD_BUTTON_RIGHT);
            shouldUpdateItem = (buttons & WPAD_BUTTON_DOWN) == WPAD_BUTTON_DOWN;
            shouldLoadState = (buttons & WPAD_CL_BUTTON_DOWN) == WPAD_CL_BUTTON_DOWN;
            break;
        case Registry::Controller::Classic:
            shouldUsePow = (buttons & KPAD_CL_BUTTON_X) == KPAD_CL_BUTTON_X;
            shouldUpdateItem = (buttons & KPAD_CL_TRIGGER_ZL) == KPAD_CL_TRIGGER_ZL;
            shouldLoadState = (buttons & KPAD_CL_TRIGGER_ZR) == KPAD_CL_TRIGGER_ZR;
            break;
        case Registry::Controller::GameCube:
            shouldUsePow = (buttons & PAD_BUTTON_X) == PAD_BUTTON_X;
            shouldUpdateItem = (buttons & PAD_BUTTON_Y) == PAD_BUTTON_Y;
            shouldLoadState = (buttons & PAD_TRIGGER_Z) == PAD_TRIGGER_Z;
            break;
        case Registry::Controller::None:
            return;
        }

        auto TAHopDodgePracticeEnabled = saveManager->getSetting<SP::ClientSettings::Setting::TAHopDodgePractice>() == SP::ClientSettings::TAHopDodgePractice::Enable;
        auto ItemWheelEnabled = saveManager->getSetting<SP::ClientSettings::Setting::YButton>() == SP::ClientSettings::YButton::ItemWheel;

        if (ItemWheelEnabled) {
            if (auto *itemDirector = Item::ItemDirector::Instance()) {
                itemDirector->processInput(shouldUpdateItem);
            }
        }

        if (shouldUsePow && TAHopDodgePracticeEnabled) {
            if (auto *powManager = Item::PowManager::Instance()) {
                powManager->beginPow(0);
            }
        }

        if (auto *saveStateManager = SP::SaveStateManager::Instance()) {
            if (shouldLoadState) {
                saveStateManager->reload();
            }
        }
    }
}

Util::Random *RaceManager::dynamicRandom() {
    return m_dynamicRandom;
}

RaceManager::Player *RaceManager::player(u32 playerId) {
    return m_players[playerId];
}

u32 RaceManager::time() const {
    return m_time;
}

MapdataKartPoint *RaceManager::getKartPoint(u32 playerId) {
    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    if (raceScenario.gameMode != RaceConfig::GameMode::OfflineBT) {
        return REPLACED(getKartPoint)(playerId);
    }

    s16 id = playerId;
    if (raceScenario.spMaxTeamSize >= 2) {
        id = 0;
        for (u32 i = 0; i < playerId; i++) {
            if (raceScenario.players[i].spTeam == raceScenario.players[playerId].spTeam) {
                id++;
            }
        }
        id = (id + m_battleKartPointStart) % raceScenario.spMaxTeamSize;
        id += raceScenario.players[playerId].spTeam * raceScenario.spMaxTeamSize;
    }

    auto *courseMap = CourseMap::Instance();
    u32 index;
    for (index = 0; index < courseMap->kartPoint()->m_numEntries; index++) {
        if (courseMap->kartPoint()->m_entryAccessors[index]->m_data->id == id) {
            break;
        }
    }

    if (index == courseMap->kartPoint()->m_numEntries) {
        return nullptr;
    } else {
        return courseMap->kartPoint()->m_entryAccessors[index];
    }
}

void RaceManager::getStartTransform(Vec3 *pos, Vec3 *rot, u32 playerId) {
    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    if (raceScenario.gameMode != RaceConfig::GameMode::OfflineBT) {
        REPLACED(getStartTransform)(pos, rot, playerId);
        return;
    }

    if (auto *kartPoint = getKartPoint(playerId)) {
        kartPoint->getTransform(pos, rot, 1, 1);
    } else {
        *pos = {0.0f, 0.0f, 0.0f};
        *rot = {1.0f, 0.0f, 0.0f}; // Sneaky 1 for some reason
    }
}

void RaceManager::calc() {
    REPLACED(calc)();

    auto *sectionManager = UI::SectionManager::Instance();
    if (sectionManager->currentSection()->id() == UI::SectionId::Thumbnails) {
        if (m_time == 25) {
            UI::SectionId sectionId;
            if (SP::ThumbnailManager::Continue()) {
                auto *raceConfig = System::RaceConfig::Instance();
                raceConfig->m_menuScenario.courseId = SP::ThumbnailManager::CourseId();
                raceConfig->m_spMenu.pathReplacement = SP::ThumbnailManager::Path();
                sectionId = UI::SectionId::Thumbnails;
            } else {
                sectionId = UI::SectionId::ServicePack;
            }
            sectionManager->setNextSection(sectionId, UI::Page::Anim::Next);
            sectionManager->startChangeSection(0, 0x000000ff);
        }
    }
}

void RaceManager::endPlayerRace(u32 playerId) {
    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    if (raceScenario.players[playerId].type == RaceConfig::Player::Type::Ghost) {
        InputManager::Instance()->endExtraGhostProxy(playerId);
    }

    REPLACED(endPlayerRace)(playerId);
}

bool RaceManager::getIfPlayerHopped() {
    auto *playerPadProxy = System::RaceManager::Instance()->player(0)->padProxy();
    auto buttons = playerPadProxy->currentRaceInputState().rawButtons;
    auto controller = playerPadProxy->pad()->getControllerId();

    // TODO: Check for other controllers.
    if (controller == Registry::Controller::GameCube) {
        if (((buttons & PAD_BUTTON_B) == PAD_BUTTON_B) || ((buttons & PAD_TRIGGER_R) == PAD_TRIGGER_R)) {
            return true;
        }
    }
    return false;
}  


RaceManager *RaceManager::CreateInstance() {
    s_instance = new RaceManager;
    assert(s_instance);

    const auto &raceScenario = RaceConfig::Instance()->raceScenario();
    for (u32 i = 0; i < raceScenario.playerCount; i++) {
        if (raceScenario.players[i].type == RaceConfig::Player::Type::Ghost) {
            s_instance->m_players[i]->setExtraGhostPadProxy();
        }
    }

    s_instance->m_spectatorMode = false;
    if (SP::RaceManager::Instance()) {
        s_instance->m_canStartCountdown = false;
    }

    return s_instance;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

} // namespace System
