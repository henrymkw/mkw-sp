#include <Common.S>

// both of these nops aren't ideal solutions, but work for now.
// Ideally, we would adjust how DWCTransportInfo.timeoutTime is set up

// This nops the bl to DWC_CloseConnectionHard for guests
PATCH_REPLACE_START(DWCi_TransportProcessASM, 0x1b8)
    nop
PATCH_REPLACE_END(DWCi_TransportProcessASM, 0x1b8)

// This nops the bl to DWC_CloseConnectionHard for hosts
PATCH_REPLACE_START(DWCi_TransportProcessASM, 0x108) 
    nop    
PATCH_REPLACE_END(DWCi_TransportProcessASM, 0x108)
