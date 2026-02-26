#include "DWCMatch.h"

#include "DWCMain.h"

#include <sp/net/mkw_server/MatchMaking.h>

bool DWC_SetupGameServer(void * /* r3 */, void * /* r4 */, void * /* r5 */, void * /* r6 */,
        void * /* r7 */, void * /* r8 */, void * /* r9 */, void * /* r10 */) {
    return connectToRoomManager();
}

void DWC_ProcessFriendsMatch() {
    // Prevent vanilla matchmaking logic from running
    if (s_dwcControl->state == DWC_STATE_MATCHING) {
        return;
    }
    REPLACED(DWC_ProcessFriendsMatch)();
}
