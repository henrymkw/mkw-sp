#include "MKW-Server.h"

#include <revolution/dwc/DWCMatch.h>

#include <string.h>

bool hasMKWServerAddress = false;
bool hasSentMKWServerAddressRequest = false;

void tryGetMKWServerAddress() {
    if (hasMKWServerAddress) {
        return;
    }

    SOSockAddrIn addr;
    addr.len = sizeof(SOSockAddrIn);
    // atm too lazy to dns resolve the address
    addr.family = 2;
    addr.port = 27900;
#ifdef LOCAL_MKW_SERVER
    addr.addr.addr = 0x7F000001;
#else
    addr.addr.addr = 0x607e6b90;
#endif

    // [0] = magic 0xd
    // [1] = 0x2 (join friend room)
    const char buf[2] = {0xc, 0x2};

    s32 result = SOSendTo(s_dwcMatch->qrec->hbsock, buf, sizeof(buf), 0, (void *)&addr);
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
    packet[0] = 0xb; // magic
    packet[3] = aid;
}

bool trySendRACEPacketToMKWServer(const void *data, u32 size) {
    if (!hasMKWServerAddress) {
        return false;
    }
    return SOSendTo(s_dwcMatch->qrec->hbsock, data, size, 0, (void *)&mkwServerAddr);
}