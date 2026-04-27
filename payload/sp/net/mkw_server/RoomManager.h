#pragma once

#include <Common.h>

typedef enum {
    SEARCH_REGION_NONE = -1,
    SEARCH_REGION_PRIVATE = 0,
    SEARCH_REGION_WW = 1,
    SEARCH_REGION_NA = 2,
    SEARCH_REGION_EU = 3,
    SEARCH_REGION_JP = 4,
    SEARCH_REGION_KOR = 5,
} SearchRegion;

// opens up a tcp connection, called when player wants to create or join a room
EXTERN_C bool connectToRoomManager();

// sends a REQUEST_OPEN_ROOM
EXTERN_C bool sendOpenFroomRequest();

EXTERN_C bool sendJoinFriendRequest(s32 friendProfileId, SearchRegion searchRegion);

EXTERN_C bool sendSuspendRequest(bool suspendVote);
