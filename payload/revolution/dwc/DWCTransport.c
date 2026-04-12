#include "DWCTransport.h"

#include <sp/net/mkw_server/MKW-Server.h>

BOOL DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, u32 ip, u16 port, const u8 *message,
        s32 len) {
    if (message == NULL || len == 0) {
        SP_LOG("GT2 Unrecognized : Null message or zero length.");
        return GT2False;
    }

    if (verifySearchIdMagic((const char *)message, len)) {
        handleSearchIdPacket(message, len);
        return GT2True;
    }

    MKWServerPacketType messageType = message[0];
    switch (messageType) {
    case MKW_SERVER_RACE_PACKET:
        if (s_dwcTransport == NULL || s_dwcTransport->userRecvCallback == NULL) {
            return FALSE;
        }

        // the aid is in offset 0x1
        u8 aid = message[1];

        s_dwcTransport->userRecvCallback(aid, message, len);
        return TRUE;
    case MKW_SERVER_COMBINED_RACE_PACKET:
        if (s_dwcTransport == NULL || s_dwcTransport->userRecvCallback == NULL) {
            return FALSE;
        }

        // we don't care about the aid since the callback knows how to parse it from the packet
        s_dwcTransport->userRecvCallback(0xff, message, len);
        return TRUE;

    default:
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
