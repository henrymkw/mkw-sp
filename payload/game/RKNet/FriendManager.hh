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

    static FriendManager* Instance() {
        return s_instance;
    }

    /**
     * @brief This really should not be publ
     * 
     * @param data 
     */
    void setDWCUserData(DWCUserData *data) {
        myUserData[0] = *data;
    }

private:
    u8 _000[0x01c - 0x000];
    DWCUserData myUserData[2]; // presumably two for two players
    u8 _09c[0x3e8 - 0x09c];

    static FriendManager *s_instance;
};
static_assert(sizeof(FriendManager) == 0x3e8);

} // namespace RKNet