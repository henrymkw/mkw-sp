#include "DWCMain.h"

#include <revolution.h>

#include <sp/net/mkw_server/RoomManager.h>

u32 DWC_GetAidBitmap() {
    return g_recvMatchPacket.aidBitmap;
}

u32 DWC_GetNumConnectionsHost() {
    return g_recvMatchPacket.numAids;
}

u32 DWC_GetDirectConnectedAidBitmap() {
    return g_recvMatchPacket.directConnectedAidBitmap;
}

u32 DWC_GetGroupId() {
    return g_recvMatchPacket.roomId;
}

u8 DWC_GetMyAid() {
    return g_recvMatchPacket.myAid;
}

u8 DWC_GetServerAid() {
    return g_recvMatchPacket.hostAid;
}

u8 DWC_GetSuspendMatch() {
    return g_recvMatchPacket.suspended;
}

u8 DWC_IsValidMatchCancel() {
    return g_recvMatchPacket.canceled;
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

    playerCount->playersAtConsole = g_recvMatchPacket.localPlayerCount[aid].playersAtConsole;
    return true;
}

bool DWC_RequestSuspendMatchAsync(bool suspendVote) {
    if (connectToRoomManager()) {
        return sendSuspendRequest(suspendVote);
    }
    return false;
}
