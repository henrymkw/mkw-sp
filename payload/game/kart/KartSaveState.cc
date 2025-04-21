#include "KartSaveState.hh"

#include "game/kart/KartBullet.hh"

#include <cstring>

namespace Kart {

KartSaveState::KartSaveState(KartAccessor accessor, VehiclePhysics *physics, KartItem *item) {
    save(accessor, physics, item);
}

void KartSaveState::save(KartAccessor accessor, VehiclePhysics *physics, KartItem *item) {
    SavedFields savedFields;
    savedFields.m_externalVel = physics->m_externalVel;
    savedFields.m_internalVel = physics->m_internalVel;
    savedFields.m_mainRot = physics->m_mainRot;
    savedFields.m_pos = physics->m_pos;

    savedFields.m_internalSpeed = accessor.move->m_internalSpeed;
    savedFields.m_boostState.m_types = accessor.move->m_boost.m_types;
    savedFields.m_boostState.m_boostMultipler = accessor.move->m_boost.m_boostMultipler;
    savedFields.m_boostState.m_boostAcceleration = accessor.move->m_boost.m_boostAcceleration;
    savedFields.m_boostState.m_1c = accessor.move->m_boost.m_1c;
    savedFields.m_boostState.m_boostSpeedLimit = accessor.move->m_boost.m_boostSpeedLimit;
    savedFields.m_up = accessor.move->m_up;
    savedFields.m_dir = accessor.move->m_dir;

    savedFields.m_airtime = accessor.state->m_airtime;

    m_item = *item;

    for (u8 i = 0; i < accessor.settings->tireCount; i++) {
        savedFields.m_wheelPhysics[i].m_realPos = accessor.tire[i]->m_wheelPhysics->m_realPos;
        savedFields.m_wheelPhysics[i].m_lastPos = accessor.tire[i]->m_wheelPhysics->m_lastPos;
        savedFields.m_wheelPhysics[i].m_lastPosDiff =
                accessor.tire[i]->m_wheelPhysics->m_lastPosDiff;
    }

    m_kartSaveStateBuffer.push_front(std::move(savedFields));
}

void KartSaveState::reload(KartAccessor accessor, VehiclePhysics *physics, KartItem *item) {
    if (doesSaveStateExist()) {
        const SavedFields &savedFields = *m_kartSaveStateBuffer.front();

        physics->m_externalVel = savedFields.m_externalVel;
        physics->m_internalVel = savedFields.m_internalVel;
        physics->m_mainRot = savedFields.m_mainRot;
        physics->m_pos = savedFields.m_pos;

        accessor.move->m_internalSpeed = savedFields.m_internalSpeed;
        accessor.move->m_boost.m_types = savedFields.m_boostState.m_types;
        accessor.move->m_boost.m_boostMultipler = savedFields.m_boostState.m_boostMultipler;
        accessor.move->m_boost.m_boostAcceleration = savedFields.m_boostState.m_boostAcceleration;
        accessor.move->m_boost.m_1c = savedFields.m_boostState.m_1c;
        accessor.move->m_boost.m_boostSpeedLimit = savedFields.m_boostState.m_boostSpeedLimit;
        accessor.move->m_up = savedFields.m_up;
        accessor.move->m_dir = savedFields.m_dir;

        accessor.state->m_airtime = savedFields.m_airtime;

        *item = m_item;

        for (u8 i = 0; i < accessor.settings->tireCount; i++) {
            accessor.tire[i]->m_wheelPhysics->m_realPos = savedFields.m_wheelPhysics[i].m_realPos;
            accessor.tire[i]->m_wheelPhysics->m_lastPos = savedFields.m_wheelPhysics[i].m_lastPos;
            accessor.tire[i]->m_wheelPhysics->m_lastPosDiff =
                    savedFields.m_wheelPhysics[i].m_lastPosDiff;
        }

        m_kartSaveStateBuffer.pop_front();
    }
}

bool KartSaveState::doesSaveStateExist() {
    return !m_kartSaveStateBuffer.empty();
}

u32 KartSaveState::getBufferSize() {
    return m_kartSaveStateBuffer.count();
}

void KartSaveState::addFrameToBuffer(KartAccessor accessor, VehiclePhysics *physics) {
    SavedFields savedFields;
    savedFields.m_externalVel = physics->m_externalVel;
    savedFields.m_internalVel = physics->m_internalVel;
    savedFields.m_mainRot = physics->m_mainRot;
    savedFields.m_pos = physics->m_pos;

    savedFields.m_internalSpeed = accessor.move->m_internalSpeed;
    savedFields.m_boostState.m_types = accessor.move->m_boost.m_types;
    savedFields.m_boostState.m_boostMultipler = accessor.move->m_boost.m_boostMultipler;
    savedFields.m_boostState.m_boostAcceleration = accessor.move->m_boost.m_boostAcceleration;
    savedFields.m_boostState.m_1c = accessor.move->m_boost.m_1c;
    savedFields.m_boostState.m_boostSpeedLimit = accessor.move->m_boost.m_boostSpeedLimit;
    savedFields.m_up = accessor.move->m_up;
    savedFields.m_dir = accessor.move->m_dir;

    savedFields.m_airtime = accessor.state->m_airtime;

    

    for (u8 i = 0; i < accessor.settings->tireCount; i++) {
        savedFields.m_wheelPhysics[i].m_realPos = accessor.tire[i]->m_wheelPhysics->m_realPos;
        savedFields.m_wheelPhysics[i].m_lastPos = accessor.tire[i]->m_wheelPhysics->m_lastPos;
        savedFields.m_wheelPhysics[i].m_lastPosDiff =
                accessor.tire[i]->m_wheelPhysics->m_lastPosDiff;
    }

    m_kartSaveStateBuffer.push_front(std::move(savedFields));
}

void KartSaveState::rewindFrame(KartAccessor accessor, VehiclePhysics *physics) {
    if (doesSaveStateExist() && m_kartSaveStateBuffer.count() > 1) {
        const SavedFields &savedFields = *m_kartSaveStateBuffer.front();

        physics->m_externalVel = savedFields.m_externalVel;
        physics->m_internalVel = savedFields.m_internalVel;
        physics->m_mainRot = savedFields.m_mainRot;
        physics->m_pos = savedFields.m_pos;

        accessor.move->m_internalSpeed = savedFields.m_internalSpeed;
        accessor.move->m_boost.m_types = savedFields.m_boostState.m_types;
        accessor.move->m_boost.m_boostMultipler = savedFields.m_boostState.m_boostMultipler;
        accessor.move->m_boost.m_boostAcceleration = savedFields.m_boostState.m_boostAcceleration;
        accessor.move->m_boost.m_1c = savedFields.m_boostState.m_1c;
        accessor.move->m_boost.m_boostSpeedLimit = savedFields.m_boostState.m_boostSpeedLimit;
        accessor.move->m_up = savedFields.m_up;
        accessor.move->m_dir = savedFields.m_dir;

        accessor.state->m_airtime = savedFields.m_airtime;

        for (u8 i = 0; i < accessor.settings->tireCount; i++) {
            accessor.tire[i]->m_wheelPhysics->m_realPos = savedFields.m_wheelPhysics[i].m_realPos;
            accessor.tire[i]->m_wheelPhysics->m_lastPos = savedFields.m_wheelPhysics[i].m_lastPos;
            accessor.tire[i]->m_wheelPhysics->m_lastPosDiff =
                    savedFields.m_wheelPhysics[i].m_lastPosDiff;
        }

        m_kartSaveStateBuffer.pop_front();
    }
}

} // namespace Kart
