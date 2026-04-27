#pragma once

#include <Common.h>

// different race packet types, currently we check the first byte (which should be reconsidered at
// some point) for the type of packet
typedef enum {
    MKW_SERVER_RACE_PACKET = 0xB,
} MKWServerPacketType;

EXTERN_C bool verifySearchIdMagic(const u8 *packet, u32 size);

EXTERN_C bool handleSearchIdPacket(const u8 *packet, u32 size);