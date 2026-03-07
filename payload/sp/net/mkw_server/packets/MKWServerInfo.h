#pragma once

#include <Common.h>

#define MKW_SERVER_INFO 0x77758783

typedef struct {
    u32 address;
    u16 port;
    u8 _6[0x8 - 0x6];
} MKWServerInfoPacket;
static_assert(sizeof(MKWServerInfoPacket) == 0x8);

bool recvMKWServerInfoPacket();
