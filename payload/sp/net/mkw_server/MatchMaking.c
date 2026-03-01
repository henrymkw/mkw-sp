#include "MatchMaking.h"

#include <string.h>

#include <revolution/so/so.h>

#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/MatchRequestHeader.h>

static SOSockAddrIn s_serverAddr;
static SOCKET s_socket = -1;
static s32 connection = -1;

MatchPacket g_recvMatchPacket;

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

    if (s_socket == -1) {
        s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s_socket == -1) {
            SP_LOG("Failed to create room manager socket!");
            return false;
        }
    }

    s_serverAddr.len = sizeof(s_serverAddr);
    connection = SOConnect(s_socket, &s_serverAddr);

    if (connection != 0) {
        SP_LOG("Failed to connect to room manager server. connection: %d", connection);
        return false;
    }

    // set non-blocking, credits: vabold
    s32 result = SOFcntl(s_socket, SO_F_GETFL, 0);
    if (result == -1) {
        SP_LOG("Failed to get status flags, returned %d", result);
        return false;
    }

    result = SOFcntl(s_socket, SO_F_SETFL, result | SO_O_NONBLOCK);
    if (result != 0) {
        SP_LOG("Failed to set status flags, returned %d", result);
        return false;
    }

    return true;
}

void resetRoomManagerConnection() {
    if (s_socket != -1 && connection == 0) {
        SOClose(s_socket);
        s_socket = -1;
        connection = -1;
    }
}

bool sendToRoomManager(void *message, s32 messageLength) {
    // check if we're connected to the server
    if (s_socket == -1) {
        SP_LOG("Not connected to room manager server!");
        return false;
    }

    return SOSend(s_socket, message, messageLength, 0);
}

bool recvFromRoomManager() {
    if (s_socket == -1) {
        return false;
    }

    MatchPacket resp;

    s32 recvResult = SORecv(s_socket, (void *)&resp, sizeof(MatchPacket), 0);

    if (recvResult > 0) {
        SP_LOG("Received MatchPacket from room manager: %d", recvResult);

        if (resp.magic != 0x77846772) {
            SP_LOG("Invalid magic in room manager response: %08X", resp.magic);
            return false;
        }

        // we should probably call a function that validates the received packet
        memcpy(&g_recvMatchPacket, &resp, sizeof(MatchPacket));

        return true;
    }

    return false;
}

bool sendOpenRoomRequest() {
    MatchRequestHeader openRoomRequest;
    openRoomRequest.magic = 0x4D524551; // "MREQ"
    openRoomRequest.type = MATCH_REQUEST_OPEN_ROOM;
    openRoomRequest.searchId = wfcSearchId;

    return sendToRoomManager(&openRoomRequest, sizeof(openRoomRequest));
}
