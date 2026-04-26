#include "RoomManager.h"

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
static SOCKET g_matchMakingSocket = -1;
static s32 connection = -1;

static u8 s_recvBuf[256];
static s32 s_recvBufSize = 0;

bool connectToRoomManager() {
    if (connection == 0) {
        // Already connected
        return true;
    }

    s_serverAddr.family = AF_INET;
    s_serverAddr.port = SOHtoNs(28910);

    // The wfc payload patches SOInetAtoN() to replace nintendowifi.net with whatever domain is used
    const char *serverHostname = "mariokartwii.ms19.gs.nintendowifi.net";
    s32 addr;
    BOOL addrResult = SOInetAtoN(serverHostname, &addr);
    if (addrResult) {
        s_serverAddr.addr.addr = addr;
    } else {
        // SOInetAtoN() can fail on console for unknown reasons
        // If it does fail, fall back to WFC_SERVER_ADDRESS
        s_serverAddr.addr.addr = WFC_SERVER_ADDRESS;
    }

    if (g_matchMakingSocket == -1) {
        g_matchMakingSocket = SOSocket(AF_INET, SOCK_STREAM, 0);
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
    SP_LOG("Connected to RoomManager!");

    // set non-blocking, credits: vabold
    s32 result = SOFcntl(g_matchMakingSocket, SO_F_GETFL, 0);
    if (result == -1) {
        SP_LOG("Failed to get status flags, returned %d", result);
        resetRoomManagerConnection();
        return false;
    }

    result = SOFcntl(g_matchMakingSocket, SO_F_SETFL, result | SO_O_NONBLOCK);
    if (result != 0) {
        SP_LOG("Failed to set status flags, returned %d", result);
        resetRoomManagerConnection();
        return false;
    }

    return true;
}

void resetRoomManagerConnection() {
    if (g_matchMakingSocket != -1) {
        SOClose(g_matchMakingSocket);
        g_matchMakingSocket = -1;
        connection = -1;
        s_recvBufSize = 0;
        resetMatchMakingInfoPacket();
        resetMKWServerInfo();
        SP_LOG("RoomManager connection/socket has been reset!");
    }
}

static s32 recvIntoBuffer(s32 needed) {
    if (s_recvBufSize >= needed) {
        // already have needed bytes
        return s_recvBufSize;
    }
    // Try to receive, update size if successful.
    s32 result = SORecv(g_matchMakingSocket, s_recvBuf + s_recvBufSize,
            sizeof(s_recvBuf) - s_recvBufSize, 0);
    if (result > 0) {
        s_recvBufSize += result;
    }
    return s_recvBufSize;
}

static void consumeBuffer(s32 n) {
    // Discard first n bytes by shifting remaining data forward
    memmove(s_recvBuf, s_recvBuf + n, s_recvBufSize - n);
    s_recvBufSize -= n;
}

void recvFromRoomManager() {
    if (g_matchMakingSocket == -1) {
        return;
    }

    // loop until we can't process anymore packets
    while (true) {
        // check if there is any magic that can be received
        if (recvIntoBuffer(4) < 4) {
            break;
        }

        u32 magic;
        memcpy(&magic, s_recvBuf, sizeof(u32));

        s32 packetSize = 0;
        // identify the type of packet based off the magic set the size
        switch (magic) {
        case MATCH_MAKING_INFO:
            packetSize = sizeof(u32) + sizeof(MatchMakingInfoPacket);
            break;
        case MKW_SERVER_INFO:
            packetSize = sizeof(u32) + sizeof(MKWServerInfoPacket);
            break;
        default:
            SP_LOG("Received unknown packet type with magic %x", magic);
            s_recvBufSize = 0;
            return;
        }

        // receive the rest of the packet if possible
        if (recvIntoBuffer(packetSize) < packetSize) {
            return;
        }

        switch (magic) {
        case MATCH_MAKING_INFO:
            bool processResult = processMatchMakingInfoPacket(s_recvBuf + 4);
            if (!processResult) {
                SP_LOG("processMatchMakingInfoPacket() failed!");
            }
            break;
        case MKW_SERVER_INFO:
            processMKWServerInfoPacket(s_recvBuf + 4);
            break;
        }

        consumeBuffer(packetSize);
    }
}

static bool sendToRoomManager(void *message, s32 messageLength) {
    // check if we're connected to the server
    if (g_matchMakingSocket == -1) {
        SP_LOG("Not connected to room manager server!");
        return false;
    }

    return SOSend(g_matchMakingSocket, message, messageLength, 0);
}

bool sendOpenFroomRequest() {
    MatchRequestHeader openRoomRequest;
    createMatchRequestHeader(&openRoomRequest, MATCH_REQUEST_OPEN_ROOM, g_wfcSearchId);

    SP_LOG("Sent OpenFroom request!");
    return sendToRoomManager(&openRoomRequest, sizeof(openRoomRequest));
}

bool sendJoinFriendRequest(s32 friendProfileId, SearchRegion searchRegion) {
    JoinFriendRequestPacket joinRequest;
    memset(&joinRequest, 0, sizeof(JoinFriendRequestPacket));
    createMatchRequestHeader(&joinRequest.header, MATCH_REQUEST_JOIN_FRIEND, g_wfcSearchId);

    joinRequest.friendProfileId = friendProfileId;
    joinRequest.searchRegion = searchRegion;

    SP_LOG("Sending JoinFriendRequest where friendProfileId is %d and searchRegion is %d",
            friendProfileId, searchRegion);
    return sendToRoomManager(&joinRequest, sizeof(joinRequest));
}

bool sendLeaveRoomRequest() {
    MatchRequestHeader leaveRoomRequest;
    createMatchRequestHeader(&leaveRoomRequest, MATCH_REQUEST_LEAVE_ROOM, g_wfcSearchId);

    SP_LOG("Sending LeaveRoom request!");
    return sendToRoomManager(&leaveRoomRequest, sizeof(leaveRoomRequest));
}

bool sendSuspendRequest(bool suspendVote) {
    SuspendRequestPacket suspendRequest;
    createMatchRequestHeader(&suspendRequest.header, MATCH_REQUEST_SUSPEND, g_wfcSearchId);
    suspendRequest.suspendVote = suspendVote;

    // TODO: Only send vote when it has changed
    return sendToRoomManager(&suspendRequest, sizeof(suspendRequest));
}

bool sendSearchRoomRequest(SearchRegion region, GameMode gameMode) {
    SearchRoomRequestPacket searchRoomRequest;
    memset(&searchRoomRequest, 0, sizeof(searchRoomRequest));
    createMatchRequestHeader(&searchRoomRequest.header, MATCH_REQUEST_SEARCH_ROOM, g_wfcSearchId);
    searchRoomRequest.region = region;
    searchRoomRequest.gameMode = gameMode;

    SP_LOG("Sending SearchRoomRequest where region: %d and gameMode: %d", region, gameMode);
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
