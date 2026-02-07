#include "DWCTransport.h"

#include <string.h>

#include <revolution.h>

#include <revolution/dwc/DWCError.h>
#include <revolution/dwc/DWCFriend.h>
#include <revolution/dwc/DWCMain.h>
#include <revolution/dwc/DWCMatch.h>

#include <sp/net/MKW-Server.h>

void DWCi_TransportProcess() {
    // do nothing
}

BOOL DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, u32 ip, u16 port, u8 *message, s32 len) {
    if (message == NULL || len == 0) {
        SP_LOG("GT2 Unrecognized : Null message or zero length.");
        return GT2False;
    }

    char messageType = message[0];
    if (messageType == 0xd) { // combined race packet

        if (s_dwcTransport == NULL) {
            return FALSE;
        }

        if (s_dwcTransport->userRecvCallback == NULL) {
            return FALSE;
        }

        // NetManager::ProcessRACEPacket() is the cb here

        // the callback will need to be careful when the number of packets is 1
        s_dwcTransport->userRecvCallback(0xff, message, len);

        return TRUE;
    } else if (messageType == 0xc) { // responce from wfc-server of mkw-server's address
        // TODO: Check message length, du
        if (len > 0xa) {
            SP_LOG("GT2 Unrecognized : MKW Server Manager message length too long: %d", len);
            return FALSE;
        }
        if (message[1] == MKW_SERVER_MSG_TYPE_SVR_ADDR) {
            setMKWServerAddress((u8 *)message, len);
            return TRUE;
        }
    } else {
        return REPLACED(DWCi_GT2UnrecognizedMessageCallback)(socket, ip, port, message, len);
    }

    return FALSE;
}

bool DWC_SetUserRecvCallback(UserRecvCallback callback) {
    if (s_dwcTransport == NULL) {
        return false;
    }

    s_dwcTransport->userRecvCallback = callback;
    return true;
}
