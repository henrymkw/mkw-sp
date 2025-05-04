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

    enum class BitField0 : u32 {
        ACCELERATE = 0x1,
        BRAKE = 0x2,
        DRIFT_INPUT = 0x4,
        DRIFT_MANUAL = 0x8,
        BEFORE_RESPAWN = 0x10,
        WALL_3_COLLISION = 0x20,
        WALL_COLLISION = 0x40,
        HOP_STATE = 0x80,
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

    enum class BitField1 : u32 {
        IN_ACTION = 0x1,
        TRIGGER_RESPAWN = 0x2,
        CANNON_START = 0x8,
        IN_CANNON = 0x10,
        TRICK_START = 0x20,
        IN_A_TRICK = 0x40,
        BOOST_OFFROAD_INVINCIBILITY = 0x80,
        ON_HALF_PIPE_RAMP = 0x200,
        OVER_ZIPPER = 0x400,
        JUMP_PAD_MUSHROOM_COLLISION = 0x800,
        UNK_1000 = 0x1000,
        UNK_1600 = 0x1600,
        ZIPPER_BOOST = 0x2000,
        ZIPPER_TRICK = 0x8000,
        UNK_10000 = 0x10000,
        BURNOUT = 0x40000,
        AT_SUSP_LIMIT = 0x80000,
        MT_BOOST = 0x100000,
        HAS_TRICK_ROT = 0x400000,
        BOUNCY_MUSHROOM_VELY_INCREASE = 0x800000,
        REJECT_ROAD = 0x8000000,
        UNK_10000000 = 0x10000000,
        TRICKABLE = 0x40000000,
        IN_A_STAR = 0x80000000,
    };

    enum class BitField2 : u32 {
        CHARGING_A_SLIPSTREAM = 0x1,
        IN_A_SLIPSTREAM = 0x2,
        SOMETHING_WHEELIE_BUMP = 0x4,
        ON_HALF_PIPE_RAMP_LOCAL = 0x8,
        WHEELIE_ROT = 0x10,
        SKIP_WHEEL_CALC = 0x20,
        JUMP_PAD_MUSHROOM_TRIGGER = 0x40,
        SHOCKED = 0x80,
        UNK_100 = 0x100,
        MOVING_WATER_STRONG_CURRENT = 0x200,
        MOVING_WATER_V2 = 0x400,
        NO_SPARK_INVISIBLE_WALL = 0x800,
        IN_RESPAWN = 0x2000,
        AFTER_RESPAWN = 0x4000,
        IN_A_MEGA = 0x8000,
        CRUSHED = 0x10000,
        HAS_STOPPED = 0x40000,
        HAS_VANISHED = 0x80000,
        JUMP_PAD_FIXED_SPEED = 0x100000,
        MOVING_WATER_NOTV2 = 0x200000,
        JUMP_PAD_DISABLE_Y_SUS_FORCE = 0x400000,
        HALFPIPE_MIDAIR = 0x800000,
        PASS_THROUGH_COURSE_OBJECTS = 0x1000000,
        OBJECT_INVISIBILITY_MAYBE = 0x2000000,
        IN_A_BULLET = 0x8000000,
        INK_APPLIED = 0x10000000,
        IN_A_TC = 0x20000000,
    };

    enum class BitField3 : u32 {
        UNK_2 = 0x2,
        SOMETHING_WALL_COLLISION = 0x8,
        SOFT_WALL_DRIFT = 0x10,
        HWG = 0x20,
        AFTER_CANNON = 0x40,
        CHARGE_START_BOOST = 0x100,
        MOVING_WATER_V3 = 0x200,
        UNK_400 = 0x400,
        END_HALF_PIPE = 0x800,
    };

    enum class BitField4 : u32 {
        CPU = 0x1,
        LOCAL = 0x2,
        ONLINE_LOCAL = 0x4,
        ONLINE_REMOTE = 0x8,
        AUTOMATIC_DRIFT = 0x10,
        GHOST = 0x40,
        BATTLE_RESPAWN = 0x80,
        SET_SPEED_ZERO = 0x200,
        DEMO_LOSS = 0x800,
        HAS_STOPPED = 0x1000,
        BEFORE_BATTLE_RESPAWN = 0x20000,
    };

private:
    u8 _00[0x04 - 0x00];
    u32 : 27;
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
