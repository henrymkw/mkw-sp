#pragma once

#include <Common.hh>

#include <sp/net/mkw_server/packets/MatchRequestHeader.hh>
namespace MKWServer {
struct SuspendRequestPacket {
    MatchRequestHeader header;
    bool suspendVote; // true to suspend match, false to unsuspend
    u8 _11[0x18 - 0x11];
};
static_assert(sizeof(SuspendRequestPacket) == 0x18);
} // namespace MKWServer