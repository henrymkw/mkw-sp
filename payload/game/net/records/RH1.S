#include <Common.S>

#ifdef DEBUG

PATCH_REPLACE_START(RH1Handler_update, 0x4c)
    // Reduce time before requesting suspension in the case there's 4 or less players from 40 seconds to 5 to improve debugging experience
    cmplwi r4, 0x5
PATCH_REPLACE_END(RH1Handler_update, 0x4c)

#endif
