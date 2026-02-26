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

    s32 connectResult = connect(s_socket, &s_serverAddr, sizeof(s_serverAddr));

    if (connectResult != 0) {
        SP_LOG("Failed to connect to room manager server. connectResult: %d", connectResult);
        return false;
    }

    return true;
}

void dummyCallback() {
    SP_LOG("Dummy callback called!");
}
