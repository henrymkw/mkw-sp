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

        return true;
    }

    return false;
}
