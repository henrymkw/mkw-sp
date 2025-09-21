#pragma once

#include <Common.h>

#include <gamespy/gt2/gt2.h>
#include <gamespy/qr2/qr2Main.h>

typedef struct {
    GT2Connection connection;
    GT2Socket socket;
    u8 _008[0x010 - 0x008];
    qr2 qrec;
    u8 _6d0[0x6d0 - 0x014];
    u16 qrtPort;
    u8 _6d2[0x6d4 - 0x6d2];
    u32 qr2Ip;
    u8 _6d8[0x71c - 0x6d8];
    u32 matchState;
    u8 _720[0x8c0 - 0x720];
} DWCMatch;
static_assert(sizeof(DWCMatch) == 0x8c0);

extern DWCMatch *s_dwcMatch;
