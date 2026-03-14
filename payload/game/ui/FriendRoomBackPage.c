#include "FriendRoomBackPage.h"

#include <revolution.h>

#include <sp/net/mkw_server/MatchMaking.h>

void onDeactivate() {
    sendLeaveFroomRequest();
    REPLACED(onDeactivate)();
}
