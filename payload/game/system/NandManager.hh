#pragma once

#include "NandHelper.hh"

namespace System {

/**
 * @brief Currently unused by SP, here for documentation.
 *
 */
class NandManager {
public:
    NandManager() = default;
    ~NandManager() = default;

    static NandManager *Instance() {
        return s_instance;
    }

private:
    /**
     * @brief Reads n bytes from rksys.dat starting at a passed in offset. Likely public.
     *
     * @param buffer Buffer to read into
     * @param len num bytes to read
     * @param offset offset to read from
     * @param unk unknown, always seems to be 1
     * @return NandResult Result of the operation
     */
    NandResult readSaveData(void *buffer, u32 len, u32 offset, s32 unk); // 0x8052c0b0

    /**
     * @brief Writes n bytes to rksys.dat starting at a passed in offset. Likely public.
     *
     * @param buffer Of type RawSave. Need to figure out the public/private organization.
     * @param len num q
     * @param offset offset to write to
     * @param unk unknown, always seems to be 1
     * @return NandResult Result of the operation
     */
    NandResult writeSaveData(void *buffer, u32 len, u32 offset, s32 unk); // 0x8052c2d0

    /**
     * @brief TODO: Document this function
     *
     * @param rawFile
     * @param len
     * @param offset
     * @param unk
     * @return NandResult
     */
    NandResult readFile(char *rawFile, u32 len, u32 offset, s32 unk); // 0x8052c168

    /**
     * @brief TODO: Document this function
     *
     * @param rawFile
     * @param len
     * @param offset
     * @param unk
     * @return NandResult
     */
    NandResult writeFile(char *rawFile, u32 len, u32 offset, s32 unk); // 0x8052c428

    // private data
    u8 _00[0x28 - 0x00];

    // PTR to s_RawRksys.dat is referenced throughout the orignal implementation
    static NandManager *s_instance; // 0x809bd720
};
static_assert(sizeof(NandManager) == 0x28);

} // namespace System
