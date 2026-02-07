#pragma once

#include <Common.h>

#include <revolution.h>

typedef void (*UserSendCallback)(u32 size, u8 aid);
typedef void (*UserRecvCallback)(u8 aid, u8 *recvBuffer, u32 size);

typedef struct {
    u8 _00[0x38 - 0x00];
    OSTime timeoutTime;
} DWCTransportInfo;
static_assert(sizeof(DWCTransportInfo) == 0x40);

typedef struct {
    DWCTransportInfo transportInfos[32];
    UserSendCallback userSendCallback; // Unused in MKW
    UserRecvCallback userRecvCallback; // this is UserRecvCallback in vanilla
    u8 _808[0x810 - 0x808];
    u16 maxUnreliablePacketSize; // This is always 1465 (0x5B9)
    u8 _812[0x818 - 0x812];
} DWCTransport;
static_assert(sizeof(DWCTransport) == 0x818);

bool DWC_SendUnreliable(u8 aid, void *data, u32 size);

REPLACE void DWCi_TransportProcess();

REPLACE bool DWC_SetUserRecvCallback(UserRecvCallback callback);

REPLACE BOOL DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, u32 ip, u16 port, u8 *message,
        s32 len);
BOOL REPLACED(DWCi_GT2UnrecognizedMessageCallback)(GT2Socket socket, u32 ip, u16 port, u8 *message,
        s32 len);

extern DWCTransport *s_dwcTransport;
