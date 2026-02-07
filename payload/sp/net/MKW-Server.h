#pragma once

#include <Common.h>

#include <revolution.h>

extern bool hasMKWServerAddress;
extern bool hasSentMKWServerAddressRequest;

static SOSockAddrIn mkwServerAddr;

// different race packet types, currently we check the first byte (which should be reconsidered at
// some point) for the type of packet
typedef enum {
    MKW_SERVER_RACE_PACKET = 0xB,
    MKW_SERVER_ADDR_RESP = 0xC,
    MKW_SERVER_COMBINED_RACE_PACKET = 0xD,
} MKWServerPacketType;

// this enum's future is tbd, intent was to distinguish between
// different messages (other than race packets) sent by mkw-server (i.e, item decision responses)
typedef enum {
    MKW_SERVER_MSG_TYPE_SVR_ADDR = 0x1,
} MKWServerMessageType;

void tryGetMKWServerAddress();
void setMKWServerAddress(const u8 *msg, u32 len);
void resetMKWServerInfo();

void applyMKWServerHeader(u8 *packet, u8 aid);

bool trySendRACEPacketToMKWServer(const void *data, u32 size);
