#pragma once

#include <Common.hh>

#include "game/system/InputManager.hh"

namespace System {

class RaceManager {
public:
    enum class RaceState {
        IntroCamera = 0,
        Countdown = 1,
        Racing = 2,
        Finished = 3,
        FinishedTimeAttack = 4,
    };
    class Player {
    public:
        PadProxy *padProxy();

    private:
        REPLACE void calc();
        void REPLACED(calc)();

        u8 _00[0x08 - 0x00];
        u8 m_playerId;
        u8 _09[0x48 - 0x09];
        PadProxy *m_padProxy;
        u8 _4c[0x54 - 0x4c];
    };
    static_assert(sizeof(Player) == 0x54);

    Player *player(u32 playerId);

    static RaceManager *Instance();

private:
    // CreateInstance() is hooked to set the s_instance pointer
    static REPLACE RaceManager *CreateInstance();
    static RaceManager *REPLACED(CreateInstance)();

    REPLACE void calc();
    void REPLACED(calc)();

    u8 _00[0x0c - 0x00];
    Player **m_players;
    u8 _10[0x28 - 0x10];
    RaceState m_state;
    bool m_introWasSkipped;
    bool m_spectatorMode;
    bool m_canCountdownStart;
    bool m_cutSceneMode;
    bool m_lapCountingIsEnabled;
    u8 _31[0x4c - 0x31];

public:
    static RaceManager *s_instance;
};
static_assert(sizeof(RaceManager) == 0x4c);

} // namespace System
