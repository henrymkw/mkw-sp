#pragma once

#include <Common.h>

#include <sp/net/mkw_server/packets/MatchRequestHeader.h>

/*
   For local multi-player to work with the new match making implementation, the server needs to know
   the number of local players for each aid. Players will inform the server their local player count
   after receiving a searchId. This is done at this point because, in MKW, the local player count is
   known before entering match making, which is not an assumption that can be made for all games.
   The local player count also doesn't change throughout a login session, so it only needs to be
   sent once. The only way the local player count can change is to disconnect and reconnect,
   creating a new session.
*/
typedef struct {
    MatchRequestHeader header;
    u8 localPlayerCount;
    u8 _11[0x18 - 0x11];
} LocalPlayerCountPacket;
static_assert(sizeof(LocalPlayerCountPacket) == 0x18);
