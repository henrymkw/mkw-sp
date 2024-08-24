#pragma once

#include "game/ui/Page.hh"

namespace UI {

/**
 * @brief PageId: 0x95, not used anymore by SP. I assume this is match making after online drift
 * select? Not sure
 *
 */
class GlobePage : public Page {
public:
    void requestSpinFar();
    void requestSpinMid();
    void requestSpinClose();

private:
    u8 _044[0x4c8 - 0x044];
};
static_assert(sizeof(GlobePage) == 0x4c8);

} // namespace UI
