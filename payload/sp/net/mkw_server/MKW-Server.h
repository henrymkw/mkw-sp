#pragma once

#include <Common.h>
#include <revolution.h>

#include <sp/net/mkw_server/packets/SearchId.h>

extern bool hasMKWServerAddress;
extern bool hasSentMKWServerAddressRequest;
extern SOSockAddrIn g_mkwServerAddr;

extern u64 wfcSearchId;

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

void setMKWServerAddress(u32 addr, u16 port);

void resetMKWServerInfo();

bool applyMKWServerHeader(void *packet, u8 aid);

bool trySendRacePacketToMKWServer(const void *data, u32 size);

bool verifySearchIdMagic(const char *packet, u32 size);

bool handleSearchIdPacket(const u8 *packet, u32 size);

bool sendMessageToQR2(const u8 *data, u32 size);
