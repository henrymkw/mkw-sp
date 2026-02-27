#pragma once

#include <Common.h>

bool connectToRoomManager();

void resetRoomManagerConnection();

bool sendToRoomManager(void *message, s32 messageLength);

bool recvFromRoomManager();
