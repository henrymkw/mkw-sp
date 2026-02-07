#pragma once

#include <Common.hh>

extern "C" {
#include <revolution/os.h>
}

#include <game/util/Registry.hh>

namespace Net {
enum class OnlineMode {
    PublicVS = 1,
    PublicBattle = 2,
    PrivateVS = 3,
    PrivateBattle = 4,
};

// TODO: better name, odd grouping of data, maybe used for the results screen?
struct SelectPlayerInfo {
    u16 startingSpot;
    u16 points;
    Registry::Character character : 8;
    Registry::Vehicle vehicle : 8;
    Registry::Vote course : 8;
    u8 starRank;
};
static_assert(sizeof(SelectPlayerInfo) == 0x8);

struct SelectRecord {
    OSTime timeSent;
    OSTime timeReceived;
    SelectPlayerInfo selectPlayerInfo[2];
    u8 _20[0x28 - 0x20];
    u8 playerIdToAidMapping[12];
    Registry::Course winningCourse : 8; // not sure if this should be Vote
    u8 _35;
    u8 winningVoterAid;
    u8 engineClass;
};
static_assert(sizeof(SelectRecord) == 0x38);
class SelectHandler {
private:
    static REPLACE SelectHandler *CreateInstance();
    static SelectHandler *REPLACED(CreateInstance)();

    void update();

    OnlineMode mode;
    u8 _004[0x008 - 0x004];
    SelectRecord m_sendPacket;
    SelectRecord m_recvPackets[12];
    u8 m_lastSentToAid; // bitfield
    u8 _2e1[0x2e8 - 0x2e1];
    OSTime m_lastSendTime;
    OSTime m_lastReceivedTimes[12];
    OSTime _350[12];
    u32 _3b0[12];
    u32 m_hasNewSelect; // bitfield
    u32 m_hasNewRH1;    // bitfield
    u32 _3e8;
    u32 _3ec;
    u32 m_aidsThatVoted; // bitfield
    u8 _3f4[0x3f8 - 0x3f4];

    static SelectHandler *s_instance;
};
static_assert(sizeof(SelectHandler) == 0x3f8);

} // namespace Net
