#pragma once

#include <Common.h>
#include <revolution.h>

#include <gamespy/qr2/qr2.h>
#include <revolution/so/so.h>

typedef struct QR2Implementation {
    SOCKET hbsock;
    u8 _004[0x0c0 - 0x004];
    s32 qport;
    s32 read_socket;
    s32 nat_negotiate;
    SOSockAddrIn hbaddr;
    u8 _0d4[0xd94 - 0x0d4];

} QR2Implementation;
