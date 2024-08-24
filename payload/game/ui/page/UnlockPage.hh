#pragma once

#include <Common.hh>

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageId: 0xcd. This is the unlock screen.
 *
 */
class UnlockPage : public MenuPage {
private:
    u8 _430[0xcc0 - 0x430];
};
static_assert(sizeof(UnlockPage) == 0xcc0);

} // namespace UI