#include "DWCMain.h"

#include <revolution.h>

#include <sp/net/mkw_server/MatchMaking.h>

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

DWCConnectionUserData DWC_GetConnectionUserData(u8 aid) {
    if (aid >= 12) {
        SP_LOG("Invalid aid %d, returning 0", aid);
        DWCConnectionUserData zeros;
        zeros.playersAtConsole = 0;
        return zeros;
    }
    return g_recvMatchPacket.localPlayerCount[aid];
}
