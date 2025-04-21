#include "VehiclePhysics.hh"

namespace Kart {

const Vec3 *VehiclePhysics::externalVel() const {
    return &m_externalVel;
}

const Vec3 *VehiclePhysics::internalVel() const {
    return &m_internalVel;
}

const Quat *VehiclePhysics::mainRot() const {
    return &m_mainRot;
}

const Vec3 *VehiclePhysics::movingRoadVel() const {
    return &m_movingRoadVel;
}

const Vec3 *VehiclePhysics::movingWaterVel() const {
    return &m_movingWaterVel;
}

const Vec3 *VehiclePhysics::pos() const {
    return &m_pos;
}

void VehiclePhysics::setExternalVel(Vec3 vel) {
    m_externalVel = vel;
}

void VehiclePhysics::setInternalVel(Vec3 vel) {
    m_internalVel = vel;
}

void VehiclePhysics::setMainRot(Quat rot) {
    m_mainRot = rot;
}

void VehiclePhysics::setPos(Vec3 pos) {
    m_pos = pos;
}

} // namespace Kart
