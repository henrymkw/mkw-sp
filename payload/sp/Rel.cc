#include "Rel.hh"

#include "Common.hh"

extern "C" {
#include "sp/Patcher.h"
#include "sp/Payload.h"
}

extern "C" {
#include <revolution.h>
}

#include <array>
#include <cstring>

namespace SP::Rel {

typedef void (*EntryFunction)(void);

struct ModuleInfo {
    u8 hash[NET_SHA1_DIGEST_SIZE];
    u32 size;
};

static EntryFunction entry = nullptr;

static const std::array<ModuleInfo, 4> moduleInfoArray = {
        // clang-format off
        // RMCP
        ModuleInfo
        {
            {
                0x88, 0x7B, 0xCC, 0x07, 0x67, 0x81, 0xF5, 0xB0, 0x05, 0xCC,
                0x31, 0x7A, 0x6E, 0x3C, 0xC8, 0xFD, 0x5F, 0x91, 0x13, 0x00,
            },
            0x004AD3C4,
        },
        // RMCE
        ModuleInfo
        {
            {
                0x07, 0x2B, 0xA4, 0x43, 0x82, 0xFD, 0xCA, 0x9B, 0xF5, 0xC0,
                0xEC, 0x9D, 0x1E, 0x9B, 0x92, 0x2C, 0xAF, 0xD9, 0x92, 0x60,
            },
            0x004ACF94,
        },
        // RMCJ
        ModuleInfo
        {
            {
                0x20, 0xB4, 0x16, 0x1D, 0x41, 0x5E, 0x40, 0x92, 0xA6, 0x29,
                0x9B, 0xB2, 0x85, 0xFA, 0xA6, 0x22, 0x83, 0x5B, 0x2B, 0x16,
            },
            0x004ACABC,
        },
        // RMCK
        ModuleInfo
        {
            {
                0xE0, 0x00, 0x62, 0x50, 0xB1, 0xE5, 0xC1, 0xBD, 0xEE, 0x8B,
                0xDB, 0x0E, 0x78, 0x9E, 0xA2, 0x00, 0xB3, 0xA8, 0x46, 0xE4,
            },
            0x004AD9FC,
        },
        // clang-format on
};

static bool IsClean(const void *rel, u32 roundedRelSize) {
    u32 regionIndex;
    switch (REGION) {
    case REGION_P:
        regionIndex = 0;
        break;
    case REGION_E:
        regionIndex = 1;
        break;
    case REGION_J:
        regionIndex = 2;
        break;
    case REGION_K:
        regionIndex = 3;
        break;
    default:
        assert(!"Invalid game region!");
    }

    u32 relSize = moduleInfoArray[regionIndex].size;
    if (roundedRelSize < relSize) {
        return false;
    }

    u8 hash[NET_SHA1_DIGEST_SIZE];
    NETSHA1Context context;
    NETSHA1Init(&context);
    NETSHA1Update(&context, rel, relSize);
    NETSHA1GetDigest(&context, hash);

    return memcmp(moduleInfoArray[regionIndex].hash, hash, NET_SHA1_DIGEST_SIZE) == 0;
}

std::expected<void, const char *> Load() {
    void *src = reinterpret_cast<void *>(OSRoundUp32B(Rel_getStart()));

    DVDFileInfo fileInfo;
    if (!DVDOpen("/rel/StaticR.rel", &fileInfo)) {
        return std::unexpected("Failed to find StaticR.rel");
    }

    s32 size = OSRoundUp32B(fileInfo.length);
    s32 result = DVDRead(&fileInfo, src, size, 0);
    DVDClose(&fileInfo);
    if (result != size) {
        return std::unexpected("Failed to read StaticR.rel from disc.");
    }
    if (!IsClean(src, size)) {
        return std::unexpected("StaticR.rel has been modified.");
    }

    OSLink(reinterpret_cast<OSModuleHeader *>(src), Rel_getBssSectionStart());
    entry = reinterpret_cast<EntryFunction>(reinterpret_cast<OSModuleHeader *>(src)->prolog);
    return {};
}

void Run() {
    assert(entry);
    Patcher_patch(PATCHER_BINARY_REL);
    entry();
}

} // namespace SP::Rel

extern "C" void Rel_Run() {
    SP::Rel::Run();
}
