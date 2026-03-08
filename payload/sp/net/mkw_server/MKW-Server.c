#include "MKW-Server.h"

#include <revolution/dwc/DWCMatch.h>

#include <sp/net/WiiLink.h>
#include <sp/net/mkw_server/packets/MatchMakingInfo.h>

#include <string.h>

#define SEARCH_ID_MAGIC "SEARCHID"

bool hasMKWServerAddress = false;
bool hasSentMKWServerAddressRequest = false;
SOSockAddrIn g_mkwServerAddr = {};
u64 wfcSearchId = 0;

void setMKWServerAddress(u32 addr, u16 port) {
    g_mkwServerAddr.addr.addr = addr;
    g_mkwServerAddr.port = port;

    g_mkwServerAddr.len = sizeof(SOSockAddrIn);
    g_mkwServerAddr.family = 2;

    hasMKWServerAddress = true;
    SP_LOG("Set MKW Server Address: %08x:%u", g_mkwServerAddr.addr.addr,
            SOHtoNs(g_mkwServerAddr.port));
}

void resetMKWServerInfo() {
    hasMKWServerAddress = false;
    hasSentMKWServerAddressRequest = false;
    memset(&g_mkwServerAddr, 0, sizeof(SOSockAddrIn));
}

void applyMKWServerHeader(u8 *packet, u8 aid) {
    packet[0] = MKW_SERVER_RACE_PACKET; // magic
    packet[3] = aid;
}

bool trySendRacePacketToMKWServer(const void *data, u32 size) {
    if (!hasMKWServerAddress) {
        return false;
    }

    bool result = SOSendTo(s_dwcMatch->qrec->hbsock, data, size, 0, (void *)&g_mkwServerAddr);
    if (!result) {
        SP_LOG("Failed to send to MKW Server!");
    }
    return result;
}

bool verifySearchIdMagic(const char *packet, u32 size) {
    if (size < 8) {
        return false;
    }
    return strncmp(packet, SEARCH_ID_MAGIC, 8) == 0;
}

bool handleSearchIdPacket(const u8 *packet, u32 size) {
    if (size != sizeof(SearchIdPacket)) {
        SP_LOG("Invalid SearchId Packet Size: %d", size);
        return false;
    }

    SearchIdPacket *searchIdPacket = (SearchIdPacket *)packet;
    if (strncmp(searchIdPacket->magic, SEARCH_ID_MAGIC, 8) != 0) {
        SP_LOG("Invalid SearchId Packet Magic: %.8s", searchIdPacket->magic);
        return false;
    }

    wfcSearchId = searchIdPacket->wfcSearchId;
    SP_LOG("Received Search Id: %llu, sending back the packet", wfcSearchId);

    // send back the same packet to confirm receipt, we'll hear back if there are issues
    bool result = sendMessageToQR2(packet, size);
    if (!result) {
        SP_LOG("Failed to send Search Id response to MKW Server!");
        return false;
    }

    return true;
}

bool sendMessageToQR2(const u8 *data, u32 size) {
    SOSockAddrIn qr2Addr;
    qr2Addr.len = sizeof(SOSockAddrIn);
    qr2Addr.family = 2;
    qr2Addr.port = 27900;
    qr2Addr.addr.addr = getWFCServerAddress();

    s32 result = SOSendTo(s_dwcMatch->qrec->hbsock, data, size, 0, (void *)&qr2Addr);
    if (result < 0) {
        SP_LOG("Failed to send message to QR2!");
        return false;
    }

    return true;
}
