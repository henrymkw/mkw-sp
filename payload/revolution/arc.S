#include <Common.S>

// nop out warning that gets spammed
PATCH_REPLACE_START(ARCOpen, 0x248)
   nop
PATCH_REPLACE_END(ARCOpen, 0x248)