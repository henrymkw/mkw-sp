#include "MKWServerInfo.hh"

#include <revolution.h>
#include <revolution/so/so.h>

#include <sp/net/mkw_server/MKW-Server.hh>
#include <sp/net/mkw_server/RoomManager.hh>

#include <string.h>

void processMKWServerInfoPacket(u8 *data) {
    MKWServerInfoPacket resp;
    memcpy(&resp, data, sizeof(MKWServerInfoPacket));

    setMKWServerAddress(resp.address, resp.port);
}
