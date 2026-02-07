#pragma once

#include <egg/core/eggHeap.hh>
#include <sp/IOSDolphin.hh>

namespace System {

class SystemManager {
public:
    void REPLACED(init)();
    REPLACE void init();
    REPLACE void shutdownSystem();
    REPLACE void returnToMenu();
    REPLACE void restart();
    u32 aspectRatio() const;
    u32 matchingArea() const;
    u32 launchType() const;

    static void ShutdownSystem();
    static void ReturnToMenu();
    static void Restart();
    static void LaunchTitle(u64 titleID);
    static u8 *RipFromDisc(const char *path, EGG::Heap *heap, bool allocTop, u32 *size);

    static SystemManager *Instance() {
        return s_instance;
    }

private:
    u8 _0000[0x0058 - 0x0000];
    u32 m_aspectRatio;
    u8 _005c[0x0070 - 0x005c];
    u32 m_launchType;
    u8 _0074[0x0084 - 0x0074];
    u32 m_matchingArea;
    u8 _0088[0x1100 - 0x0088];

    static SystemManager *s_instance;
};

static_assert(sizeof(SystemManager) == 0x1100);

} // namespace System
