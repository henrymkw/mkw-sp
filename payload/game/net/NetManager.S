#include <Common.S>

PATCH_REPLACE_START(NetManager_CreateInstance, 0x28) 
    // patch the call to new to account for the addition of m_outgoingUniquePackets;
    li r3, 0x2a28
PATCH_REPLACE_END(NetManager_CreateInstance, 0x28)

