#pragma once

#include <Common.hh>

namespace Enemy {
class KartEnemy10 {
public:
    REPLACE void unk();

    void REPLACED(unk)();

private:
    u8 _00[0x60 - 0x00];
};
static_assert(sizeof(KartEnemy10) == 0x60);

} // namespace Enemy