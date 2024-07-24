#include <Common.S>

PATCH_REPLACE_START(SaveManager_CreateInstance, 0x2c)

addi r3, r3, 0x64b8

PATCH_REPLACE_END(SaveManager_CreateInstance, 0x2c)
