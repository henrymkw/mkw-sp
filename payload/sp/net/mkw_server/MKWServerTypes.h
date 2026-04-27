#pragma once

#include <Common.h>

#include <revolution/dwc/DWCNode.h>

// Some types are shared between C and C++ files. To prevent conflicts from this,
// these sorts of types are defined here in C.

typedef struct {
    u32 aidBitmap;
    u32 numAids;
    u32 directConnectedAidBitmap;
    u32 roomId;
    u8 myAid;
    u8 hostAid;
    bool suspended;
    bool canceled;
    DWCConnectionUserData localPlayerCount[12];
} MatchMakingInfoPacket;
static_assert(sizeof(MatchMakingInfoPacket) == 0x44);

typedef enum {
    SEARCH_REGION_NONE = -1,
    SEARCH_REGION_PRIVATE = 0,
    SEARCH_REGION_WW = 1,
    SEARCH_REGION_NA = 2,
    SEARCH_REGION_EU = 3,
    SEARCH_REGION_JP = 4,
    SEARCH_REGION_KOR = 5,
} SearchRegion;

// different race packet types, currently we check the first byte (which should be reconsidered at
// some point) for the type of packet
typedef enum {
    MKW_SERVER_RACE_PACKET = 0xB,
} MKWServerPacketType;
