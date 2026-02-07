extern "C" {
#include "Payload.h"
}

extern "C" {
#include "sp/Patcher.h"
}
#include "sp/Rel.hh"
#include "sp/storage/DecompLoader.hh"
#include "sp/storage/LogFile.hh"
#include "sp/storage/Storage.hh"
#include "sp/Time.hh"

#include <common/Console.hh>
#include <common/VI.hh>
extern "C" {
#include <revolution.h>
#include "sp/storage/Usb.h"
#include "sp/Host.h"
}

#include <cstring>

extern void (*payload_ctors_start)(void);
extern void (*payload_ctors_end)(void);

namespace SP::Payload {

static void Init() {
    VI::Init();

    Console::Init();
    Console::Print("Reconfiguring payload memory...");
    OSSetMEM1ArenaLo(Payload_getEnd());
    Console::Print(" done.\n");

    Console::Print("Initializing RTC...");
    Time::Init();
    Console::Print(" done.\n");

    Console::Print("Initializing host...");
    Host_Init();
    Console::Print(" done.\n");

    Console::Print("Initializing USB...");
    bool usbWasInit = Usb_init();
    assert(usbWasInit);
    Console::Print(" done.\n");

    Console::Print("Initializing storage...");
    if (!Storage::Init()) {
        Console::Print(" failed!\n");
        Console::Print("Please make sure that an SD or USB device is inserted.\n");
        Console::Print("Trying again in a loop...");
        u32 i;
        for (i = 0; i < 60 && !Storage::Init(); i++) {
            OSSleepMilliseconds(500);
        }
        if (i == 60) {
            Console::Print(" failed!\n");
        }
    }
    Console::Print(" done.\n");

    Console::Print("Initializing LogFile...");
    SP::LogFile::Init();
    Console::Print(" done.\n");

    Host_PrintMkwSpInfo(OSReport);

    Console::Print("Initializing concurrent decompressor...");
    Storage::DecompLoader::Init();
    Console::Print(" done.\n");

    Console::Print("Loading StaticR.rel...");
    auto rel_ok = Rel::Load();
    if (!rel_ok) {
        Console::Print(" failed with reason \"");
        Console::Print(rel_ok.error());
        Console::Print("\"!\n");
        Console::Print("Hanging...");
        asm("b 0x0");
    }
    Console::Print(" done.\n");

    VIInit();
    VISetBlack(true);
    VIFlush();
    VIWaitForRetrace();
    // We don't clear the arena in OSInit because the payload is already copied at that point, and
    // the XFB would turn green, but some code expects it to be zeroed.
    auto *mem1Lo = reinterpret_cast<u8 *>(OSGetMEM1ArenaLo());
    auto *mem1Hi = reinterpret_cast<u8 *>(OSGetMEM1ArenaHi());
    memset(OSGetMEM1ArenaLo(), 0, mem1Hi - mem1Lo);
}

static void Run() {
    for (void (**ctor)(void) = &payload_ctors_start; ctor < &payload_ctors_end; ctor++) {
        (*ctor)();
    }
    Patcher_patch(PATCHER_BINARY_DOL);
}

} // namespace SP::Payload

extern "C" void Payload_Init() {
    SP::Payload::Init();
}

extern "C" __attribute__((no_stack_protector, section("first"))) void Payload_Run() {
    SP::Payload::Run();
}
