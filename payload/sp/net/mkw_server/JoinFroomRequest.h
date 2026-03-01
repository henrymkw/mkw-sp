#pragma once

#include <Common.h>

#include <sp/net/mkw_server/MatchRequestHeader.h>

typedef struct {
    MatchRequestHeader header;
    u32 friendProfileId;
    u8 _14[0x18 - 0x14]; // padding
} JoinFroomRequest;
static_assert(sizeof(JoinFroomRequest) == 0x18);
