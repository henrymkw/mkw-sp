#pragma once

#include <Common.hh>

extern "C" {
#include <sp/net/mkw_server/MKWServerTypes.h>
}

namespace MKWServer {
#define MATCH_MAKING_INFO 0x77846772

extern MatchMakingInfoPacket g_recvMatchPacket;

bool matchMakingInfoValid(MatchMakingInfoPacket *mmInfo);

bool processMatchMakingInfoPacket(u8 *data);

void resetMatchMakingInfoPacket();
} // namespace MKWServer
