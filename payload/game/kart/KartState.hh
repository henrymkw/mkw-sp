#pragma once

#include <Common.hh>

namespace Kart {

class KartState {
    friend class KartMove;
    friend class KartRollback;
    friend class KartSaveState;
    friend class KartSub;

public:
    bool inCannon() const;

    bool hopStart() const;
    
    bool touchingGround() const;

    bool hop() const;

    enum class BitField0 : u32 {
        ACCELERATE = 0x1,
        BRAKE = 0x2,
        DRIFT_INPUT = 0x4,
        DRIFT_MANUAL = 0x8,
        BEFORE_RESPAWN = 0x10,
        WALL_3_COLLISION = 0x20,
        WALL_COLLISION = 0x40,
        HOP_START = 0x80,
        ACCELERATE_START = 0x100,
        GROUND_START = 0x200,
        VEHICLE_BODY_FLOOR_COLLISION = 0x400,
        ANY_WHEEL_COLLISION = 0x800,
        ANY_WHEELS_COLLISION = 0x1000,
        STICK_LEFT = 0x2000,
        WALL_COLLISION_START = 0x4000,
        AIRTIME_OVER_20 = 0x8000,
        STICKY_ROAD = 0x10000,
        UNKNOWN_20000 = 0x20000,
        TOUCHING_GROUND = 0x40000,
        HOP = 0x80000,
        BOOST = 0x100000,
        DISABLE_ACCELERATION = 0x400000,
        AIR_START = 0x800000,
        STICK_RIGHT = 0x1000000,
        LARGE_FLIP_HIT = 0x2000000,
        MUSHROOM_BOOST = 0x4000000,
        SLIPDRIFT_CHARGE = 0x8000000,
        DRIFT_AUTO = 0x10000000,
        WHEELIE = 0x20000000,
        JUMP_PAD = 0x40000000,
        RAMP_BOOST = 0x80000000,
    };

private:
    u8 _00[0x04 - 0x00];
    u32 : 12;
    bool m_hop : 1;
    bool m_touchingGround : 1;
    u32 : 10;
    bool m_hopStart : 1;
    u32 : 2;
    bool m_beforeRespawn : 1;
    u32 : 4;
    u32 : 23;
    bool m_blinking : 1;
    u32 : 3;
    bool m_inCannon : 1;
    u32 : 4;
    u8 _0c[0x14 - 0x0c];
    u32 : 31;
    bool m_isCpu : 1;
    u8 _18[0x1c - 0x18];
    u32 m_airtime;
    u8 _20[0xc0 - 0x20];
};
static_assert(sizeof(KartState) == 0xc0);

} // namespace Kart
