#include "MKWServer.hh"

extern "C" {
#include <revolution.h>
#include <revolution/dwc/DWCMatch.h>
}

#include <sp/net/WiiLink.hh>
#include <sp/net/mkw_server/RoomManager.hh>
#include <sp/net/mkw_server/packets/MatchMakingInfo.hh>

#include <string.h>

namespace MKWServer {
#define SEARCH_ID_MAGIC "SEARCHID"

static bool s_hasRequestedMKWServerAddress = false;
static SOSockAddrIn s_mkwServerAddr = {};
static u64 s_wfcSearchId = 0;

void setMKWServerAddress(u32 addr, u16 port) {
    s_mkwServerAddr.addr.addr = addr;
    s_mkwServerAddr.port = port;

    s_mkwServerAddr.len = sizeof(SOSockAddrIn);
    s_mkwServerAddr.family = 2;

    SP_LOG("Set MKWServer Address: %08x:%u", s_mkwServerAddr.addr.addr,
            SOHtoNs(s_mkwServerAddr.port));
}

void resetMKWServerInfo() {
    s_hasRequestedMKWServerAddress = false;
    memset(&s_mkwServerAddr, 0, sizeof(SOSockAddrIn));
}

bool applyMKWServerHeader(void *packet, u8 myAid) {
    if (hasMKWServerAddress()) {
        u8 *data = (u8 *)packet;
        data[0] = MKW_SERVER_RACE_PACKET; // magic
        data[1] = myAid;
        return true;
    }
    SP_LOG("Failed to apply MKW-Server header! Don't have MKW-Server Address");
    return false;
}

u64 getSearchId() {
    return s_wfcSearchId;
}

bool hasMKWServerAddress() {
    return s_mkwServerAddr.addr.addr != 0 && s_mkwServerAddr.port != 0;
}

bool trySendRacePacketToMKWServer(const void *data, u32 size) {
    if (!hasMKWServerAddress()) {
        return false;
    }

    bool result = SOSendTo(s_dwcMatch->qrec->hbsock, data, size, 0, &s_mkwServerAddr);
    if (!result) {
        SP_LOG("Failed to send to MKW Server!");
    }
    return result;
}

bool verifySearchIdMagic(const u8 *packet, u32 size) {
    if (size < 8) {
        return false;
    }
    return strncmp((const char *)packet, SEARCH_ID_MAGIC, 8) == 0;
}

bool handleSearchIdPacket(const u8 *packet, u32 size) {
    SearchIdPacket *searchIdPacket = (SearchIdPacket *)packet;

    s_wfcSearchId = searchIdPacket->wfcSearchId;
    SP_LOG("Received Search Id: %llu, sending back the packet", s_wfcSearchId);

    // send back the same packet to confirm receipt, we'll hear back if there are issues
    bool result = sendMessageToQR2(packet, size);
    if (!result) {
        SP_LOG("Failed to send Search Id response to MKW Server!");
        return false;
    }

    // Establish connection with the room manager server early in session's existance
    if (!connectToRoomManager()) {
        SP_LOG("Unable to connect to room manager upon sending searchId!");
        return false;
    }

    return true;
}

bool sendMessageToQR2(const u8 *data, u32 size) {
    SOSockAddrIn qr2Addr;
    qr2Addr.len = sizeof(SOSockAddrIn);
    qr2Addr.family = 2;
    qr2Addr.port = 27900;
    qr2Addr.addr.addr = WFC_SERVER_ADDRESS;

    s32 result = SOSendTo(s_dwcMatch->qrec->hbsock, data, size, 0, &qr2Addr);
    if (result < 0) {
        SP_LOG("Failed to send message to QR2!");
        return false;
    }

    return true;
}
} // namespace MKWServer
