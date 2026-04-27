#pragma once

#include <Common.hh>

#include <sp/net/mkw_server/MatchRequestHeader.hh>

typedef struct {
    MatchRequestHeader header;
    u8 playerCount;
} SetLocalPlayerCountRequest;
static_assert(sizeof(SetLocalPlayerCountRequest) == 0x18);