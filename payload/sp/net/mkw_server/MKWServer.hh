#pragma once

#include <Common.hh>
extern "C" {
#include <revolution.h>
#include <sp/net/mkw_server/MKWServerTypes.h>
}

#include <sp/net/mkw_server/packets/SearchId.hh>
namespace MKWServer {
extern bool hasMKWServerAddress;
extern bool hasSentMKWServerAddressRequest;
extern SOSockAddrIn g_mkwServerAddr;

extern u64 g_wfcSearchId;

void setMKWServerAddress(u32 addr, u16 port);

void resetMKWServerInfo();

bool applyMKWServerHeader(void *packet, u8 myAid);

EXTERN_C bool trySendRacePacketToMKWServer(const void *data, u32 size);

EXTERN_C bool verifySearchIdMagic(const u8 *packet, u32 size);

EXTERN_C bool handleSearchIdPacket(const u8 *packet, u32 size);

bool sendMessageToQR2(const u8 *data, u32 size);
} // namespace MKWServer