#include "DWCTransport.h"

#include <sp/net/mkw_server/MKWServer.h>

BOOL DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, u32 ip, u16 port, const u8 *message,
        s32 len) {
    if (message == NULL || len == 0) {
        SP_LOG("GT2 Unrecognized : Null message or zero length.");
        return GT2False;
    }

    if (verifySearchIdMagic(message, len)) {
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

    default:
        return REPLACED(DWCi_GT2UnrecognizedMessageCallback)(socket, ip, port, message, len);
    }
    return FALSE;
}
