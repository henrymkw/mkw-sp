// Handles storing and restoring as much state about the kart as needed
// to allow for faster dolphin-like save states on console for practicing.
//
// This class is only for low level management, further functionality should
// most likely be added to SP::SaveStateManager.

#pragma once

#include "game/item/KartItem.hh"
#include "game/kart/KartBoost.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/kart/KartTire.hh"
#include "game/kart/VehiclePhysics.hh"

#include <sp/CircularBuffer.hh>

extern "C" {
#include "game/item/ItemDirector.h"
}

#include <optional>
#include <tuple>

namespace Kart {

// KartBoost's constructor cannot be patched to replace
// the vtable, and therefore define a GCC destructor
// as it is inlined in KartMove.
struct PODKartBoost {
    s16 m_timesBeforeEnd[6];
    u16 m_types;
    f32 m_boostMultipler;
    f32 m_boostAcceleration;
    f32 m_1c;
    f32 m_boostSpeedLimit;
};

struct MinifiedWheelPhysics {
    Vec3 m_realPos;
    Vec3 m_lastPos;
    Vec3 m_lastPosDiff;
};

class KartSaveState {
public:
    KartSaveState(KartAccessor accessor, VehiclePhysics *physics, KartItem *item);

    void save(KartAccessor accessor, VehiclePhysics *physics, KartItem *item);
    void reload(KartAccessor accessor, VehiclePhysics *physics, KartItem *item);

    /**
     * @brief Check if a save state exists by checking if the buffer is not empty.
     */
    bool doesSaveStateExist();

    /**
     * @brief Add the current frame to the buffer. Takes in Kart values of a given frame.
     */
    void addFrameToBuffer(KartAccessor accessor, VehiclePhysics *physics);

    /**
     * @brief Sets the member fields of the parameters to the head of the buffer, then pops the
     * first element.
     */
    void rewindFrame(KartAccessor accessor, VehiclePhysics *physics);

    u32 getBufferSize();

private:
    struct SavedFields {
        // VehiclePhysics
        Vec3 m_externalVel;
        Vec3 m_internalVel;
        Quat m_mainRot;
        Vec3 m_pos;

        // KartMove
        f32 m_internalSpeed;
        PODKartBoost m_boostState;
        Vec3 m_up;
        Vec3 m_dir;

        // KartState
        u32 m_airtime;

        MinifiedWheelPhysics m_wheelPhysics[4];
    };

    SavedFields m_savedFields;

    KartItem m_item;

    SP::CircularBuffer<SavedFields, 900> m_kartSaveStateBuffer;

    bool m_firstReload = true;
};

} // namespace Kart
