#include "DWCMatch.h"

#include "DWCMain.h"

#include <sp/net/mkw_server/RoomManager.h>

bool DWC_SetupGameServer(void * /* r3 */, void * /* r4 */, void * /* r5 */, void * /* r6 */,
        void * /* r7 */, void * /* r8 */, void * /* r9 */, void * /* r10 */) {
    if (connectToRoomManager()) {
        return sendOpenFroomRequest();
    }
    return false;
}

bool DWC_ConnectToGameServerAsync(s32 friendId, void * /* r4 */, void * /* r5 */, void * /* r6 */,
        void * /* r7 */, void * /* r8 */, void * /* r9 */, void * /* r10 */) {
    // need to get the friend profile id
    s32 friendProfileId = DWCi_GetProfileIDFromList(friendId);
    if (friendProfileId == 0) {
        SP_LOG("Failed to get friend profile ID for friend ID %d", friendId);
        return false;
    }

    if (connectToRoomManager()) {
        return sendJoinFriendRequest(friendProfileId, SEARCH_REGION_PRIVATE);
    }

    return false;
}

void DWC_ProcessFriendsMatch() {
    // Prevent vanilla matchmaking logic from running
    if (s_dwcControl->state == DWC_STATE_MATCHING) {
        return;
    }
    REPLACED(DWC_ProcessFriendsMatch)();
}
