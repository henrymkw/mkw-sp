/*  Copyright (c) 2011, IGN Entertainment, Inc. ("IGN")
    All rights reserved.
*/

#pragma once

#include <Common.h>
#include <revolution/gamespy/qr2/qr2Main.h>

#define QR_MAGIC_1 0xFE
#define QR_MAGIC_2 0xFD

typedef struct QR2Implementation *qr2;

// gets called when leaving a froom, used to reset mkw-server related globals
REPLACE void qr2_shutdown(qr2 qr2);
void REPLACED(qr2_shutdown)(qr2 qr2);
