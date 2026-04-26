#pragma once

#include <Common.h>

#define MKW_SERVER_INFO 0x4D4B5753 // "MKWS"

// Pack since wfc-server sends exactly 6 bytes
#pragma pack(push, 1)
typedef struct {
    u32 address;
    u16 port;
} MKWServerInfoPacket;
#pragma pack(pop)
static_assert(sizeof(MKWServerInfoPacket) == 0x6);

void processMKWServerInfoPacket(u8 *data);
