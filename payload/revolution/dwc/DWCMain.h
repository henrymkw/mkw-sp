#pragma once

#include <Common.h>
#include <revolution.h>

#include <revolution/dwc/DWCMain.h>
#include <revolution/dwc/DWCNode.h>

typedef enum {
    DWC_STATE_INIT = 0,
    DWC_STATE_LOGIN = 0x2,
    DWC_STATE_ONLINE = 0x3,
    DWC_STATE_MATCHING = 0x5,
    DWC_STATE_CONNECTED_TO_PEERS = 0x6,
} DWCState;

REPLACE GT2Connection DWCi_GetGT2ConnectionByProfileID(s32 profileId);
GT2Connection REPLACED(DWCi_GetGT2ConnectionByProfileID)(s32 profileId);

typedef struct {
    GT2Socket socket;
    GT2ConnectionCallbacks gt2Callbacks;
    s32 gt2sendBufSize;
    s32 gt2RecvBufSize;
    u8 _001c[0x0024 - 0x001c];
    DWCState state;
    DWCState prevState;
    u8 aid;
    u8 _002d[0x1438 - 0x002d];
} DWCContext;
static_assert(sizeof(DWCContext) == 0x1438);

extern DWCContext *s_dwcContext;
