#pragma once

#include <Common.hh>

namespace Net {

class RACEHEADER1Handler {
public:
    struct RACEHEADER1Packet {
        u32 elapsedTimeSinceRaceStart;
        u32 seed;
        u8 _08[0x0c - 0x08];
        u16 _unkC;
        u8 _0e[0x17 - 0x0e];
        u8 playerType; // 0 for regular player, 2 for spectator
        u8 _18[0x28 - 0x18];
    };
    static_assert(sizeof(RACEHEADER1Packet) == 0x28);
    struct RACEHEADER1Data {
        u32 _00;
        u8 _04[0x14 - 0x04];
        // This can take on values outside of the current courseId enum, maybe they're
        // different types?
        // System::CourseId courseId;
        u32 courseId;
        u8 _18[0x20 - 0x18];
        u8 playerIdToAidMapping[12];
        u8 _2c[0x30 - 0x2c];
    };
    static_assert(sizeof(RACEHEADER1Data) == 0x30);

private:
    bool m_prepared;
    u8 _001[0x008 - 0x001];
    u32 m_unk8;
    u8 _00c[0x020 - 0x00c];
    RACEHEADER1Data m_RH1Datas[12];

    static RACEHEADER1Handler *s_instance;
};
static_assert(sizeof(RACEHEADER1Handler) == 0x260);

} // namespace Net
