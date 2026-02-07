#pragma once

#include <Common.h>
#include <revolution.h>

typedef void (*GT2ConnectedCallback)(GT2Connection conn, GT2Result, const char *msg, int msgLen);
typedef void (*GT2ReceivedCallback)(GT2Connection conn, u8 *msg, int msgLen, int isReliable);
typedef void (*GT2ClosedCallback)(GT2Connection conn, GT2CloseReason reason);
typedef void (*GT2PingCallback)(GT2Connection conn, int latency);

typedef struct {
    GT2ConnectedCallback connectedCallback;
    GT2ReceivedCallback receivedCallback;
    GT2ClosedCallback closedCallback;
    GT2PingCallback pingCallback;
} GT2Callbacks;

// 0x80109c1c
u32 gti2ReceivedCallback(GT2Connection connection, GT2Byte *message, s32 len, bool reliable);
