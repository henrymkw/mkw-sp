#pragma once

#include "game/kart/KartObjectProxy.hh"
#include "game/kart/KartSub.hh"

namespace Kart {

class KartObject : public KartObjectProxy {
public:
    void init(); // 0x8058e22c

    void calcEarly(); // 0x8058eeb4

    void calcLate(); // 0x8058eebc

private:
    u8 _0x0c[0x10 - 0x0c];
    KartSub *m_sub;
    KartSettings *m_settings;
    u8 _0x18[0x1c - 0x18];

public:
    KartAccessor m_accessor;
};
static_assert(sizeof(KartObject) == 0x80);

} // namespace Kart
