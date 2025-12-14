#include "so.h"

#include <revolution.h>
#include <stdio.h>

#include <revolution/dwc/DWCTransport.h>

#ifdef LOCAL_MKW_SERVER
#define SERVER_IP 0x7F000001 // localhost
#else
#define SERVER_IP 0x607e6b90 // mkw-server.xyz
#endif

int SOSendTo(int s, const char *buf, int len, int flags, void *sockTo) {
    // check if its a race packet by checking the magic
    if (buf[0] == 0x0B) {
        // atm, just hardcode for testing. needs to change later
        if (hasMKWServerAddress) {
            ((SOSockAddrIn *)sockTo)->addr.addr = mkwServerIp;
            ((SOSockAddrIn *)sockTo)->port = mkwServerPort;
        } else {
            ((SOSockAddrIn *)sockTo)->addr.addr = SERVER_IP;
            ((SOSockAddrIn *)sockTo)->port = 27900;
        }
    }

    return REPLACED(SOSendTo)(s, buf, len, flags, sockTo);
}
