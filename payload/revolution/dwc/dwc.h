#pragma once

#include <Common.h>

PRAGMA("pack(push, 4)")
typedef struct DWCUserData {
    u32 dataSize; // always 0x40, size of this struct
    u64 pseudoUserID;
    u32 pseudoPlayerID;
    u64 authenticUserID;
    u32 authenticPlayerID;
    u32 profileID;
    u8 _20[0x24 - 0x20]; // unknown flags
    char gameID[4];      // Always RMCJ
    u8 _68[0x7c - 0x68]; // unknown
    u32 crc;             // crc-32 of this struct
} DWCUserData;
PRAGMA("pack(pop)")
static_assert(sizeof(DWCUserData) == 0x40);

// Needs further investigation/documentation
typedef struct DWCLoginContext {
    u8 _000[0x01c - 0x000];
    DWCUserData *userData;
    u64 userIdMaybe;
    u8 _028[0x268 - 0x028];
} DWCLoginContext;
static_assert(sizeof(DWCLoginContext) == 0x268);

void DWCi_Auth_SendRequest(int p1, wchar_t *p2, char *p3, int p4, int p5, int p6);

u64 DWCi_Acc_GetUserId(void *unk);

u32 DWCi_Acc_GetPlayerId(void *unk);

void DWCi_Acc_SetPlayerId(void *unk, u32 id);

void DWCi_Acc_SetUserId(void *unk, u64 id);

// DWCLoginContext + 0x1c is passed in,
bool DWCi_Acc_IsAuthentic(u32 unk);

bool DWCi_Acc_IsValidLoginId(void *unk);

bool DWC_CheckUserData(DWCUserData *unk);

extern DWCLoginContext *s_DWCLoginContext;
