#pragma once

#include <Common.h>

#include <revolution/dwc/DWCNode.h>

typedef struct {
    u32 magic; // 0x77846772 ("MTCH")
    u32 aidBitmap;
    u32 numAids;
    u32 directConnectedAidBitmap;
    u32 roomId;
    u8 myAid;
    u8 hostAid;
    bool suspended; // note to self: the NetManager enum has values 0 thru 3
    bool canceled;
    DWCConnectionUserData localPlayerCount[12];
} MatchPacket;
static_assert(sizeof(MatchPacket) == 0x48);

extern MatchPacket g_recvMatchPacket;

typedef enum {
    MATCH_REQUEST_OPEN_ROOM = 0x0,
    MATCH_REQUEST_JOIN_ROOM = 0x1,
} MatchRequestType;

// opens up a tcp connection, called when player wants to create or join a room
bool connectToRoomManager();

// closes the tcp connection. called upon match making state exit.
void resetRoomManagerConnection();

// sends a generic message to room manager
bool sendToRoomManager(void *message, s32 messageLength);

bool recvFromRoomManager();

// sends a REQUEST_OPEN_ROOM
bool sendOpenFroomRequest();

bool sendJoinFroomRequest(s32 friendProfileId);
