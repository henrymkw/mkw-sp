#include "PowManager.hh"

#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include "game/util/Registry.hh"

namespace Item {

PowManager *PowManager::Instance() {
    return s_instance;
}

void PowManager::calc() {
    REPLACED(calc)();
}

void PowManager::beginPow(u32 playerIndex) {
    REPLACED(beginPow)(playerIndex);
    m_playerIdxAffectedByPow[0] = true;
}

} // namespace Item