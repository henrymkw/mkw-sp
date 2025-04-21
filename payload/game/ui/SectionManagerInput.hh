#pragma once

#include <Common.hh>

#include "game/system/InputManager.hh"

namespace UI {

enum {
    REGISTERED_PAD_TYPE_GC = 0x4,
};

#define REGISTERED_PAD_FLAGS_GET_TYPE(flags) ((flags) >> 0 & 0xf)

class SectionManagerInput {
public:
    void onCreateSection(bool r4);
    void setDriftIsAuto(u32 localPlayerId, bool driftIsAuto);
    void setFlags(u32 localPlayerId, u32 flags);
    u32 getFlags(u32 localPlayerId);
    void _8061B5A4();
    
    enum class SectionManagerInputInfoEnum {
        WII_WHEEL = 0x11,
        NUNCHUCK = 0x12,
        CLASSIC = 0x13,
        GCN = 0x24,
        PORT_1 = 0x100,
        PORT_2 = 0x200,
        PORT_3 = 0x300,
        PORT_4 = 0x400,
    };

private:
    struct SectionManagerInputInfo {
        SectionManagerInputInfoEnum lastConnectedController;
        SectionManagerInputInfoEnum currentController;
        u32 isDisconnected;
        System::PadProxy *padProxy;
    };

    u8 _00[0x04 - 0x00];
    SectionManagerInputInfo m_inputInfo[5];
    u8 _54[0x5c - 0x54];
};
static_assert(sizeof(SectionManagerInput) == 0x5c);


} // namespace UI
