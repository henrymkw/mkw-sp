#include "DWCMain.h"

#include <revolution.h>

#include <sp/net/mkw_server/RoomManager.h>
#include <sp/net/mkw_server/packets/MatchMakingInfo.h>

u32 DWC_GetAidBitmap() {
    return mmInfo()->aidBitmap;
}

u32 DWC_GetNumConnectionsHost() {
    return mmInfo()->numAids;
}

u32 DWC_GetDirectConnectedAidBitmap() {
    return mmInfo()->directConnectedAidBitmap;
}

u32 DWC_GetGroupId() {
    return mmInfo()->roomId;
}

u8 DWC_GetMyAid() {
    return mmInfo()->myAid;
}

u8 DWC_GetServerAid() {
    return mmInfo()->hostAid;
}

u8 DWC_GetSuspendMatch() {
    return mmInfo()->suspended;
}

u8 DWC_IsValidMatchCancel() {
    return mmInfo()->canceled;
}

bool DWC_GetConnectionUserData(u8 aid, DWCConnectionUserData *playerCount) {
    if (aid >= 12) {
        SP_LOG("Invalid aid %d, returning 0", aid);
        return false;
    }

    if (playerCount == NULL) {
        SP_LOG("playerCount is null, returning false");
        return false;
    }

    playerCount->playersAtConsole = mmInfo()->localPlayerCount[aid].playersAtConsole;
    return true;
}

bool DWC_RequestSuspendMatchAsync(bool suspendVote) {
    if (connectToRoomManager()) {
        return sendSuspendRequest(suspendVote);
    }
    return false;
}
