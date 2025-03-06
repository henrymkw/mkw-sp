#include <Common.S>

PATCH_B_START(ObjManager_ct, 0x12c) 
    // I am terrible at writing asm, dont mind me
    // original instruction
    li r7, 0x1

    // Get the item box setting and check the result
    bl SaveManager_GetTAItemBoxes
    cmpwi r3, 0x1
    bne jump_back
    
    // Overwrite the original instruction if the setting is enabled.
    li r7, 0x0
    b ObjManager_ct + 0x130

jump_back:
    b ObjManager_ct + 0x130
PATCH_B_END(ObjManager_ct, 0x12c)