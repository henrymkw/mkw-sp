#include "so.h"

#include <Common.h>

#include <revolution/dwc/DWCTransport.h>
#include <sp/net/mkw_server/MKW-Server.h>

int SOSendTo(int s, const u8 *buf, int len, int flags, SOSockAddrIn *sockTo) {
    /*
    if (hasMKWServerAddress && buf[0] == MKW_SERVER_RACE_PACKET) {
        sockTo->addr.addr = mkwServerAddr.addr.addr;
        sockTo->port = mkwServerAddr.port;
    }
    */
    if (sockTo->port == 27900) {
        SP_LOG("Sending to QR2");
    }
    return REPLACED(SOSendTo)(s, buf, len, flags, sockTo);
}
