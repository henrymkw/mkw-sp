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
 * RaceScene exit. Its main logic is is ran in its calc(), but the business
 * logic will only run when m_isPrepared is true.
 *
 */
class MiscPacketHandler {
private:
    bool m_isPrepared;
    bool m_unk1;
    u32 m_unkBitfield_4;
    u32 m_unkBitfield_8;
    u32 m_unkBitfield_C;
    u16 m_unk10;
    u16 m_unk12;
    RaceDataRecord m_sendRaceDataRecords[2]; // 0x14 - 0x94
    EventRecord m_sendEventRecords;          // 0x94 - 0x18c
    u8 _18c[0x190 - 0x18c];
    OSTime m_elapsedTimeSinceFriendStatusUpdated;
    u32 m_aidsTimeSinceRaceStart[MAX_PLAYER_COUNT];

    static MiscPacketHandler *s_instance;
};
static_assert(sizeof(MiscPacketHandler) == 0x1c8);

} // namespace Net
