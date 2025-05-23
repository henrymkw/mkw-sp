#include <Common.S>

PATCH_REPLACE_START(ItemDirector_checkKartCollision, 0x28)
    // This patch allows items to be colidable in TTs.
    // No need to add a setting for this, so just nop original instructions

    // nop out the early return when tts are on
    nop
    nop

PATCH_REPLACE_END(ItemDirector_checkKartCollision, 0x28)