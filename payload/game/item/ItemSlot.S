#include <Common.S>

PATCH_B_START(ItemSlot_init, 0x50)
    // Original instruction
    lbz r0, 0x38bc(r3)

    // Check if item boxes are enabled in TTs
    mr r12, r3
    bl SaveManager_GetTAItemBoxes
    cmpwi r3, 0x1
    bne jump_back

    // Setting r0 to 0, which makes this function runs as if TTs isn't the current mode.
    li r0, 0x0
  
jump_back:
    mr r3, r12
    b ItemSlot_init + 0x54
PATCH_B_END(ItemSlot_init, 0x50)
