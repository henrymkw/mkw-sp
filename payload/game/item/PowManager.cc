#include "PowManager.hh"

#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/util/Registry.hh"

namespace Item {

PowManager *PowManager::Instance() {
    return s_instance;
}

u32 PowManager::getPowTimer() const {
    return m_powTimer;
}

void PowManager::calc() {
    REPLACED(calc)();
}

void PowManager::beginPow(u32 playerIndex) {
    REPLACED(beginPow)(playerIndex);
   
    // For hop dodge practice, we need to first check if the setting is enabled and the user is in TTs.
    auto *raceConfig = System::RaceConfig::Instance();
    auto gameMode = raceConfig->raceScenario().gameMode;
    
    auto *saveManager = System::SaveManager::Instance();
    auto isTAHopDodgePracticeEnabled = saveManager->getSetting<SP::ClientSettings::Setting::TAHopDodgePractice>() == SP::ClientSettings::TAHopDodgePractice::Enable;


    if (gameMode == System::RaceConfig::GameMode::TimeAttack && isTAHopDodgePracticeEnabled) {
        // It doesn't matter but looks better if ghosts can't get powed.
        m_playerIdxAffectedByPow[0] = true;
        m_playerIdxAffectedByPow[1] = false;
        m_playerIdxAffectedByPow[2] = false;
        m_playerIdxAffectedByPow[3] = false;
        m_playerIdxAffectedByPow[4] = false;
        m_playerIdxAffectedByPow[5] = false;
        m_playerIdxAffectedByPow[6] = false;
        m_playerIdxAffectedByPow[7] = false;
        m_playerIdxAffectedByPow[8] = false;
        m_playerIdxAffectedByPow[9] = false;
        m_playerIdxAffectedByPow[10] = false;
        m_playerIdxAffectedByPow[11] = false;
    }
}

} // namespace Item
