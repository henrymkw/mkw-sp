#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageId: 0x60
 *
 */
class MiiSelectPage : public MenuPage {
private:
    u8 _430[0xf20 - 0x430];
};

static_assert(sizeof(MiiSelectPage) == 0xf20);

} // namespace UI