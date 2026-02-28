#pragma once

#include <Common.h>

#include <revolution/dwc/DWCNode.h>

typedef struct {
    u32 magic; // "MTCH"
    u32 aidBitmap;
    u32 numAids;
    u32 numPlayers;
    u32 roomID;
    u8 myAid;
    u8 hostAid;
    bool suspended; // note to self: the NetManager enum has values 0 thru 3
    bool canceled;
    DWCConnectionUserData localPlayerCount[12];
} MatchPacket;

typedef enum {
    MM_REQUEST_OPEN_ROOM = 0x0,
} MMRequestType;

// opens up a tcp connection, called when player wants to create or join a room
bool connectToRoomManager();

// closes the tcp connection. called upon match making state exit.
void resetRoomManagerConnection();

// sends a generic message to room manager
bool sendToRoomManager(void *message, s32 messageLength);

bool recvFromRoomManager();

// sends a REQUEST_OPEN_ROOM
bool sendOpenRoomRequest();
