#include "DWCMain.h"

#include <revolution.h>

#include <sp/net/mkw_server/RoomManager.h>
#include <sp/net/mkw_server/packets/MatchMakingInfo.h>

u32 DWC_GetNumConnectionsHost() {
    return mmInfo()->numAids;
}

u32 DWC_GetGroupId() {
    return mmInfo()->roomId;
}

u8 DWC_GetSuspendMatch() {
    return mmInfo()->suspended;
}

u8 DWC_IsValidMatchCancel() {
    return mmInfo()->canceled;
}

bool DWC_RequestSuspendMatchAsync(bool suspendVote) {
    if (connectToRoomManager()) {
        return sendSuspendRequest(suspendVote);
    }
    return false;
}
