#pragma once

#include "game/kart/KartObject.hh"

extern f32 g_speedModFactor;

namespace Kart {

// NOTE: This class inherits Disposer
class KartObjectManager {
public:
    KartObjectManager();
    virtual ~KartObjectManager();

    KartObject *object(u32 playerId);

    void beforeCalc();
    void calc();

    REPLACE static KartObjectManager *CreateInstance();
    static KartObjectManager *REPLACED(CreateInstance)();

    static KartObjectManager *Instance() {
        return s_instance;
    }

private:
    u8 _00[0x20 - 0x04];
    KartObject **m_objects;
    u8 m_count;
    u8 _25[0x38 - 0x25];

    static KartObjectManager *s_instance;
};

static_assert(sizeof(KartObjectManager) == 0x38);

} // namespace Kart
