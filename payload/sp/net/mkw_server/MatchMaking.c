#include "MatchMaking.h"

#include <string.h>

#include <sp/net/WiiLink.h>
#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/packets/JoinFriendRequest.h>
#include <sp/net/mkw_server/packets/LocalPlayerCount.h>
#include <sp/net/mkw_server/packets/MKWServerInfo.h>
#include <sp/net/mkw_server/packets/MatchRequestHeader.h>
#include <sp/net/mkw_server/packets/SearchRoomRequest.h>
#include <sp/net/mkw_server/packets/SuspendRequest.h>

static SOSockAddrIn s_serverAddr;
SOCKET g_matchMakingSocket = -1;
static s32 connection = -1;

bool connectToRoomManager() {
    if (connection == 0) {
        // SP_LOG("Already connected to Room Manager!"); TODO: Unsupress!
        return true;
    }

    s_serverAddr.family = AF_INET;
    s_serverAddr.port = SOHtoNs(28910);

    // The wfc payload patches inet_addr to replace nintendowifi.net with whatever domain is used,
    // so this works (just unclear without comment, maybe should be changed to be a client patch)
    s_serverAddr.addr.addr = getWFCServerAddress();

    if (g_matchMakingSocket == -1) {
        g_matchMakingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (g_matchMakingSocket == -1) {
            SP_LOG("Failed to create room manager socket!");
            return false;
        }
    }

    s_serverAddr.len = sizeof(s_serverAddr);
    connection = SOConnect(g_matchMakingSocket, &s_serverAddr);

    if (connection != 0) {
        SP_LOG("Failed to connect to room manager server. connection: %d", connection);
        return false;
    }

    // set non-blocking, credits: vabold
    s32 result = SOFcntl(g_matchMakingSocket, SO_F_GETFL, 0);
    if (result == -1) {
        SP_LOG("Failed to get status flags, returned %d", result);
        return false;
    }

    result = SOFcntl(g_matchMakingSocket, SO_F_SETFL, result | SO_O_NONBLOCK);
    if (result != 0) {
        SP_LOG("Failed to set status flags, returned %d", result);
        return false;
    }

    return true;
}

void resetRoomManagerConnection() {
    if (g_matchMakingSocket != -1 && connection == 0) {
        SOClose(g_matchMakingSocket);
        g_matchMakingSocket = -1;
        connection = -1;
    }
}

bool sendToRoomManager(void *message, s32 messageLength) {
    // check if we're connected to the server
    if (g_matchMakingSocket == -1) {
        SP_LOG("Not connected to room manager server!");
        return false;
    }

    return SOSend(g_matchMakingSocket, message, messageLength, 0);
}

bool recvFromRoomManager() {
    if (g_matchMakingSocket == -1) {
        return false;
    }

    // first recv the magic, return if recv isn't 4 (i think)
    // 4 indicates sucessful recv
    u32 magic;
    s32 magicRecv = SORecv(g_matchMakingSocket, (void *)&magic, sizeof(u32), 0);
    if (magicRecv != 4) {
        return false;
    }

    bool dataRecvResult = false;
    switch (magic) {
    case MATCH_MAKING_INFO:
        dataRecvResult = recvMatchMakingInfoPacket();
        if (!dataRecvResult) {
            SP_LOG("Got Match Making Info magic (%d) but recvMatchMakingInfoPacket() returned "
                   "false!",
                    MATCH_MAKING_INFO);
        }
        return dataRecvResult;

        break;
    case MKW_SERVER_INFO:
        dataRecvResult = recvMKWServerInfoPacket();
        if (!dataRecvResult) {
            SP_LOG("Got MKWServerInfo magic (%d) but recvMKWServerInfoPacket() returned false",
                    MKW_SERVER_INFO);
        }
        return dataRecvResult;

        break;
    default:
        SP_LOG("Received unknown packet type with magic %x", magic);
        break;
    }

    return false;
}

bool sendOpenFroomRequest() {
    MatchRequestHeader openRoomRequest;
    createMatchRequestHeader(&openRoomRequest, MATCH_REQUEST_OPEN_ROOM, g_wfcSearchId);

    SP_LOG("OpenFroom request header magic: %d", openRoomRequest.magic);

    return sendToRoomManager(&openRoomRequest, sizeof(openRoomRequest));
}

bool sendJoinFriendRequest(s32 friendProfileId, SearchRegion searchRegion) {
    JoinFriendRequestPacket joinRequest;
    memset(&joinRequest, 0, sizeof(JoinFriendRequestPacket));
    createMatchRequestHeader(&joinRequest.header, MATCH_REQUEST_JOIN_FRIEND, g_wfcSearchId);

    joinRequest.friendProfileId = friendProfileId;
    joinRequest.searchRegion = searchRegion;

    return sendToRoomManager(&joinRequest, sizeof(joinRequest));
}

bool sendLeaveFroomRequest() {
    SP_LOG("Sending a leave request!");
    MatchRequestHeader leaveRoomRequest;
    createMatchRequestHeader(&leaveRoomRequest, MATCH_REQUEST_LEAVE_ROOM, g_wfcSearchId);

    return sendToRoomManager(&leaveRoomRequest, sizeof(leaveRoomRequest));
}

bool sendSuspendRequest(bool suspendVote) {
    SuspendRequestPacket suspendRequest;
    createMatchRequestHeader(&suspendRequest.header, MATCH_REQUEST_SUSPEND, g_wfcSearchId);
    suspendRequest.suspendVote = suspendVote;

    // TODO: Only send vote when it has changed
    if (sizeof(suspendRequest) != 0x18) {
        SP_LOG("SuspendRequest size isn't 0x18. Actual: %d. Vote: %d", sizeof(suspendRequest),
                suspendRequest.suspendVote);
    }
    return sendToRoomManager(&suspendRequest, sizeof(suspendRequest));
}

bool sendSearchRoomRequest(SearchRegion region, GameMode gameMode) {
    SearchRoomRequestPacket searchRoomRequest;
    memset(&searchRoomRequest, 0, sizeof(searchRoomRequest));
    createMatchRequestHeader(&searchRoomRequest.header, MATCH_REQUEST_SEARCH_ROOM, g_wfcSearchId);
    searchRoomRequest.region = region;
    searchRoomRequest.gameMode = gameMode;

    SP_LOG("Sending SearchRoomRequest where region: %d and gameMode: %d", region, gameMode);

    SP_LOG("Search request header magic: %d and size %d", searchRoomRequest.header.magic,
            sizeof(searchRoomRequest));
    return sendToRoomManager(&searchRoomRequest, sizeof(searchRoomRequest));
}

bool sendLocalPlayerCount(u8 localPlayerCount) {
    LocalPlayerCountPacket lpcPacket;
    memset(&lpcPacket, 0, sizeof(LocalPlayerCountPacket));
    createMatchRequestHeader(&lpcPacket.header, MATCH_REQUEST_LOCAL_PLAYER_COUNT, g_wfcSearchId);
    lpcPacket.localPlayerCount = localPlayerCount;

    SP_LOG("Sending LocalPlayerCountPacket where localPlayerCount is %d", localPlayerCount);
    return sendToRoomManager(&lpcPacket, sizeof(lpcPacket));
}
