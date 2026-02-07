#include "KartObjectManager.hh"

extern "C" {
#include "revolution.h"
}

namespace Kart {

KartObjectManager *KartObjectManager::CreateInstance() {
    auto *kartObjectManager = REPLACED(CreateInstance)();
    if (!kartObjectManager) {
        SP_LOG("KartObjectManager::CreateInstance: Failed to create instance!");
    }
    s_instance = kartObjectManager;
    return s_instance;
}

} // namespace Kart