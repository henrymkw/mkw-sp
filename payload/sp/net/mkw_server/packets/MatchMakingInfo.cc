#include "MatchMakingInfo.hh"

extern "C" {
#include <revolution.h>
}

#include <sp/net/mkw_server/RoomManager.hh>

#include <string.h>

MatchMakingInfoPacket g_recvMatchPacket;

bool matchMakingInfoValid(MatchMakingInfoPacket *mmInfo) {
    // Check that none of bits 12-31 are set
    if (mmInfo->aidBitmap & ~0xfff) {
        SP_LOG("Aid bitmap is invalid! Value: %d", mmInfo->aidBitmap);
        return false;
    }
    if (mmInfo->directConnectedAidBitmap & ~0xfff) {
        SP_LOG("Direct connected aid bitmap is invalid! Value: %d",
                mmInfo->directConnectedAidBitmap);
        return false;
    }

    if (mmInfo->numAids > 12) {
        SP_LOG("Number of aids is invalid! Value: %d", mmInfo->numAids);
        return false;
    }

    // 11 is the highest valued aid
    if (mmInfo->myAid > 11 && mmInfo->myAid != 0xff) {
        SP_LOG("My aid is invalid! Value: %d", mmInfo->myAid);
        return false;
    }
    if (mmInfo->hostAid > 11 && mmInfo->hostAid != 0xff) {
        SP_LOG("Host's aid is invalid! Value: %d", mmInfo->hostAid);
        return false;
    }

    // check that my aid is in the aid bitmap
    if (mmInfo->aidBitmap != 0 && !(mmInfo->aidBitmap & (1 << mmInfo->myAid))) {
        SP_LOG("My aid (%d) isn't set in the aid bitmap (%d)", mmInfo->myAid, mmInfo->aidBitmap);
        return false;
    }

    // Only valid local player counts are 0 (not in room), 1, and 2
    for (u32 i = 0; i < 12; i++) {
        u8 aidLocalPlayerCount = mmInfo->localPlayerCount[i].playersAtConsole;
        if (aidLocalPlayerCount > 2) {
            SP_LOG("Aid %d reported an invalid localPlayers (%d)!", i, aidLocalPlayerCount);
            return false;
        }
    }
    return true;
}

bool processMatchMakingInfoPacket(u8 *data) {
    // Cast data into MatchMakingInfoPacket to be validated
    MatchMakingInfoPacket *mmInfo = (MatchMakingInfoPacket *)data;
    if (!matchMakingInfoValid(mmInfo)) {
        return false;
    }

    // Copy to the recv buffer and return
    memcpy(&g_recvMatchPacket, mmInfo, sizeof(MatchMakingInfoPacket));
    return true;
}

void resetMatchMakingInfoPacket() {
    memset(&g_recvMatchPacket, 0, sizeof(MatchMakingInfoPacket));
}

extern "C" {
MatchMakingInfoPacket *mmInfo() {
    return &g_recvMatchPacket;
}
}
