#include "dwc.h"

#include <revolution.h>

bool DWCi_Acc_IsAuthentic(u32 unk) {
    bool result = REPLACED(DWCi_Acc_IsAuthentic)(unk);
    u32 val = *(u32 *)(unk + 0x10);
    SP_LOG("IsAuthentic() val: %d", val);
    SP_LOG("IsAuthentic() result %d", result);
    return result;
    
}

bool DWC_CheckUserData(DWCUserData *unk) {
    SP_LOG("unk: %p", unk);
    SP_LOG("unk->dataSize: %d", unk->dataSize);
    SP_LOG("unk->pseudoUserID: %llu", unk->pseudoUserID);
    SP_LOG("unk->pseudoPlayerID: %d", unk->pseudoPlayerID);
    SP_LOG("unk->authenticUserID: %llu", unk->authenticUserID);
    SP_LOG("unk->authenticPlayerID: %d", unk->authenticPlayerID);
    SP_LOG("unk->profileID: %d", unk->profileID);
    SP_LOG("unk->gameID: %s", unk->gameID);
    SP_LOG("unk->crc: %d", unk->crc);

    bool result = REPLACED(DWC_CheckUserData)(unk);
    SP_LOG("CheckUserData() result %d", result);
    return result;
}
