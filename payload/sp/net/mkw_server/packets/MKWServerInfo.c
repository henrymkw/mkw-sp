#include "MKWServerInfo.h"

#include <revolution.h>
#include <revolution/so/so.h>

#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/MatchMaking.h>

#include <string.h>

bool recvMKWServerInfoPacket() {
    MKWServerInfoPacket resp;
    s32 recvResult = SORecv(g_matchMakingSocket, (void *)&resp, sizeof(MKWServerInfoPacket), 0);

    if (recvResult > 0) {
        // we should probably call a function that validates the received packet
        u32 address;
        u16 port;

        memcpy(&address, &resp, sizeof(u32));
        memcpy(&port, &(resp.port), sizeof(u16));

        setMKWServerAddress(address, port);

        SP_LOG("Received MKW-Server Address: %x, %d", g_mkwServerAddr.addr.addr,
                g_mkwServerAddr.port);
        return true;
    }

    return false;
}
