#include "RacePage.hh"

#include "game/item/PowManager.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ctrl/CtrlRaceBattleAddPoint.hh"
#include "game/ui/ctrl/CtrlRaceBattlePoint.hh"
#include "game/ui/ctrl/CtrlRaceBattleTotalPoint.hh"
#include "game/ui/ctrl/CtrlRaceDebugPanel.hh"
#include "game/ui/ctrl/CtrlRaceInputDisplay.hh"
#include "game/ui/ctrl/CtrlRaceLap.hh"
#include "game/ui/ctrl/CtrlRaceSpeed.hh"
#include "game/ui/ctrl/CtrlRaceWaitSymbol.hh"

#include <sp/cs/RaceManager.hh>

static bool isPowActive = false;
static bool playerPressedHopButton = false;
static u32 timeOfHopAction = 0;
static bool writtenMessage = false;

namespace UI {

namespace Control {

enum {
    Time = 1 << 1,
    Count = 1 << 2,
    Map = 1 << 3,
    RankNum = 1 << 4,
    Lap = 1 << 5,
    ItemWindow = 1 << 6,
    Point = 1 << 7,
    GhostTimeDiff = 1 << 8,
    Live = 1 << 9,
    Score = 1 << 10,
    ItemBalloon = 1 << 11,
    GhostMessage = 1 << 12,
};

} // namespace Control

u32 RacePage::watchedPlayerId() const {
    return m_watchedPlayerId;
}

u32 RacePage::lastWatchedPlayerId() const {
    return m_lastWatchedPlayerId;
}

RacePage *RacePage::Instance() {
    return s_instance;
}

PageId RacePage::getReplacement() {
    switch (id()) {
    case PageId::Bt1pHud:
    case PageId::Bt2pHud:
    case PageId::Bt3pHud:
    case PageId::Bt4pHud:
        return PageId::ResultRaceUpdate;
    default:
        return m_replacement;
    }
}

void RacePage::onInit() {
    REPLACED(onInit)();

    m_lastWatchedPlayerId = m_watchedPlayerId;
}

void RacePage::afterCalc() {
    REPLACED(afterCalc)();   
    m_lastWatchedPlayerId = m_watchedPlayerId;

    if (m_ghostMessage) {
        m_ghostMessage->setVisible(true);
        auto isTACurrentMode = System::RaceConfig::Instance()->raceScenario().gameMode == System::RaceConfig::GameMode::TimeAttack;
        auto isHopDodgePracticeEnabled = System::SaveManager::Instance()->getSetting<SP::ClientSettings::Setting::TAHopDodgePractice>() == SP::ClientSettings::TAHopDodgePractice::Enable;
        auto *raceManager = System::RaceManager::Instance();
        auto *powManager = Item::PowManager::Instance();

        u32 powTimer = powManager->getPowTimer();
        
        auto accessor = Kart::KartObjectManager::Instance()->object(0)->m_accessor;
        auto *state = accessor.state;
        
        bool playerStartHop = state->hopStart();
        bool isTouchingGround = state->touchingGround();

        if (isTACurrentMode && isHopDodgePracticeEnabled) {

            // We check if the pow is active using the timer. For simplicity the user has a 20 frame window but this isnt needed.
            if (powTimer < 55 && powTimer > 35) {
                isPowActive = true;
                m_ghostMessage->setVisible(true);
            }
            else {
                // Gotta reset variables when pow isn't active.
                isPowActive = false;
                playerPressedHopButton = false;
                writtenMessage = false;
                timeOfHopAction = 0;
            }
            
            if (isPowActive) {
                // This entire if body is more complicated than it should be
                // We need to get the time the player started the hop (one flag),
                // but the way we get that depends if the user was early or late (physics variable or controller input)

                if (powTimer > 45 && playerStartHop && !playerPressedHopButton) {
                    playerPressedHopButton = true;
                    timeOfHopAction = powTimer;
                    SP_LOG("Player started hop. Pow timer: %u", powTimer);
                }
                else if (powTimer < 45 && raceManager->getIfPlayerHopped() && !playerPressedHopButton) {
                    playerPressedHopButton = true;
                    timeOfHopAction = powTimer;
                    SP_LOG("Player pressed hop button. Pow timer: %u", powTimer);
                }
                else if (powTimer == 45 && playerStartHop && !playerPressedHopButton) {
                    timeOfHopAction = 45;
                    playerPressedHopButton = true;
                    SP_LOG("This should hop dodge??");
                }

                // I think the first two flags are explanitory, the last is so we don't spam writing. 
                if (playerPressedHopButton && !isTouchingGround && !writtenMessage) {
                    if (timeOfHopAction == 45) {
                        m_ghostMessage->setMessageAll(10472);
                    }
                    else {
                        bool early = false;
                        early = timeOfHopAction > 45;
                        
                        wchar_t message[64];
                        if (early) {
                            swprintf(message, sizeof(message), L"%d frames too early!", timeOfHopAction - 45);
                        }
                        else {
                            swprintf(message, sizeof(message), L"%d frames too late!", 45 - timeOfHopAction);
                        }

                        MessageInfo info;
                        info.strings[0] = message;
                        m_ghostMessage->setMessageAll(10471, &info);
                    } 
                    writtenMessage = true;
                }
            }

            
        }
    }
    else {
        SP_LOG("m_ghostMessage is null");
    }

}

u8 RacePage::getControlCount(u32 controls) const {
    u8 count = REPLACED(getControlCount)(controls);

    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    u32 localPlayerCount = std::max(raceScenario.localPlayerCount, static_cast<u8>(1));

    if (controls & Control::Point) {
        if (raceScenario.spMaxTeamSize < 2) {
            count--; // CtrlRaceBattleTotalPoint
        }
    }

    if (localPlayerCount <= 2 || !(controls & Control::Point)) {
        count += localPlayerCount; // CtrlRaceSpeed
    }

    auto *saveManager = System::SaveManager::Instance();
    auto vanillaMode = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
    if (vanillaMode == SP::ClientSettings::VanillaMode::Disable) {
        count += localPlayerCount;     // CtrlRaceInputDisplay
        count += localPlayerCount < 2; // CtrlRaceDebugPanel
    }

    if (getNameBalloonCount() != 0) {
        count += 12 - getNameBalloonCount();
    }

    // Extend vanilla online logic to SP
    if (SP::RaceManager::Instance()) {
        count += (localPlayerCount * 2) + 1;
    }

    return count;
}

void RacePage::initControls(u32 controls) {
    REPLACED(initControls)(controls & ~(Control::Point | Control::Lap));

    u32 index = getControlCount(controls) - 1;
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    u32 localPlayerCount = std::max(raceScenario.localPlayerCount, static_cast<u8>(1));

    if (controls & Control::Lap) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceLap;
            insertChild(index--, control, 0);
            u32 screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
            control->load(screenCount, i);
        }
    }

    if (controls & Control::Point) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceBattlePoint;
            insertChild(index--, control, 0);
            u32 screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
            char variant[0x20];
            snprintf(variant, std::size(variant), "CtrlRacePoint_%u_%u", screenCount, i);
            control->load(variant, i);
        }
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceBattleAddPoint;
            insertChild(index--, control, 0);
            u32 screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
            char variant[0x20];
            snprintf(variant, std::size(variant), "BattleAddPoint_%u_%u", screenCount, i);
            control->load(variant, i);
        }
        if (raceScenario.spMaxTeamSize >= 2) {
            auto *control = new CtrlRaceBattleTotalPoint;
            insertChild(index--, control, 0);
            control->load();
        }
    }

    if (localPlayerCount <= 2 || !(controls & Control::Point)) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceSpeed;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
    }

    auto *saveManager = System::SaveManager::Instance();
    auto vanillaMode = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
    if (vanillaMode != SP::ClientSettings::VanillaMode::Enable) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceInputDisplay;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
        if (localPlayerCount < 2) {
            auto *control = new CtrlRaceDebugPanel;
            insertChild(index--, control, 0);
            control->load();
        }
    }

    if (SP::RaceManager::Instance()) {
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceWifiStartMessage;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
        for (u32 i = 0; i < localPlayerCount; i++) {
            auto *control = new CtrlRaceWifiFinishMessage;
            insertChild(index--, control, 0);
            control->load(localPlayerCount, i);
        }
        auto *control = new CtrlRaceWaitSymbol;
        insertChild(index--, control, 0);
        control->load(1, 0);
    }
}

u32 TARacePage::getControls() const {
    u32 controls = 0;
    controls |= Control::Time;
    controls |= Control::Count;
    controls |= Control::Map;
    controls |= Control::RankNum;
    controls |= Control::Lap;
    controls |= Control::ItemWindow;
    controls |= Control::GhostTimeDiff;
    controls |= Control::GhostMessage;
    return controls;
}

u32 VSMultiRacePage::getControls() const {
    u32 controls = 0;
    controls |= Control::Time;
    controls |= Control::Count;
    controls |= Control::Map;
    controls |= Control::RankNum;
    controls |= Control::Lap;
    controls |= Control::ItemWindow;
    controls |= Control::ItemBalloon;
    return controls;
}

u32 GhostReplayRacePage::getControls() const {
    u32 controls = 0;
    controls |= Control::Time;
    controls |= Control::Count;
    controls |= Control::Map;
    controls |= Control::RankNum;
    controls |= Control::Lap;
    controls |= Control::ItemWindow;
    return controls;
}

} // namespace UI
