#include <Common.S>

PATCH_REPLACE_START(ItemSlot_init, 0x50)
    li r0, 0x0
PATCH_REPLACE_END(ItemSlot_init, 0x50)