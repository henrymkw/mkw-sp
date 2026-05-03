#include "MatchMakingInfo.hh"

#include <string.h>

namespace Net {

void MatchMakingInfo::reset() {
    availableAids.reset();
    numAids = 0;
    numPlayers = 0;
    connectedToAids.reset();
    groupId = 0;
    myAid = 0xff;
    hostAid = 0xff;
    memset(localPlayerCounts, 0, sizeof(localPlayerCounts));
}

void MatchMakingInfo::importMKWServerMMInfo(MatchMakingInfoPacket *incomingPacket) {
    availableAids = incomingPacket->aidBitmap;
    numAids = incomingPacket->numAids;
    connectedToAids = incomingPacket->directConnectedAidBitmap;
    groupId = incomingPacket->roomId;
    myAid = incomingPacket->myAid;
    hostAid = incomingPacket->hostAid;

    // get localplayer counts one at a time
    u32 currNumPlayers = 0;
    for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
        if (aid == myAid) {
            // This gets set before requesting to join a room
            currNumPlayers += myLocalPlayerCount;
            localPlayerCounts[aid].playersAtConsole = myLocalPlayerCount;
        } else {
            u8 aidsLocalPlayers = incomingPacket->localPlayerCount[aid].playersAtConsole;
            currNumPlayers += aidsLocalPlayers;
            localPlayerCounts[aid].playersAtConsole = aidsLocalPlayers;
        }
    }
    numPlayers = currNumPlayers;
    isMatchMakingSuspended = incomingPacket->suspended;
}

} // namespace Net
