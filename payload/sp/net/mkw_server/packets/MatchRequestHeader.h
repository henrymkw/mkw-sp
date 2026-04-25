#pragma once

#include <Common.h>

#include <sp/net/mkw_server/RoomManager.h>

#define MATCH_REQUEST_HEADER_MAGIC 0x4D524551 // "MREQ"

typedef struct {
    u32 magic;
    MatchRequestType type : 8;
    u8 _5[0x07 - 0x05];
    u64 searchId;
} MatchRequestHeader;
static_assert(sizeof(MatchRequestHeader) == 0x10);

void createMatchRequestHeader(MatchRequestHeader *header, MatchRequestType type, u64 searchId);
