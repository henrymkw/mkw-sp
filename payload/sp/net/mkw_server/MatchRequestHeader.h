#pragma once

#include <Common.h>

#include <sp/net/mkw_server/MatchMaking.h>

typedef struct {
    u32 magic; // "MREQ"
    u8 type;
    u8 _5[0x07 - 0x05];
    u64 searchId;
} MatchRequestHeader;
static_assert(sizeof(MatchRequestHeader) == 0x10);
