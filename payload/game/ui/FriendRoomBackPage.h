#pragma once

#include <Common.h>

typedef struct {
    u8 _0000[0x2c68 - 0x0000];
} FriendRoomBackPage;
static_assert(sizeof(FriendRoomBackPage) == 0x2c68);

// Hooked to send a LeaveRoom upon exiting the friend room screen
REPLACE void onDeactivate();
void REPLACED(onDeactivate)();
