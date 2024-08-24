#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageId: 0x72. Used to select between team and vs mode.
 */
class VSModeSelectPage : public MenuPage {
private:
    u8 _430[0x6c4 - 0x430];
};
static_assert(sizeof(BattleModeSelectPage) == 0x6c4);

} // namespace UI