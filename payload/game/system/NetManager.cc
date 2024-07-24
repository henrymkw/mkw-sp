#include "NetManager.hh"

#include "NandManager.hh"
#include <game/RKNet/FriendManager.hh>
#include <game/system/SaveManager.hh>

extern "C" {
#include <revolution.h>
}

namespace System {

void NetManager::Connect() {
    auto *data = SaveManager::Instance()->getDWCUserData();
    RKNet::FriendManager::Instance()->setDWCUserData(data);
    REPLACED(Connect)();
}

} // namespace System