#include "sp/SaveStateManager.hh"

#include <game/system/SaveManager.hh>
extern "C" {
#include <revolution.h>
#include <sp/Commands.h>
}

#include <cstring>

sp_define_command("/example_command", "Example command", const char *tmp) {
    (void)tmp;
}

sp_define_command("/store", "", const char *tmp) {
    (void)tmp;

    if (auto *saveStateManager = SP::SaveStateManager::Instance()) {
        saveStateManager->save();
    } else {
        OSReport("SaveStateManager not initialized\n");
    }
}

sp_define_command("/reload", "", const char *tmp) {
    (void)tmp;

    if (auto *saveStateManager = SP::SaveStateManager::Instance()) {
        saveStateManager->reload();
    } else {
        OSReport("SaveStateManager not initialized\n");
    }
}
