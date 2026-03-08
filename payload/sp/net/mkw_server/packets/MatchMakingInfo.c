#include "MatchMakingInfo.h"

#include <revolution.h>
#include <revolution/so/so.h>

#include <sp/net/mkw_server/MatchMaking.h>

#include <string.h>

MatchMakingInfoPacket g_recvMatchPacket;

bool recvMatchMakingInfoPacket() {
    MatchMakingInfoPacket resp;
    s32 recvResult = SORecv(g_MatchMakingSocket, (void *)&resp, sizeof(MatchMakingInfoPacket), 0);

    if (recvResult > 0) {
        // we should probably call a function that validates the received packet
        memcpy(&g_recvMatchPacket, &resp, sizeof(MatchMakingInfoPacket));

        /*
        SP_LOG("aidBitmap %d numAids %d directConnectedAidBitmap %d roomId %d myAid %d hostAid %d",
                g_recvMatchPacket.aidBitmap, g_recvMatchPacket.numAids,
                g_recvMatchPacket.directConnectedAidBitmap, g_recvMatchPacket.roomId,
                g_recvMatchPacket.myAid, g_recvMatchPacket.hostAid);
        */

        return true;
    }

    return false;
}
