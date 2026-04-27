#pragma once

#include <Common.hh>

#include <sp/net/mkw_server/packets/MatchRequestHeader.hh>
namespace MKWServer {
struct SearchRoomRequestPacket {
    MatchRequestHeader header;
    SearchRegion region : 8;
    GameMode gameMode : 8;
    u8 _12[0x18 - 0x12];
};
static_assert(sizeof(SearchRoomRequestPacket) == 0x18);
} // namespace MKWServer