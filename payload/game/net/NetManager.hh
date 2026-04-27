#pragma once

#include <Common.hh>

#include "game/net/DisconnectInfo.hh"
#include "game/net/FriendInfo.hh"
#include "game/net/MatchMakingInfo.hh"
#include "game/net/RacePacketHolder.hh"

#include <egg/core/eggExpHeap.hh>
#include <egg/core/eggTaskThread.hh>

#include <sp/net/mkw_server/RoomManager.hh>
#include <sp/net/mkw_server/packets/OutgoingPacket.hh>

#define MAX_FRIEND_COUNT 30
#define MAX_PLAYER_COUNT 12

namespace Net {

class NetManager {
public:
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

    static NetManager *Instance() {
        return s_instance;
    }

private:
    const MatchMakingInfo *currentMMInfo() const;

    u8 myAid() const;

    u32 numAids() const;

    bool aidInUse(u8 aid) const;

    // check that the aid isn't ours and the aid is in the room before sending a race packet
    bool canSendToAid(u8 aid) const;

    RacePacketHolder *lastSentRaceBuffer(u8 aid);

    RecordHolder<Header> *outgoingBuffer(u8 aid);

    // 'converts' the REGION to associated SearchRegion
    SearchRegion getSearchRegion();

    // 0x80655c10
    // Hooked to initialize m_outgoingUniquePackets
    REPLACE void init(u8 localPlayerCount);
    void REPLACED(init)(u8 localPlayerCount);

    // 0x806561a8
    // Sets m_shutdownScheduled, which mainNetworkLoop() reacts to during next iteration.
    // Resets all wfc-server/mkw-server structures
    REPLACE void scheduleShutdown();

    // 0x80656898
    // Called in vanilla to exit match making state, hooked to reset mkw-server mm info
    REPLACE void cancelMatching();

    // 0x80656f00
    // checks that my aid is unavailable and we have connected to someone
    REPLACE bool hasFoundMatch() const;

    // 0x80657ab0
    // Completely rewritten. This will export unique packets to the send buffer,
    // and set the aids to send to.
    REPLACE void createRacePacket();

    // 0x80657e30
    // the patch patches the race packet. intention is for it to be called once a frame
    REPLACE void sendRacePacket();
    // when settings are implemented, to turn mkw-server off, we just call the original function
    void REPLACED(sendRacePacket)();

    // Sends the i-th unique packet
    bool sendRacePacketToMKWServer(u8 packetIdx);

    // 0x80658de0
    // Retreives/stores new MatchMakingInfo values and vr/br
    // Hooked to call recvFromRoomManager()
    REPLACE void updateMatchMakingInfoAndRating();
    void REPLACED(updateMatchMakingInfoAndRating)();

    // 0x80659170
    // Gets called upon entering match making connection state setting the room type to a public
    // room. This function is replaced with mkw-server's public room joining
    REPLACE void connectToAnybodyAsync();

    // 0x80659680
    // Called when joining a friends public room. replaced to implement mkw-server match making
    REPLACE void connectToGameServerFromGroupId();

    // 0x80659a84
    REPLACE void processRacePacket(u8 aid, u8 *header, u32 size);

    // 0x80659fa4
    FriendJoinableStatus getFriendJoinableStatus(u32 friendId);

    // 0x8065a8d4
    // Callback thats evoked to update whether or not your friends have added you back. Hooked to
    // inform wfc-server of the local player count. This funciton was chosen for this hook because
    // the login session was just established and this function gets called once a session.
    REPLACE void updateAddedFriendsCallback(void *r3, void *r4, void *r5);
    void REPLACED(updateAddedFriendsCallback)(void *r3, void *r4, void *r5);

    // Two vtables
    void *m_vtable1; // offset 0xc is NetManager's dtor
    void *m_vtable2; // unk dtor at 0xc, also present in FriendManager vtable
    OSMutex m_mutex;
    EGG::ExpHeap *m_heap;
    EGG::TaskThread *m_taskThread; // runs the mainLoop
    ConnectionState m_connectionState;
    DisconnectInfo m_disconnectInfo;
    u8 _0034[0x0038 - 0x0034];             // padding
    MatchMakingInfo m_matchMakingInfos[2]; // 0x0038 - 0x00e8
    RoomType m_roomType;
    VoteMatchMakingSuspended m_voteMMSuspension;
    // points to Race packets to be sent, two per aid / 0xf0
    RacePacketHolder *m_sendRacePackets[2][MAX_PLAYER_COUNT];
    // points to Race packets to be recieved, two per aid / 0x150
    RacePacketHolder *m_recvRacePackets[2][MAX_PLAYER_COUNT];
    // The Race packet to be sent, formed from m_sendRacePackets, one per aid /
    // 0x1b0
    RecordHolder<Header> *m_outgoingRacePacket[MAX_PLAYER_COUNT];
    OSTime m_timeOfLastSentRace[MAX_PLAYER_COUNT];        // 0x1e0
    OSTime m_timeOfLastRecvRace[MAX_PLAYER_COUNT];        // 0x240
    OSTime m_timeBetweenSendingPackets[MAX_PLAYER_COUNT]; // time bewteen sent
                                                          // packets per aid / 0x2a0
    OSTime m_timeBetweenRecvPackets[MAX_PLAYER_COUNT];    // time between recieved
                                                          // packets per aid / 0x300
    u8 m_aidLastSentTo;                                   // Aid of last player we sent to / 0x360
    u8 m_recvRacePacketBuffer[MAX_PLAYER_COUNT][0x2e0];   // 0x361
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
    u32 m_lastSendIdx[MAX_PLAYER_COUNT]; // idx of m_sendRacePackets last sent per
                                         // aid
    // idx of m_recvRacePackets last recvieved per packet per aid
    u32 m_lastRecvIdx[MAX_PLAYER_COUNT][8];      // 0x279c
    u32 m_currMMInfo;                            // Current MM info used 0x291c
    u8 m_playerIdToAidMapping[MAX_PLAYER_COUNT]; // 0x2920
    u32 m_disconnectedAids;                      // disconnected if 1 << aid is 1 // 0x292c
    u32 m_disconnectedPlayerIds;                 // disconnected if 1 << pid is 1 // 0x2930
    u8 _2934[0x295c - 0x2934];                   // elo based MM struct
    u8 _295c[0x29c8 - 0x295c];                   // some timers

    SP::OutgoingRacePackets m_outgoingUniquePackets; // added. TODO: Replace m_outgoingRacePacket
    static NetManager *s_instance;
};
// TODO: Idk why the + sizeof(u32) is needed, but it is
static_assert(sizeof(NetManager) == (0x29c8 + sizeof(SP::OutgoingRacePackets) + sizeof(size_t)));

} // namespace Net
