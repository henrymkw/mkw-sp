#pragma once

#include <Common.h>

#include <revolution.h>

extern bool hasMKWServerAddress;
extern bool hasSentMKWServerAddressRequest;

static SOSockAddrIn mkwServerAddr;

void tryGetMKWServerAddress();
void setMKWServerAddress(const u8 *msg, u32 len);
void resetMKWServerInfo();

void applyMKWServerHeader(u8 *packet, u8 aid);

bool trySendRACEPacketToMKWServer(const void *data, u32 size);
