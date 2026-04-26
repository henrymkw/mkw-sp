#pragma once

#include <Common.hh>

extern "C" {
#include <revolution/os.h>
}

#include <game/util/Registry.hh>

namespace Net {

struct RH1Record {
    u32 elapsedTimeSinceRaceStart;
    u32 seed; // random seed set from RaceConfig::Settings, MiscPacketHandler's bitfield
    Registry::Team p1Team : 16;
    Registry::Team p2Team : 16;
    u16 lagFrames;
    Registry::Vehicle p1Vehicle : 8;
    Registry::Vehicle p2Vehicle : 8;
    Registry::Character p1Character : 8;
    Registry::Character p2Character : 8;
    u16 countDownTime;
    u16 starRank;
    Registry::Course coursePlayed : 8;
    // sets bitfield when != 0, but not sure how it gets set.
    // wiki refers to this as playerType, but ghidra doesn't pick up on RH1Record well automatically
    // so can't corroborated yet
    u8 _17;
    u8 aidMap[12];
    Registry::EngineClass engineClass : 8;

    u8 _25[0x28 - 0x25];
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

    void reset();

    static RH1Handler *Instance() {
        return s_instance;
    }

private:
    void calc();
    void exportPackets();
    void importPackets();

    bool m_prepared;
    u8 _001[0x004 - 0x001];
    // gets set when importing a players RH1 record, but doesn't get cleared until
    // it gets reset by a dc (from static analysis). Effectively, since it doesn't get cleared,
    // it's effectively an availableAids bitfield. Though it only gets read in the following way
    // m_availableAids != 0, not like a bitfield but checking that at least one aid has sent a RH1
    // record
    u32 m_hasRecvRH1; // bitfield by aid

    // this also gets set when importing a players RH1 record, but it gets cleared
    // after exporting rh1 to that player. bitfield if not clear
    u32 m_hasRecvRH1FromAid;
    u8 _00c[0x018 - 0x00c];
    OSTime m_time;
    RH1Player m_RH1Players[12];

    static RH1Handler *s_instance;
};
static_assert(sizeof(RH1Handler) == 0x260);

} // namespace Net
