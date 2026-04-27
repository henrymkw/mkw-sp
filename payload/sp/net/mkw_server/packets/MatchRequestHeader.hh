#pragma once

#include <Common.hh>

#include <sp/net/mkw_server/RoomManager.hh>

#define MATCH_REQUEST_HEADER_MAGIC 0x4D524551 // "MREQ"
namespace MKWServer {
struct MatchRequestHeader {
    u32 magic;
    MatchRequestType type;
    u8 _5[0x07 - 0x05];
    u64 searchId;
};
static_assert(sizeof(MatchRequestHeader) == 0x10);

void createMatchRequestHeader(MatchRequestHeader *header, MatchRequestType type, u64 searchId);
} // namespace MKWServer