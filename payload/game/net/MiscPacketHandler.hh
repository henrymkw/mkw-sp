#pragma once

#include <Common.hh>

#include "game/net/records/Event.hh"
#include "game/net/records/RH1.hh"
#include "game/net/records/RH2.hh"
#include "game/net/records/RaceData.hh"
#include "game/net/records/Select.hh"

extern "C" {
#include <revolution/os.h>
}

namespace Net {

/*
 * MiscPacketHandler gets created when switching to RaceScene and destroyed upon
 * RaceScene exit. Has a number of misc jobs related to packets (and some not). Imports, exports,
 * and clears a number of records. Handles frame lag, timer differences, and disconnect logic.
 * Also has ties with Event and Item records
 */
class MiscPacketHandler {
private:
    // 0x80653728
    // Its main logic is is ran in its update(), but the business
    // logic will only run when m_isPrepared is true. This runs in online races but
    // also rival ghost races to update friend status
    void update();

    // 0x80654150
    // calls bellow functions as mentioned, but manages the start of a race also by making
    // decisions based off the timers of other players, voting to suspend/unsuspend match making
    // calls ItemHandler::update(), EventHandler::update(), and processLagFrames()
    REPLACE void updateAsRacer();
    void REPLACED(updateAsRacer)();

    // 0x80654d08
    // called by updateAsRacer(). only RH1 record gets created, both get exported to the send buffer
    void createExportRH1ExportRaceData();

    // 0x806554a0
    // called by updateAsRacer(). unpacks all players RH1 records. can also set m_scheduleDisconnect
    void processRecvRH1Records();

    // 0x80654038
    // called by updateAsRacer() to begin the race countdown. the countdown can start once everyone
    // is in the race.
    REPLACE bool isEveryoneInRace() const;
    bool REPLACED(isEveryoneInRace)() const;

    // 0x80654b00
    // tbd explanation, loops over recv RH1 records and compares other players lag frames with ours
    // if their lag frames are greater than ours, then set GameScene[0x2539] (maybe called
    // skipFrame?) this causes GameScene::update() to call beginFrame() and endFrame(), which I'd
    // assume skips a frame? also takes framerate into account, so it would skip two frames at
    // 30fps.
    void processLagFrames();

    // 0x806544a8
    // returns 0 if playerId isn't in the room
    // returns aidTimeSinceRaceStart if in a race (?) and difference between
    // m_elapsedTimeSinceRaceStart and aid's time is less than 600 ms (not a bunch of)
    // returns m_elapsedTimeSinceRaceStart otherwise
    u32 getRH1Timer(s32 playerId) const;

    bool m_isPrepared;
    bool m_scheduleDisconnect; // triggers a dc when set
    u8 _002[0x004 - 0x002];
    u32 m_aidsWithRH1Seed;         // bitfield by aid, 1 when aid's seed is set
    u32 m_aidLastSentRoomOrSelect; // aid bitfield, 1 << aid for select, 0 for room

    // bitfield by aid, used to (partially) decide to dc/stop a player, but the conditions that get
    // it set are unclear because it gets set in updateBitfields() when RH1Record[0x17] != 0, but
    // that field is also unknown, making this one unknown.
    u32 m_shouldStopAid;
    s16 m_myLagFrames;   // refers to frames that didnt get rendered due to lag, gets updated in
                         // processLagFrames()
    s16 m_countdownTime; // set to 3000 ms,
    RaceDataRecord m_sendRaceDataRecords[2]; // 0x14 - 0x94
    EventRecord m_sendEventRecords;          // 0x94 - 0x18c
    u8 _18c[0x190 - 0x18c];
    // last time we updated our friends statuses. updated about once every 10 seconds
    OSTime m_elapsedTimeSinceFriendStatusUpdated;
    u32 m_aidsTimeSinceRaceStart[MAX_PLAYER_COUNT];

    static MiscPacketHandler *s_instance;
};
static_assert(sizeof(MiscPacketHandler) == 0x1c8);

} // namespace Net
