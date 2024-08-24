#pragma once

#include <Common.hh>

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageId: 0x81
 *
 */
class MultiKartSelectPage : public MenuPage {
private:
    u8 _0430[0x4280 - 0x0430];
};
static_assert(sizeof(MultiKartSelectPage) == 0x4280);

} // namespace UI