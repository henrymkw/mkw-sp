#pragma once

#include <Common.h>

#include <sp/net/mkw_server/packets/MatchRequestHeader.h>

typedef struct {
    MatchRequestHeader header;
    SearchRegion region : 8;
    GameMode gameMode : 8;
    u8 _12[0x18 - 0x12];
} SearchRoomRequestPacket;
static_assert(sizeof(SearchRoomRequestPacket) == 0x18);
