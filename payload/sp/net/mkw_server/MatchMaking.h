#pragma once

#include <Common.h>

#include <revolution/so/so.h>

#include <sp/net/mkw_server/packets/MatchMakingInfo.h>

extern SOCKET g_MatchMakingSocket;

typedef enum {
    MATCH_REQUEST_OPEN_ROOM = 0,
    MATCH_REQUEST_JOIN_ROOM = 1,
    MATCH_REQUEST_LEAVE_ROOM = 2,
    MATCH_REQUEST_SUSPEND = 3,

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

bool sendLeaveFroomRequest();

bool sendSuspendRequest(bool suspendVote);
