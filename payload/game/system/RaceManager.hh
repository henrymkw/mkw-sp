#pragma once

#include "game/system/CourseMap.hh"
#include "game/system/GhostFile.hh"
#include "game/system/InputManager.hh"
#include "game/util/Random.hh"

#include <common/TVec3.hh>

namespace System {

class RaceManager {
public:
    enum class RaceInfoPlayerFlags {
        IN_RACE = 0x1,
        FINISHED = 0x2,
        DRIVING_WRONG_WAY = 0x4,
        FINISHED_REMOTE = 0x8,
        DISCONNECTED = 0x10,
        STOPPED = 0x20,
        COMING_LAST_ANIMATION = 0x40,
    };
    class Player {
    public:
        u8 rank() const;
        u16 battleScore() const;
        u8 maxLap() const;
        bool hasFinished() const;
        PadProxy *padProxy();
        void setExtraGhostPadProxy();

    private:
        REPLACE void calc();
        void REPLACED(calc)();

        void *vtable;
        u8 _04[0x08 - 0x04];
        u8 m_playerId; // idx
        u8 _09;
        u16 checkpointId;
        f32 raceCompletion;
        f32 raceCompletionMax;
        f32 checkpointFactor;
        f32 checkpointStartLapCompletion;
        f32 lapCompletion;
        u8 m_rank; // position
        u8 respawn;
        u16 m_battleScore; // battleScore
        s16 currentLap;
        u8 m_maxLap; // maxLap
        u8 currentKcp;
        u8 maxKcp;
        u8 _29[0x2b - 0x29];
        u32 frameCounter;
        u32 framesInFirstPlace;
        s32 _34;
        RaceInfoPlayerFlags flags;
        Time *lapFinishTimes;
        Time *raceFinishTime;
        u32 somethingRaceEndMessageOnline;
        PadProxy *m_padProxy; // input
        u8 _4c[0x4f - 0x4c];
        u16 playersAheadFlags;
        u8 _52;
        u8 finishingPosition;
    };
    static_assert(sizeof(Player) == 0x54);

    enum class Stage {
        Sync,
        Countdown,
        Race,
        // ...
    };

    Util::Random *dynamicRandom();
    Player *player(u32 playerId);
    u32 time() const;
    bool hasReachedStage(Stage stage) const;
    MapdataKartPoint *REPLACED(getKartPoint)(u32 playerId);
    REPLACE MapdataKartPoint *getKartPoint(u32 playerId);
    void REPLACED(getStartTransform)(Vec3 *pos, Vec3 *rot, u32 playerId);
    REPLACE void getStartTransform(Vec3 *pos, Vec3 *rot, u32 playerId);
    void REPLACED(calc)();
    REPLACE void calc();
    void REPLACED(endPlayerRace)(u32 playerId);
    REPLACE void endPlayerRace(u32 playerId);

    static REPLACE RaceManager *CreateInstance();
    static RaceManager *Instance();
    static u8 GetLapCount();

private:
    RaceManager();

    u8 _00[0x04 - 0x00];
    Util::Random *m_dynamicRandom;
    Util::Random *m_staticRandom;
    Player **m_players;
    u8 _10[0x20 - 0x10];
    u32 m_time;
    u8 m_battleKartPointStart;
    u8 _25[0x2d - 0x25];
    bool m_spectatorMode;

public:
    bool m_canStartCountdown;

private:
    u8 _2e[0x4c - 0x2f];

    static RaceManager *s_instance;
};
static_assert(sizeof(RaceManager) == 0x4c);

} // namespace System
