#include "MatchMakingInfo.h"

#include <revolution.h>
#include <revolution/so/so.h>

#include <sp/net/mkw_server/RoomManager.h>

#include <string.h>

MatchMakingInfoPacket g_recvMatchPacket;

bool matchMakingInfoValid(MatchMakingInfoPacket *mmInfo) {
    // Check that none of bits 12-31 are set
    if (mmInfo->aidBitmap & ~0xfff) {
        return false;
    }
    if (mmInfo->directConnectedAidBitmap & ~0xfff) {
        return false;
    }

    if (mmInfo->numAids > 12) {
        return false;
    }

    // 11 is the highest valued aid
    if (mmInfo->myAid > 11) {
        return false;
    }
    if (mmInfo->hostAid > 11) {
        return false;
    }

    // Only valid local player counts are 0 (not in room), 1, and 2
    for (u32 i = 0; i < 12; i++) {
        if (mmInfo->localPlayerCount[i].playersAtConsole > 2) {
            return false;
        }
    }
    return true;
}

bool recvMatchMakingInfoPacket() {
    MatchMakingInfoPacket resp;
    s32 recvResult = SORecv(g_matchMakingSocket, &resp, sizeof(MatchMakingInfoPacket), 0);

    if (recvResult > 0) {
        if (!matchMakingInfoValid(&resp)) {
            return false;
        }
        // we should probably call a function that validates the received packet
        memcpy(&g_recvMatchPacket, &resp, sizeof(MatchMakingInfoPacket));

        return true;
    }

    return false;
}
