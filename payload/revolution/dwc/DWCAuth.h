#pragma once

#include <Common.h>

#include <revolutionex/nhttp.h>

void REPLACED(
        DWCi_Auth_SendRequest)(u32 authStage, wchar_t *miiName, char *gameId, u32 r6, u64 userId);

REPLACE void DWCi_Auth_SendRequest(u32 authStage, wchar_t *miiName, char *gameId, u32 r6,
        u64 userId);

void setAuthError(s32 error);

typedef struct {
    u8 _0000[0x59e0 - 0x0000];
    int handle;
    u8 _59e4[0x5b40 - 0x59e4];
} DWCAuthControl;
static_assert(sizeof(DWCAuthControl) == 0x5b40);

extern s32 s_auth_error;
extern DWCAuthControl *s_DWCAuthControl;
