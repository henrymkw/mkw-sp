#pragma once

#include <Common.h>

#include <sp/net/mkw_server/packets/MatchRequestHeader.h>

typedef struct {
    MatchRequestHeader header;
    u32 friendProfileId;
    SearchRegion searchRegion : 8;
    u8 _15[0x18 - 0x15]; // padding
} JoinFriendRequestPacket;
static_assert(sizeof(JoinFriendRequestPacket) == 0x18);
