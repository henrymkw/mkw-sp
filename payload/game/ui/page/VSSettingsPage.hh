#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageId: 0x73, page of the versus settings menu. Replaced by SP's settings
 *
 */
class VSSettingsPage : public MenuPage {
private:
    u8 _0430[0x1e10 - 0x0430];
};
static_assert(sizeof(VSSettingsPage) == 0x1e10);

} // namespace UI
