#include "so.h"

#include <Common.h>

#include <revolution/dwc/DWCTransport.h>
#include <sp/net/MKW-Server.h>

int SOSendTo(int s, const char *buf, int len, int flags, SOSockAddrIn *sockTo) {
    if (hasMKWServerAddress && buf[0] == MKW_SERVER_RACE_PACKET) {
        sockTo->addr.addr = mkwServerAddr.addr.addr;
        sockTo->port = mkwServerAddr.port;
    }
    return REPLACED(SOSendTo)(s, buf, len, flags, sockTo);
}