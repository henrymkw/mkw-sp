#pragma once

#include <Common.h>

#include <gamespy/gt2/gt2.h>

// 0x8010ced8
GT2Bool gti2ReceiveMessages(GT2Socket socket);

// 0x8010b480
GT2Bool gti2DeliverReliableMessage(GT2Connection connection, GTI2MessageType type, GT2Byte *message,
        int len);

// 0x8010c6fc
GT2Bool gti2HandleMessage(GT2Socket socket, GT2Byte *message, s32 len, u32 ip, u16 port);

// 0x8010bdfc
GT2Bool gti2HandleReliableMessage(GT2Connection connection, GTI2MessageType type, GT2Byte *message,
        s32 len);

// 0x8010c434
GT2Bool gti2HandleUnreliableMessage(GT2Connection connection, GTI2MessageType type,
        GT2Byte *message, s32 len);

// 0x8010b178
GT2Bool gti2HandleClientResponse(GT2Connection connection, GT2Byte *message, s32 len);

// 0x8010dd40
GT2Bool gti2Send(GT2Connection connection, const GT2Byte *message, s32 len, bool reliable);
