#pragma once

#include "NandHelper.hh"

namespace System {

class NandManager {
public:
    NandManager() = default;
    ~NandManager() = default;

    static NandManager* Instance() {
        return s_instance;
    }

private:
    NandResult readSaveData(void *rawFile, u32 len, u32 offset, s32 unk);
    NandResult writeSaveData(void *rawFile, u32 len, u32 offset, s32 unk);
    NandResult readFile(char *rawFile, u32 len, u32 offset, s32 unk);
    NandResult writeFile(char *rawFile, u32 len, u32 offset, s32 unk);
    u8 _00[0x28 - 0x00];
    
    static NandManager* s_instance;
};
static_assert(sizeof(NandManager) == 0x28);

}
