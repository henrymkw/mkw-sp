#include "RoomManager.hh"

#include <string.h>

#include <sp/net/WiiLink.hh>
#include <sp/net/mkw_server/MKWServer.hh>
#include <sp/net/mkw_server/packets/JoinFriendRequest.hh>
#include <sp/net/mkw_server/packets/LocalPlayerCount.hh>
#include <sp/net/mkw_server/packets/MKWServerInfo.hh>
#include <sp/net/mkw_server/packets/MatchRequestHeader.hh>
#include <sp/net/mkw_server/packets/SearchRoomRequest.hh>
#include <sp/net/mkw_server/packets/SuspendRequest.hh>
namespace MKWServer {
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

// returns true when a MatchMakingInfoPacket can be processed, otherwise false
bool recvFromRoomManager() {
    if (g_matchMakingSocket == -1) {
        return false;
    }

    bool recvMMInfo = false;

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
            return false;
        }

        // receive the rest of the packet if possible
        if (recvIntoBuffer(packetSize) < packetSize) {
            return false;
        }

        switch (magic) {
        case MATCH_MAKING_INFO: {
            recvMMInfo = processMatchMakingInfoPacket(s_recvBuf + 4);
            if (!recvMMInfo) {
                SP_LOG("processMatchMakingInfoPacket() failed!");
            }
            break;
        }
        case MKW_SERVER_INFO:
            processMKWServerInfoPacket(s_recvBuf + 4);
            break;
        }

        consumeBuffer(packetSize);
    }

    return recvMMInfo;
}

static bool sendToRoomManager(void *message, s32 messageLength) {
    // check if we're connected to the server
    if (g_matchMakingSocket == -1) {
        SP_LOG("Not connected to room manager server!");
        return false;
    }

    return SOSend(g_matchMakingSocket, message, messageLength, 0);
}

bool sendOpenRoomRequest() {
    MatchRequestHeader openRoomRequest{};
    createMatchRequestHeader(&openRoomRequest, MatchRequestType::OpenRoom);

    SP_LOG("Sent OpenRoom request!");
    return sendToRoomManager(&openRoomRequest, sizeof(openRoomRequest));
}

bool sendJoinFriendRequest(s32 friendProfileId, SearchRegion searchRegion) {
    JoinFriendRequestPacket joinRequest{};
    createMatchRequestHeader(&joinRequest.header, MatchRequestType::JoinFriend);

    joinRequest.friendProfileId = friendProfileId;
    joinRequest.searchRegion = searchRegion;

    SP_LOG("Sending JoinFriendRequest where friendProfileId is %d and searchRegion is %d",
            friendProfileId, searchRegion);
    return sendToRoomManager(&joinRequest, sizeof(joinRequest));
}

bool sendLeaveRoomRequest() {
    MatchRequestHeader leaveRoomRequest{};
    createMatchRequestHeader(&leaveRoomRequest, MatchRequestType::LeaveRoom);

    SP_LOG("Sending LeaveRoom request!");
    return sendToRoomManager(&leaveRoomRequest, sizeof(leaveRoomRequest));
}

bool sendSuspendRequest(bool suspendVote) {
    SuspendRequestPacket suspendRequest{};
    createMatchRequestHeader(&suspendRequest.header, MatchRequestType::Suspend);
    suspendRequest.suspendVote = suspendVote;

    // TODO: Only send vote when it has changed
    return sendToRoomManager(&suspendRequest, sizeof(suspendRequest));
}

bool sendSearchRoomRequest(SearchRegion region, GameMode gameMode) {
    SearchRoomRequestPacket searchRoomRequest{};
    createMatchRequestHeader(&searchRoomRequest.header, MatchRequestType::SearchRoom);
    searchRoomRequest.region = region;
    searchRoomRequest.gameMode = gameMode;

    SP_LOG("Sending SearchRoomRequest where region: %d and gameMode: %d", region,
            static_cast<u32>(gameMode));
    return sendToRoomManager(&searchRoomRequest, sizeof(searchRoomRequest));
}

bool sendLocalPlayerCount(u8 localPlayerCount) {
    LocalPlayerCountPacket lpcPacket{};
    createMatchRequestHeader(&lpcPacket.header, MatchRequestType::LocalPlayerCount);
    lpcPacket.localPlayerCount = localPlayerCount;

    SP_LOG("Sending LocalPlayerCountPacket where localPlayerCount is %d", localPlayerCount);
    return sendToRoomManager(&lpcPacket, sizeof(lpcPacket));
}
} // namespace MKWServer
