#pragma once

#include <Common.h>

bool connectToRoomManager();

bool sendToRoomManager(void *message, s32 messageLength);

bool recvFromRoomManager();
