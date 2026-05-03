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

// See MatchMaking.md for bellow replacements

// 0x800d2158
// Called by NetManager::mainNetworkLoop() to get DWC to clean up a few things.
// Patch may not be needed but added to be safe.
REPLACE u32 DWC_GetNumConnectionsHost();

// 0x800d4994
// Called by NetManager::updateStatusData()
REPLACE u32 DWC_GetGroupId();

// 0x800d49b4
// Called by NetManager::mainNetworkLoop() to check match suspension before voting MM suspension
REPLACE u8 DWC_GetSuspendMatch();

// 0x800d41f4
// Also called by NetManager::mainNetworkLoop() to perform some clean up.
// Added to be safe.
REPLACE u8 DWC_IsValidMatchCancel();

// 0x800d49b0
// Patched out to send suspension request to MKW-Server
REPLACE bool DWC_RequestSuspendMatchAsync(bool suspendVote);

extern DWCControl *s_dwcControl;
