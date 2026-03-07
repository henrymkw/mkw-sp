#pragma once

#include <Common.h>

#include <sp/net/mkw_server/packets/MatchRequestHeader.h>

typedef struct {
    MatchRequestHeader header;
    u32 friendProfileId;
    u8 _14[0x18 - 0x14]; // padding
} JoinFroomRequestPacket;
static_assert(sizeof(JoinFroomRequestPacket) == 0x18);
