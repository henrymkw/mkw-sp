#pragma once

#include <Common.h>
#include <revolution.h>

typedef void (*NegotiateProgressFunc)(int state, void *userData);
typedef void (*NegotiateCompletedFunc)(int result, int gamesocket, SOSockAddrIn *remoteAddr,
        void *userData);
typedef void (*NatDetectionResultsFunc)(BOOL success, NAT *nat);

typedef struct {
    int negotiateSock;
    int gameSock;
    int cookie;
    int clientindex;
    int state;
    int initAckRecv[4];
    int retryCount;
    int maxRetryCount;
    u32 retryTime;
    u32 guessedIP;
    u16 guessedPort;
    u8 gotRemoteData;
    u8 sendGotRemoteData;
    NegotiateProgressFunc progressCallback;
    NegotiateCompletedFunc completedCallback;
    void *userdata;
    int result;
    int connectedSocket;
    SOSockAddrIn remoteAddr;
} NATNegotiator;
static_assert(sizeof(NATNegotiator) == 0x54);