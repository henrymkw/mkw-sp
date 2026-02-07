#pragma once

#include <Common.hh>

extern "C" {
#include <revolution/os.h>
}

#include <game/util/Registry.hh>

namespace Net {

struct RH1Record {
    u32 elapsedTimeSinceRaceStart;
    u32 seed;
    u8 _08[0x28 - 0x08];
};
static_assert(sizeof(RH1Record) == 0x28);

class RH1Handler {
public:
    struct RH1Player {
        u32 timer;
        Registry::Vehicle player1Vehicle;
        Registry::Vehicle player2Vehicle;
        Registry::Character player1Character;
        Registry::Character player2Character;
        Registry::Course course;
        u8 _18[0x20 - 0x18];
        u8 playerIdToAidMapping[12];
        u8 _2c[0x30 - 0x2c];
    };
    static_assert(sizeof(RH1Player) == 0x30);

private:
    void calc();
    void exportPackets();
    void importPackets();

    bool m_prepared;
    u8 _001[0x004 - 0x001];
    u32 m_hasNewRH1;
    u8 _008[0x018 - 0x008];
    OSTime m_time;
    RH1Player m_RH1Players[12];

    static RH1Handler *s_instance;
};
static_assert(sizeof(RH1Handler) == 0x260);

} // namespace Net
