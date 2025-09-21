#pragma once

#include <Common.h>

#include <gamespy/gt2/gt2.h>

extern bool hasMKWServerAddress;
extern bool hasSentMKWServerAddressRequest;

extern u32 mkwServerIp;
extern u16 mkwServerPort;

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
    UserRecvCallback userRecvCallback; // calls NetManager::processRacePacket()
    u8 _808[0x810 - 0x808];
    u16 maxUnreliablePacketSize; // This is always 1465 (0x5B9)
    u8 _812[0x818 - 0x812];
} DWCTransport;
static_assert(sizeof(DWCTransport) == 0x818);

// added
typedef enum {
    MKW_SERVER_MSG_TYPE_SVR_ADDR = 0x1,
} MKWServerMessageType;

REPLACE GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket, u32 ip, u16 port,
        GT2Byte *message, s32 len);
GT2Bool REPLACED(DWCi_GT2UnrecognizedMessageCallback)(GT2Socket socket, u32 ip, u16 port,
        GT2Byte *message, s32 len);

void tryGetMKWServerAddress();

REPLACE void DWCi_TransportProcess();
void REPLACED(DWCi_TransportProcess)();

REPLACE void DWCi_ShutdownTransport();
void REPLACED(DWCi_ShutdownTransport)();

extern DWCTransport *s_dwcTransport;
