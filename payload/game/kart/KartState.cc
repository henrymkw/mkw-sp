#include "KartState.hh"

namespace Kart {

bool KartState::inCannon() const {
    return m_inCannon;
}

bool KartState::touchingGround() const {
    return m_touchingGround;
}

bool KartState::hopStart() const {
    return m_hopStart;
}

bool KartState::hop() const {
    return m_hop;
}

} // namespace Kart
