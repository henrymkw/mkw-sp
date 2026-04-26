#include "MKWServerInfo.h"

#include <revolution.h>
#include <revolution/so/so.h>

#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/RoomManager.h>

#include <string.h>

void processMKWServerInfoPacket(u8 *data) {
    MKWServerInfoPacket resp;
    memcpy(&resp, data, sizeof(MKWServerInfoPacket));

    setMKWServerAddress(resp.address, resp.port);
}
