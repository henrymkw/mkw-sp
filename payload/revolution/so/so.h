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

#define AF_INET 2

#define SOCK_STREAM 1

#define IPPROTO_TCP 6
#define SO_F_GETFL 3
#define SO_F_SETFL 4
#define SO_O_NONBLOCK 0x04

#define SOCKET s32

// 0x801ec184
int SOStartup();

// 0x801ecff4
SOCKET SOSocket(int pf, int type, int protocol);

// 0x801ed0e4
int SOClose(int s);

// 0x801ed188
int SOBind(int s, const void *sockAddr);

// 0x801ed270
int SOConnect(int s, const void *sockAddr);

// 0x801ed454
int SORecvFrom(int s, void *buf, int len, int flags, void *sockFrom);

// 0x801ed47c
int SORecv(int s, void *buf, int len, int flags);

// 0x801ed4a0
int SOSendTo(int s, const void *buf, int len, int flags, SOSockAddrIn *sockTo);

// 0x801ed4c8
int SOSend(int s, const void *buf, int len, int flags);

// 0x801ed4ec
int SOFcntl(int s, int cmd, ...);

// 0x801ed82c
BOOL SOInetAtoN(const char *name, int *addr);

// 0x801ed99c
short SOHtoNs(short hostshort);

// 0x801edf00
s32 SOGetHostByName(const char *name);
