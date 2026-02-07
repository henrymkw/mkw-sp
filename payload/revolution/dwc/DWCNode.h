/*
 * Credits:
 * - CLF78: Midnight-Variety-Pack, licensed under the MPL license. See license in
 * licenses/Midnight-Variety-Pack.
 *
 */

#pragma once

#include <Common.h>

typedef struct {
    u8 playersAtConsole;
    u8 _1[0x4 - 0x1];
} DWCConnectionUserData;
static_assert(sizeof(DWCConnectionUserData) == 0x4);

typedef struct {
    int profileId;
    u32 publicip;
    u32 localIp;
    u16 publicport;
    u16 localPort;
    u32 gt2Ip;
    u16 gt2Port;

    u8 aid;
    u8 hasPrivateAddress;
    int nnTryCount;
    u8 _1c[0x20 - 0x1c];

    s64 nextMeshMakeTryTick;
    DWCConnectionUserData connectionUserData;
    u8 _2c[0x30 - 0x2c];
} DWCNodeInfo;
static_assert(sizeof(DWCNodeInfo) == 0x30);

typedef struct {
    u32 nodeCount;
    u8 _004[0x008 - 0x004];
    DWCNodeInfo nodeInfos[32];
} DWCNodeInfoList;
static_assert(sizeof(DWCNodeInfoList) == 0x608);