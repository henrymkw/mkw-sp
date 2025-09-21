#pragma once

#include <Common.hh>

#include "game/net/DisconnectInfo.hh"
#include "game/net/FriendInfo.hh"
#include "game/net/PacketHolder.hh"
#include "game/net/RACEPacketHolder.hh"
#include "game/net/packets/EVENT.hh"
#include "game/net/packets/HEADER.hh"
#include "game/net/packets/ITEM.hh"
#include "game/net/packets/RACEDATA.hh"
#include "game/net/packets/RACEHEADER1.hh"
#include "game/net/packets/RACEHEADER2.hh"
#include "game/net/packets/SELECT.hh"
#include "game/net/packets/USER.hh"

#include <egg/core/eggExpHeap.hh>
#include <egg/core/eggTaskThread.hh>

extern "C" {
#include <revolution/dwc/DWCFriend.h>
#include <revolution/os.h>
#include <revolution/os/OSMutex.h>
}

#define MAX_FRIEND_COUNT 30
#define MAX_PLAYER_COUNT 12
namespace Net {

class NetManager {
private:
    enum class ConnectionState : u32 {
        Shutdown = 0x0, // offline
        BeginLogin = 0x1,
        CheckProfanity = 0x2,
        UnkGPFriends = 0x3,  // Friend related, needs better name
        SyncFriends = 0x4,   // checks if a friend has added back
        Idle = 0x5,          // online but not doing anything
        InMatchMaking = 0x6, // searching or in a room
        Error = 0x7,         // non SAKE errors
        SAKEError = 0x8,     // set when the EC is 4xxxx or 98xxx
    };

    enum class RoomType : u32 {
        None = 0x0,
        VersusWorldWide = 0x1,
        VersusRegional = 0x2,
        BattleWorldWide = 0x3,
        BattleRegional = 0x4,
        HostPrivate = 0x5,
        GuestPrivate = 0x6,
        JoiningFriendVsWorldWide = 0x7,
        JoiningFriendVsRegional = 0x8,
        JoiningFriendBattleWorldWide = 0x9,
        JoiningFriendBattleRegional = 0xA,
    };

    // Only applies when m_connectionState == CONNECTION_STATE_IN_MM.
    // MatchMakingInfo::isMatchMakingSuspended controls whether others can join a
    // room. To change its value, clients vote for the room to be
    // suspended/unsuspended. The host sets the flag by unanimus vote.
    enum class VoteMatchMakingSuspended : u32 {
        None = 0x0,
        Disconnected = 0x1,  // Triggers a dc
        VoteSuspend = 0x2,   // Set when ending a public race or starting a private room
        VoteUnsuspend = 0x3, // Set when private room ends
    };

    struct MatchMakingInfo {          // 0x0038
        OSTime matchMakingStartTime;  // gets set upon match making 0x0 / 0x0038
        u32 numConnectedConsoles;     // number of non guest players 0x8  / 0x0040
        u32 playerCount;              // players in room (includes guests) 0xC / 0x0044
        u32 fullAidBitmap;            // # bits is equal to num consoles, all 1 0x10 / 0x0048
        u32 directConnectedAidBitmap; // Aids I'm connected to. It will fill up to
                                      // equal fullAidBitmap by the end of MM as
                                      // I connect to other users. 0x14 / 0x004c
        u32 roomId;                   // Also known as groupId by DWC 0x18 / 0x0050
        s32 hostFriendId;             // -1 if host isn't a friend. 0x1C / 0x0054
        u8 localPlayerCount;          // 0x20 / 0x0058
        u8 myAid;                     // 0x21 / 0x0059
        u8 hostAid;                   // value returned by DWC_GetServerAid() 0x22 / 0x005a
        LocalPlayerCountDWC localPlayerCounts[MAX_PLAYER_COUNT];
        // When matching is suspended, friends aren't able to join your room.
        // This gets set to true during the voting screen in public rooms
        // and transitioning to opening a private room, both cases friends can't
        // join.
        bool isMatchMakingSuspended; // 0x53 / 0x008b
        u8 _54[0x58 - 0x54];
    };
    static_assert(sizeof(MatchMakingInfo) == 0x58);

    // Checks if the aid is in the room and that its not our aid.
    // This same logic is in the base game (inlined) but don't know if a function exists.
    bool IsValidSendableAid(u8 aid) const;

    REPLACE void SendRacePacket();
    void REPLACED(SendRacePacket)();

    // nearly byte-matched (regswap) so REPLACE is acceptable
    REPLACE void ProcessRACEPacket(u8 aid, HEADERPacket *header, u32 size);

    // Two vtables
    void *m_vtable1; // offset 0xc is NetManager's dtor
    void *m_vtable2; // unk dtor at 0xc, also present in FriendManager vtable
    OSMutex m_mutex;
    EGG::ExpHeap *m_heap;
    EGG::TaskThread *m_taskThread; // runs the mainLoop
    ConnectionState m_connectionState;
    DisconnectInfo m_disconnectInfo;
    u8 _0034[0x0038 - 0x0034];             // padding?
    MatchMakingInfo m_matchMakingInfos[2]; // 0x0038 - 0x00e8
    RoomType m_roomType;
    VoteMatchMakingSuspended m_voteMMSuspension;
    // points to RACE packets to be sent, two per aid / 0xf0
    RACEPacketHolder *m_sendRACEPackets[2][MAX_PLAYER_COUNT];
    // points to RACE packets to be recieved, two per aid / 0x150
    RACEPacketHolder *m_recvRACEPackets[2][MAX_PLAYER_COUNT];
    // The RACE packet to be sent, formed from m_sendRACEPackets, one per aid /
    // 0x1b0
    PacketHolder *m_outgoingRACEPacket[MAX_PLAYER_COUNT];
    OSTime m_timeOfLastSentRACE[MAX_PLAYER_COUNT];        // 0x1e0
    OSTime m_timeOfLastRecvRACE[MAX_PLAYER_COUNT];        // 0x240
    OSTime m_timeBetweenSendingPackets[MAX_PLAYER_COUNT]; // time bewteen sent
                                                          // packets per aid / 0x2a0
    OSTime m_timeBetweenRecvPackets[MAX_PLAYER_COUNT];    // time between recieved
                                                          // packets per aid / 0x300
    u8 m_aidLastSentTo;                                   // Aid of last player we sent to / 0x360
    u8 m_recvRACEPacketBuffer[MAX_PLAYER_COUNT][0x2e0];   // 0x361
    u8 _25e1[0x25e4 - 0x25e1];                            // padding
    StatusData m_myStatusData;                            // 0x25e4
    FriendInfo m_friends[MAX_FRIEND_COUNT];
    bool m_friendRosterChanged;      // set when a friend adds back 0x2753
    bool m_shutdownScheduled;        // set when logging off // 0x2755
    bool m_shouldUpdateFriendStatus; // 0x2756
    bool m_hasEjectedDisk;           // triggers a dc screen 0x2757
    bool m_profanityCheckFailed;     // 0x2758
    u8 _2759[0x275c - 0x2759];
    s32 m_badWordsNum; // number of bad words found in the profanity check
    u32 m_disconnectPenalty;
    s32 m_vr;
    s32 m_br;
    u32 m_lastSendIdx[MAX_PLAYER_COUNT]; // idx of m_sendRACEPackets last sent per
                                         // aid
    // idx of m_recvRACEPackets last recvieved per packet per aid
    u32 m_lastRecvIdx[MAX_PLAYER_COUNT][8];      // 0x279c
    u32 m_currMMInfo;                            // Current MM info used 0x291c
    u8 m_playerIdToAidMapping[MAX_PLAYER_COUNT]; // 0x2920
    u32 m_disconnectedAids;                      // disconnected if 1 << aid is 1 // 0x292c
    u32 m_disconnectedPlayerIds;                 // disconnected if 1 << pid is 1 // 0x2930
    u8 _2934[0x295c - 0x2934];                   // elo based MM struct
    u8 _295c[0x29c8 - 0x295c];                   // some timers

    static NetManager *s_instance;
};

} // namespace Net
