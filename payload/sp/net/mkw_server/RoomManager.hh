#pragma once

#include <Common.hh>

extern "C" {
#include <revolution/so/so.h>
}

#include <sp/net/mkw_server/packets/MatchMakingInfo.hh>
namespace MKWServer {
enum class MatchRequestType : u8 {
    OpenRoom = 0,
    JoinFriend = 1,
    LeaveRoom = 2,
    Suspend = 3,
    SearchRoom = 4,
    LocalPlayerCount = 5,
};

enum class GameMode : u8 {
    Undecided = 0,
    VS = 1,
    Battle = 2,
};

// opens up a tcp connection, called when player wants to create or join a room
EXTERN_C bool connectToRoomManager();

// closes the tcp connection. called upon match making state exit.
void resetRoomManagerConnection();

// Receives and processes as many packets as possible in a single call
void recvFromRoomManager();

EXTERN_C bool sendOpenRoomRequest();

EXTERN_C bool sendJoinFriendRequest(s32 friendProfileId, SearchRegion searchRegion);

bool sendLeaveRoomRequest();

bool sendSearchRoomRequest(SearchRegion region, GameMode gameMode);

EXTERN_C bool sendSuspendRequest(bool suspendVote);

bool sendLocalPlayerCount(u8 localPlayerCount);
} // namespace MKWServer