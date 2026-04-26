#pragma once

#include <Common.h>

#include <revolution/so/so.h>

#include <sp/net/mkw_server/packets/MatchMakingInfo.h>

extern SOCKET g_matchMakingSocket;

typedef enum {
    MATCH_REQUEST_OPEN_ROOM = 0,
    MATCH_REQUEST_JOIN_FRIEND = 1,
    MATCH_REQUEST_LEAVE_ROOM = 2,
    MATCH_REQUEST_SUSPEND = 3,
    MATCH_REQUEST_SEARCH_ROOM = 4,
    MATCH_REQUEST_LOCAL_PLAYER_COUNT = 5,
} MatchRequestType;

typedef enum {
    SEARCH_REGION_NONE = -1,
    SEARCH_REGION_PRIVATE = 0,
    SEARCH_REGION_WW = 1,
    SEARCH_REGION_NA = 2,
    SEARCH_REGION_EU = 3,
    SEARCH_REGION_JP = 4,
    SEARCH_REGION_KOR = 5,
} SearchRegion;

typedef enum {
    GAME_MODE_UNDECIDED = 0,
    GAME_MODE_VS = 1,
    GAME_MODE_BATTLE = 2,
} GameMode;

// opens up a tcp connection, called when player wants to create or join a room
bool connectToRoomManager();

// closes the tcp connection. called upon match making state exit.
void resetRoomManagerConnection();

// sends a generic message to room manager
bool sendToRoomManager(void *message, s32 messageLength);

bool recvFromRoomManager();

// sends a REQUEST_OPEN_ROOM
bool sendOpenFroomRequest();

bool sendJoinFriendRequest(s32 friendProfileId, SearchRegion searchRegion);

bool sendLeaveRoomRequest();

bool sendSuspendRequest(bool suspendVote);

bool sendSearchRoomRequest(SearchRegion region, GameMode gameMode);

bool sendLocalPlayerCount(u8 localPlayerCount);
