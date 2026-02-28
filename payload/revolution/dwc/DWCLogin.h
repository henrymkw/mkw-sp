#pragma once

#include <Common.h>

typedef struct {
    u8 _0[0x4 - 0x0];
    u32 userId;
    u32 playerId;
} DWCAccLoginId;
static_assert(sizeof(DWCAccLoginId) == 0xc);

typedef struct {
    u32 size;
    DWCAccLoginId pseudoID;
    DWCAccLoginId authenticID;
    u32 gsProfileId;
    u8 _20[0x40 - 0x20];
} DWCAccUserData;
static_assert(sizeof(DWCAccUserData) == 0x40);

typedef struct {
    u8 _000[0x01c - 0x000];
    DWCAccUserData *userData;
    u8 _020[0x268 - 0x020];
} DWCLoginControl;
static_assert(sizeof(DWCLoginControl) == 0x268);

u64 DWCi_Acc_GetUserId(DWCAccLoginId *loginId);