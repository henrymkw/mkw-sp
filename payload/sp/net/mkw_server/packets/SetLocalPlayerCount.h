#pragma once

#include <Common.h>

#include <sp/net/mkw_server/MatchRequestHeader.h>

typedef struct {
    MatchRequestHeader header;
    u8 playerCount;
} SetLocalPlayerCountRequest;
static_assert(sizeof(SetLocalPlayerCountRequest) == 0x18);