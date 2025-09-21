#pragma once

#include <Common.h>

#include <revolution/os.h>

typedef struct SOInAddr {
    u32 addr;

} SOInAddr;

typedef struct SOSockAddr {
    u8 len;
    u8 family;
    u8 data[6];

} SOSockAddr;

typedef struct SOSockAddrIn {
    u8 len;
    u8 family;
    u16 port;
    SOInAddr addr;
} SOSockAddrIn;

#define SOCKET s32

// 0x801ecff4
SOCKET SOSocket(int pf, int type, int protocol);

// 0x801ed0e4
int SOClose(int s);

// 0x801ed4a0
REPLACE int SOSendTo(int s, const char *buf, int len, int flags, void *sockTo);
int REPLACED(SOSendTo)(int s, const char *buf, int len, int flags, void *sockTo);

// 0x801ed188
int SOBind(int s, const void *sockAddr);

// 0x801ed47c
int SORecv(int s, void *buf, int len, int flags);

// 0x801ed454
int SORecvFrom(int s, void *buf, int len, int flags, void *sockFrom);

int SOStartup();

int SOConnect(int s, const void *sockAddr);

int SOSend(int s, const void *buf, int len, int flags);

// 0x801ed9a4
void net_recvfrom(int r3, int s, void *buf, unsigned int len, unsigned int flags, int *fromlen);

// 0x801ed99c
u16 SOHtoNs(u16 hostshort);
