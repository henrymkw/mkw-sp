#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageID: 0x75, selecting between baloon and coin runners
 *
 */
class BattleModeSelectPage : public MenuPage {
private:
    BattleModeSelectPage();
    ~BattleModeSelectPage() override;

    void REPLACED(onInit)();
    REPLACE void onInit() override;
    REPLACE void onButtonFront(const PushButton *button);
    u8 _000[0x6c4 - 0x430];
};
static_assert(sizeof(BattleModeSelectPage) == 0x6c4);

} // namespace UI
