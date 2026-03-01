#include "MKW-Server.h"

#include <revolution/dwc/DWCMatch.h>

#include <sp/net/WiiLink.h>

#include <string.h>

#define SEARCH_ID_MAGIC "SEARCHID"

bool hasMKWServerAddress = false;
bool hasSentMKWServerAddressRequest = false;
SOSockAddrIn mkwServerAddr = {};
u64 wfcSearchId = 0;

void tryGetMKWServerAddress() {
    if (hasMKWServerAddress) {
        return;
    }

    // [0] = magic 0xd
    // [1] = 0x2 (join friend room)
    const u8 buf[2] = {0xc, 0x2};

    s32 result = sendMessageToQR2(buf, sizeof(buf));
    if (result < 0) {
        SP_LOG("Failed to send to mkw-server manager!");
        return;
    }
    SP_LOG("Sent MKW Server Address Request");
    // this hacky poc is over udp, better not get lost! LOL
    // (i cant figure out how to use an existing tcp connection or make a new one...)
    hasSentMKWServerAddressRequest = true;
}

void setMKWServerAddress(const u8 *msg, u32 len) {
    if (len != 10 && strnlen((const char *)msg, 10) != 10) {
        SP_LOG("MKW Server Address Message Length Invalid: %d", len);
        return;
    }
    // message should be {0x0c, 0x1, ip[4], port[2]}
    memcpy(&mkwServerAddr.addr.addr, &msg[4], 4);
    memcpy(&mkwServerAddr.port, &msg[8], 2);

    mkwServerAddr.len = sizeof(SOSockAddrIn);
    mkwServerAddr.family = 2;

    hasMKWServerAddress = true;
    SP_LOG("Set MKW Server Address: %08x:%u", mkwServerAddr.addr.addr, SOHtoNs(mkwServerAddr.port));
}

void resetMKWServerInfo() {
    hasMKWServerAddress = false;
    hasSentMKWServerAddressRequest = false;
    memset(&mkwServerAddr, 0, sizeof(SOSockAddrIn));
}

void applyMKWServerHeader(u8 *packet, u8 aid) {
    packet[0] = MKW_SERVER_RACE_PACKET; // magic
    packet[3] = aid;
}

bool trySendRACEPacketToMKWServer(const void *data, u32 size) {
    if (!hasMKWServerAddress) {
        return false;
    }
    bool result = SOSendTo(s_dwcMatch->qrec->hbsock, data, size, 0, (void *)&mkwServerAddr);
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
