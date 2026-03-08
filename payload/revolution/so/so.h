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

// 0x801ecff4
SOCKET SOSocket(int pf, int type, int protocol);

// 0x801ed0e4
int SOClose(int s);

// 0x801ed4a0
// patching the address here felt like lag starts improved, but I think that was a placebo
REPLACE int SOSendTo(int s, const u8 *buf, int len, int flags, SOSockAddrIn *sockTo);
int REPLACED(SOSendTo)(int s, const u8 *buf, int len, int flags, SOSockAddrIn *sockTo);

// 0x801ed188
int SOBind(int s, const void *sockAddr);

// 0x801ed47c
int SORecv(int s, void *buf, int len, int flags);

// 0x801ed454
int SORecvFrom(int s, void *buf, int len, int flags, void *sockFrom);

int SOFcntl(int s, int cmd, ...);

int SOStartup();

int SOConnect(int s, const void *sockAddr);

int SOSend(int s, const void *buf, int len, int flags);

// 0x801ed9a4
void net_recvfrom(int r3, s32 s, void *buf, u32 len, u32 flags, s32 *fromlen);

// 0x801ed99c
u16 SOHtoNs(u16 hostshort);

s32 SOGetHostByName(const char *name);

// 0x800f14e4
s32 inet_addr(const char *name);

// 0x800f164c
void *gethostbyname(const char *name);

// 0x800f118c
s32 socket(s32 domain, int type, int protocol);

// 0x800f12a4
s32 connect(s32 sd, void *sockaddr, s32 socklen);

s32 send(s32 sock, const void *buf, s32 len, s32 flags);
