#pragma once

#include <Common.h>

#define MKW_SERVER_INFO 0x77758783

typedef struct {
    u32 address;
    u16 port;
} MKWServerInfoPacket;
static_assert(sizeof(MKWServerInfoPacket) == 0x8);
