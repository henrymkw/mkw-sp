#include "KartMove.hh"

#include "game/kart/KartObjectManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"

namespace Kart {

f32 KartMove::hardSpeedLimit() const {
    return m_hardSpeedLimit;
}

const Vec3 *KartMove::internalVelDir() const {
    return &m_internalVelDir;
}

} // namespace Kart
