#pragma once

#include <Common.hh>

extern "C" {
#include <revolution.h>
#include <revolution/dwc/dwc.h>
}

namespace RKNet {

class FriendManager {
public:
    FriendManager() = default;
    ~FriendManager() = default;

    static FriendManager *Instance() {
        return s_instance;
    }

    /**
     * @brief I would like this to be private, but currently its being used to fix an issue when
     * connecting to WWFC. The issue is that DWCUserData isn't being set, or is being reseted before
     * connecting to online. This means no user data exists, so a new one is created. This bug was
     * caused by something not known in SP (almost certainly related to license selection/creation).
     *
     * @param data The UserData to set
     */
    void setDWCUserData(DWCUserData *data) {
        myUserData[0] = *data;
        // TODO: What to do about the second user data field?
    }

private:
    u8 _000[0x01c - 0x000];
    DWCUserData myUserData[2]; // presumably two for two players
    u8 _09c[0x3e8 - 0x09c];

    static FriendManager *s_instance; // 0x809c2110
};
static_assert(sizeof(FriendManager) == 0x3e8);

} // namespace RKNet