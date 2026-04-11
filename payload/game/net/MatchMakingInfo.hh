#pragma once

#include <Common.hh>

extern "C" {
#include <revolution.h>
}

namespace Net {

struct MatchMakingInfo {
    // Time match making started
    OSTime matchMakingStartTime;

    // Number of aids in the room
    u32 numAids;

    // Number of players in the room. One aid can have a max of two players
    u32 numPlayers;

    // Bitmap of available aids, indexed by aid (1 << aid). When the bit position is 0,
    // that aid is available. When it's 1, that aid is used by another aid. availableAids
    // is also used to check if we're in a match. When availableAids != 0, we're in a match.
    u32 availableAids;

    // Aids we have established a connection with. This gets set, but isn't used
    u32 connectedToAids;

    // Identifier for the room a player is in. Group is the DWC term for a room
    u32 groupId;

    // Friend Id for host. Value is -1 if in a public room
    s32 hostFriendId;

    // My number of local players. 1 for single player, 2 for multiplayer.
    u8 myLocalPlayerCount;

    // Aid assigned to me.
    u8 myAid;

    // Aid of the host
    u8 hostAid;

    // Local player counts for each aid in little endian
    DWCConnectionUserData localPlayerCounts[MAX_PLAYER_COUNT];

    // Current suspension state of the room. When suspended, other players can't join.
    // When unsuspended, new players are able to join.
    bool isMatchMakingSuspended;

    // Padding
    u8 _54[0x58 - 0x54];
};
static_assert(sizeof(MatchMakingInfo) == 0x58);

} // namespace Net
