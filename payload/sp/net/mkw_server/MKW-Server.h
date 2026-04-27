#pragma once

#include <Common.h>

#include <sp/net/mkw_server/MKWServerTypes.h>

EXTERN_C bool verifySearchIdMagic(const u8 *packet, u32 size);

EXTERN_C bool handleSearchIdPacket(const u8 *packet, u32 size);