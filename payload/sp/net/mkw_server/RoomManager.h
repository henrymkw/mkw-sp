#pragma once

#include <Common.h>

#include <sp/net/mkw_server/MKWServerTypes.h>

// opens up a tcp connection, called when player wants to create or join a room
EXTERN_C bool connectToRoomManager();

// sends a REQUEST_OPEN_ROOM
EXTERN_C bool sendOpenRoomRequest();

EXTERN_C bool sendJoinFriendRequest(s32 friendProfileId, SearchRegion searchRegion);

EXTERN_C bool sendSuspendRequest(bool suspendVote);
