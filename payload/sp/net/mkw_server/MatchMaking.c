#include "MatchMaking.h"

#include <string.h>

#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/packets/JoinFroomRequest.h>
#include <sp/net/mkw_server/packets/MKWServerInfo.h>
#include <sp/net/mkw_server/packets/MatchMakingInfo.h>
#include <sp/net/mkw_server/packets/MatchRequestHeader.h>
#include <sp/net/mkw_server/packets/SuspendRequest.h>

static SOSockAddrIn s_serverAddr;
SOCKET g_MatchMakingSocket = -1;
static s32 connection = -1;

bool connectToRoomManager() {
    if (connection == 0) {
        SP_LOG("Already connected to Room Manager!");
        return true;
    }

    s_serverAddr.family = AF_INET;
    s_serverAddr.port = SOHtoNs(28910);

    // The wfc payload patches inet_addr to replace nintendowifi.net with whatever domain is used,
    // so this works (just unclear without comment, maybe should be changed to be a client patch)
    const char *serverHostname = "mariokartwii.ms19.gs.nintendowifi.net";
    s_serverAddr.addr.addr = inet_addr(serverHostname);

    if (g_MatchMakingSocket == -1) {
        g_MatchMakingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (g_MatchMakingSocket == -1) {
            SP_LOG("Failed to create room manager socket!");
            return false;
        }
    }

    s_serverAddr.len = sizeof(s_serverAddr);
    connection = SOConnect(g_MatchMakingSocket, &s_serverAddr);

    if (connection != 0) {
        SP_LOG("Failed to connect to room manager server. connection: %d", connection);
        return false;
    }

    // set non-blocking, credits: vabold
    s32 result = SOFcntl(g_MatchMakingSocket, SO_F_GETFL, 0);
    if (result == -1) {
        SP_LOG("Failed to get status flags, returned %d", result);
        return false;
    }

    result = SOFcntl(g_MatchMakingSocket, SO_F_SETFL, result | SO_O_NONBLOCK);
    if (result != 0) {
        SP_LOG("Failed to set status flags, returned %d", result);
        return false;
    }

    return true;
}

void resetRoomManagerConnection() {
    if (g_MatchMakingSocket != -1 && connection == 0) {
        SOClose(g_MatchMakingSocket);
        g_MatchMakingSocket = -1;
        connection = -1;
    }
}

bool sendToRoomManager(void *message, s32 messageLength) {
    // check if we're connected to the server
    if (g_MatchMakingSocket == -1) {
        SP_LOG("Not connected to room manager server!");
        return false;
    }

    return SOSend(g_MatchMakingSocket, message, messageLength, 0);
}

bool recvFromRoomManager() {
    if (g_MatchMakingSocket == -1) {
        return false;
    }

    // first recv the magic, return if recv isn't 4 (i think)
    // 4 indicates sucessful recv
    u32 magic;
    s32 magicRecv = SORecv(g_MatchMakingSocket, (void *)&magic, sizeof(u32), 0);
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
    createMatchRequestHeader(&openRoomRequest, MATCH_REQUEST_OPEN_ROOM, wfcSearchId);

    return sendToRoomManager(&openRoomRequest, sizeof(openRoomRequest));
}

bool sendJoinFroomRequest(s32 friendProfileId) {
    JoinFroomRequestPacket joinRequest;
    memset(&joinRequest, 0, sizeof(JoinFroomRequestPacket));
    createMatchRequestHeader(&joinRequest.header, MATCH_REQUEST_JOIN_ROOM, wfcSearchId);

    joinRequest.friendProfileId = friendProfileId;
    SP_LOG("Sending JoinFroomRequestPacket for friend profile ID: %d", friendProfileId);

    return sendToRoomManager(&joinRequest, sizeof(joinRequest));
}

bool sendLeaveFroomRequest() {
    MatchRequestHeader leaveRoomRequest;
    createMatchRequestHeader(&leaveRoomRequest, MATCH_REQUEST_LEAVE_ROOM, wfcSearchId);

    return sendToRoomManager(&leaveRoomRequest, sizeof(leaveRoomRequest));
}

bool sendSuspendRequest(bool suspendVote) {
    SuspendRequestPacket suspendRequest;
    createMatchRequestHeader(&suspendRequest.header, MATCH_REQUEST_SUSPEND, wfcSearchId);
    suspendRequest.suspendVote = suspendVote;

    // TODO: Only send vote when it has changed
    return sendToRoomManager(&suspendRequest, sizeof(suspendRequest));
}
