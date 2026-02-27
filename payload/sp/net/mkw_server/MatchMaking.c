#include "MatchMaking.h"

#include <string.h>

#include <revolution/so/so.h>

static SOSockAddrIn s_serverAddr;
static SOCKET s_socket = -1;

bool connectToRoomManager() {
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
    s32 connectResult = SOConnect(s_socket, &s_serverAddr);

    if (connectResult != 0) {
        SP_LOG("Failed to connect to room manager server. connectResult: %d", connectResult);
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
    char buffer[256];

    s32 recvResult = SORecv(s_socket, buffer, sizeof(buffer), 0);

    if (recvResult > 0) {
        SP_LOG("Received message from room manager: %.*s", recvResult, buffer);
        return true;
    }

    return false;
}
