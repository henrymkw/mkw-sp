#pragma once

#include <Common.h>

#include <gamespy/gt2/gt2.h>

// 0x8010a464
void gti2RejectConnection(GT2Connection connection, const GT2Byte *message, s32 len);

// 0x8010a3c0
u32 gti2AcceptConnection(GT2Connection connection, GT2ConnectionCallbacks *callbacks);
