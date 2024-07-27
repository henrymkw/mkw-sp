#include "NetManager.hh"

#include "SaveManager.hh"
#include <game/RKNet/FriendManager.hh>

namespace System {

void NetManager::Connect() {
    // Get DWCUserData from SaveManager and set it in FriendManager before running the original
    // function.
    auto *data = SaveManager::Instance()->getDWCUserData();
    RKNet::FriendManager::Instance()->setDWCUserData(data);
    REPLACED(Connect)();
}

} // namespace System