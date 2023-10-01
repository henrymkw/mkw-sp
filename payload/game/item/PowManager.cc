#include "PowManager.hh"

#include "game/system/RaceManager.hh"
#include "game/system/SaveManager.hh"

#include "game/util/Registry.hh"

namespace Item {

PowManager *PowManager::Instance() {
    return s_instance;
}

void PowManager::unk(u32 playerIndex) {
    REPLACED(unk)(playerIndex);
}

} // namespace Item