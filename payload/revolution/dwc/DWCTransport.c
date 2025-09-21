#include "DWCTransport.h"

#include <string.h>

#include <revolution.h>

#include <revolution/dwc/DWCError.h>
#include <revolution/dwc/DWCFriend.h>
#include <revolution/dwc/DWCMain.h>
#include <revolution/dwc/DWCMatch.h>

#include <gamespy/qr2/qr2.h>
#include <revolution/so/so.h>

#include <gamespy/gt2/gt2.h>
#include <gamespy/gt2/gt2Main.h>
#include <gamespy/gt2/gt2Socket.h>

bool hasMKWServerAddress = false;
bool hasSentMKWServerAddressRequest = false;

u32 mkwServerIp = 0;
u16 mkwServerPort = 0;

GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, u32 ip, u16 port, GT2Byte *message,
        s32 len) {
    if (message == NULL || len == 0) {
        SP_LOG("GT2 Unrecognized : Null message or zero length.");
        return GT2False;
    }

    char messageType = message[0];
    if (messageType == 0x0b) { // race packet
        u8 senderAid = message[3];

        if (s_dwcTransport == NULL) {
            return GT2False;
        }

        if (s_dwcTransport->userRecvCallback == NULL) {
            return GT2False;
        }

        // NetManager::ProcessRACEPacket() is the cb here
        s_dwcTransport->userRecvCallback(senderAid, message, len);

        return GT2True;
    } else if (messageType == 0xc) { // responce from wfc-server of mkw-server's address
        // TODO: Check message length, du
        if (len > 0xa) {
            SP_LOG("GT2 Unrecognized : MKW Server Manager message length too long: %d", len);
            return GT2False;
        }
        if (message[1] == MKW_SERVER_MSG_TYPE_SVR_ADDR) {
            // message should be {0x0c, 0x1, ip[4], port[2]}
            memcpy(&mkwServerIp, &message[4], 4);
            memcpy(&mkwServerPort, &message[8], 2);

            hasMKWServerAddress = true;
            SP_LOG("GT2 Unrecognized : Received MKW Server Address: %08x:%u", mkwServerIp,
                    SOHtoNs(mkwServerPort));
            return GT2True;
        }
    } else {
        return REPLACED(DWCi_GT2UnrecognizedMessageCallback)(socket, ip, port, message, len);
    }

    return GT2False;
}

void tryGetMKWServerAddress() {
    if (hasMKWServerAddress) {
        return;
    }

    SOSockAddrIn addr;
    addr.len = sizeof(SOSockAddrIn);
    // atm too lazy to dns resolve the address
    addr.family = 2;
    addr.port = 27900;
#ifdef PROD
    addr.addr.addr = 0x607e6b90;
#else
    addr.addr.addr = 0x7F000001;
#endif

    // [0] = magic 0xd
    // [1] = 0x2 (join friend room)
    char buf[2] = {0xc, 0x2};

    s32 result =
            SOSendTo(s_dwcMatch->qrec->hbsock, (const char *)buf, sizeof(buf), 0, (void *)&addr);
    if (result < 0) {
        SP_LOG("Failed to send to mkw-server manager!");
        return;
    }
    // this hacky poc is over udp, better not get lost! LOL
    // (i cant figure out how to use an existing tcp connection or make a new one...)
    hasSentMKWServerAddressRequest = true;
}

void DWCi_TransportProcess() {
    // not a good place for this check since this cmp gets spammed (every frame)
    if (hasSentMKWServerAddressRequest == false) {
        SP_LOG("DWCi_TransportProcess: Attempting to get MKW-Server address.");
        tryGetMKWServerAddress();
    }

    REPLACED(DWCi_TransportProcess)();
}

void DWCi_ShutdownTransport() {
    SP_LOG("DWCi_ShutdownTransport: Clearing MKW Server address info.");
    hasMKWServerAddress = false;
    hasSentMKWServerAddressRequest = false;
    mkwServerIp = 0;
    mkwServerPort = 0;

    REPLACED(DWCi_ShutdownTransport)();
}
