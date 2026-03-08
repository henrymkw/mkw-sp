#pragma once

#include <Common.h>

#include <revolution/dwc/DWCFriend.h>
#include <revolution/dwc/DWCLogin.h>
#include <revolution/dwc/DWCNode.h>

#include <revolution/gamespy/gt2/gt2.h>
#include <revolution/gamespy/gt2/gt2Connection.h>

typedef enum {
    DWC_STATE_INIT = 0,
    DWC_STATE_LOGIN = 0x2,
    DWC_STATE_ONLINE = 0x3,
    DWC_STATE_MATCHING = 0x5,
    DWC_STATE_CONNECTED_TO_PEERS = 0x6,
} DWCState;

GT2Connection DWCi_GetGT2ConnectionByProfileID(s32 profileId);

typedef struct {
    GT2Socket socket;
    GT2ConnectionCallbacks gt2Callbacks;
    s32 gt2sendBufSize;
    s32 gt2RecvBufSize;
    u8 _001c[0x0024 - 0x001c];
    DWCState state;
    DWCState prevState;
    u8 aid;
    u8 _002d[0x0098 - 0x002d];
    DWCLoginControl loginControl;
    DWCFriendControl friendControl;
    u8 _0360[0x1438 - 0x0360];
} DWCControl;
static_assert(sizeof(DWCControl) == 0x1438);

REPLACE u32 DWC_GetAidBitmap();

REPLACE u32 DWC_GetNumConnectionsHost();

REPLACE u32 DWC_GetDirectConnectedAidBitmap();

REPLACE u32 DWC_GetGroupId();

REPLACE u8 DWC_GetMyAid();

REPLACE u8 DWC_GetServerAid();

REPLACE u8 DWC_GetSuspendMatch();

REPLACE u8 DWC_IsValidMatchCancel();

REPLACE bool DWC_GetConnectionUserData(u8 aid, DWCConnectionUserData *playerCount);

extern DWCControl *s_dwcControl;
