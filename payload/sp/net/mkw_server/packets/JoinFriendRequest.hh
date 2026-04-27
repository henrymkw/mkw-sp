#pragma once

#include <Common.hh>

#include <sp/net/mkw_server/packets/MatchRequestHeader.hh>

typedef struct {
    MatchRequestHeader header;
    u32 friendProfileId;
    SearchRegion searchRegion : 8;
    u8 _15[0x18 - 0x15]; // padding
} JoinFriendRequestPacket;
static_assert(sizeof(JoinFriendRequestPacket) == 0x18);
