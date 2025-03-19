#include "KartMove.hh"

#include "game/kart/KartObjectManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"

#include <sp/ThumbnailManager.hh>

namespace Kart {

f32 KartMove::hardSpeedLimit() const {
    return m_hardSpeedLimit;
}

const Vec3 *KartMove::internalVelDir() const {
    return &m_internalVelDir;
}

void KartMove::init(u32 r4, u32 r5) {
    REPLACED(init)(r4, r5);
    m_hardSpeedLimit = m_hardSpeedLimit * g_speedModFactor;
}

void KartMove::applyBulletSpeedLimit() {
    m_hardSpeedLimit = 145.0f * g_speedModFactor;
}

void KartMove::revertBulletSpeedLimit() {
    m_hardSpeedLimit = 120.0f * g_speedModFactor;
}

void KartMove::calcBlink() {
    if (SP::ThumbnailManager::IsActive()) {
        m_timeBeforeBlinkEnd = 150;
        getKartState()->m_blinking = true;
    }

    REPLACED(calcBlink)();
}

bool KartMove::activateTcLightning() {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();
    if (setting == SP::ClientSettings::VSMegaClouds::Enable) {
        KartMove::activateMega();
        return false;
    }

    return REPLACED(activateTcLightning)();
}

bool KartMove::applyLightningEffect(s32 timer, s32 r5, s32 r6) {
    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::TALongerTCs>();
    auto gameMode = System::RaceConfig::Instance()->raceScenario().gameMode;
    // since this setting is intended to used in TTs and can affect other modes, we check current mode
    if (gameMode == System::RaceConfig::GameMode::TimeAttack && setting == SP::ClientSettings::TALongerTCs::Enable) {
        // About 54 times longer duration.
        return REPLACED(applyLightningEffect)(0x7FFF, r5, r6);
    }
    else {
        return REPLACED(applyLightningEffect)(timer, r5, r6);
    }
}

} // namespace Kart
