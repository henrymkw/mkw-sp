#include <Common.S>

// This patch changes the offline behavior if FIBs to stick to the ground when thrown.
// This is their behavior for online. 
PATCH_B_START(ItemObjThrowable_CalcCollision, 0x298)
    // Check if the item is a FIB (objID is 7)
    lwz r23, 0x4(r29)
    cmpwi r23, 0x7
    bne original_check
    
    // Obj is a FIB, so branch to the stick to ground instruction.
    b ItemObjThrowable_CalcCollision + 0x29c 

original_check:
    // Original comparison instruction, branch to the branch instruction
    cror eq, lt, eq
    b ItemObjThrowable_CalcCollision + 0x29c
PATCH_B_END(ItemObjThrowable_CalcCollision, 0x298)