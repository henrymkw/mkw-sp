#pragma once

#include <revolution/dwc/DWCNode.h>

#define MATCH_MAKING_INFO 0x77846772

typedef struct {
    u32 aidBitmap;
    u32 numAids;
    u32 directConnectedAidBitmap;
    u32 roomId;
    u8 myAid;
    u8 hostAid;
    bool suspended; // note to self: the NetManager enum has values 0 thru 3
    bool canceled;
    DWCConnectionUserData localPlayerCount[12];
} MatchMakingInfoPacket;
static_assert(sizeof(MatchMakingInfoPacket) == 0x44);

extern MatchMakingInfoPacket g_recvMatchPacket;

bool recvMatchMakingInfoPacket();
