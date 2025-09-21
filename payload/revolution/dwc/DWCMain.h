#pragma once

#include <Common.h>

#pragma once

#include <Common.h>

#include <revolution/dwc/DWCMain.h>

#include <gamespy/gt2/gt2.h>

typedef enum {
    DWC_STATE_INIT = 0,
    DWC_STATE_LOGIN = 0x2,
    DWC_STATE_ONLINE = 0x3,
    DWC_STATE_MATCHING = 0x5,
    DWC_STATE_CONNECTED_TO_PEERS = 0x6,
} DWCState;

typedef struct {
    GT2Socket socket;
    GT2ConnectionCallbacks gt2Callbacks;
    u8 _0014[0x0024 - 0x0014];
    DWCState state;
    DWCState prevState;
    u8 aid;
    u8 _002d[0x1438 - 0x002d];
} DWCContext;
static_assert(sizeof(DWCContext) == 0x1438);

extern DWCContext *s_dwcContext;
