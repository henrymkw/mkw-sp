#pragma once

#include <Common.h>

#include <gamespy/gt2/gt2.h>

// 0x80109c1c
u32 gti2ReceivedCallback(GT2Connection connection, GT2Byte *message, s32 len, bool reliable);
