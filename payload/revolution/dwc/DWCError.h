#pragma once

#include <Common.h>

#include <revolution.h>

typedef enum {
    DWC_ERROR_NONE = 0x0,
} DWCError;

typedef enum {
    DWC_ERROR_TYPE_NONE = 0x0,
} DWCErrorType;

// 0x800ccb4c
u32 DWC_GetLastError(s32 *errorCode);

// 0x800ccb64
u32 DWC_GetLastErrorEx(s32 *errorCode, DWCErrorType *errorType);

// 0x800ccc38
void DWC_ClearError();

// 0x800ccc54
u32 DWCi_IsError();

// 0x800ccc68
void DWCi_SetError(DWCError error, s32 errorCode);
