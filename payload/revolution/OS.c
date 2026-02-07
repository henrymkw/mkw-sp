#include "revolution/os.h"

#include "sp/Payload.h"

// Tail call to Payload_Init
PATCH_B(OSInit + 0x474, Payload_Init);
