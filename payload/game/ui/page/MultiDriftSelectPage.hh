#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageId: 0x82, selecting drifts in multiplayer
 *
 */
class MultiDriftSelectPage : public MenuPage {
public:
    MultiDriftSelectPage();
    ~MultiDriftSelectPage() override;

    MultiControlInputManager *getInputManager() override;

    REPLACE void onButtonFront(PushButton *button, u32 localPlayerId);

private:
    u8 _430[0x6cc - 0x430];
    SectionId m_replacementSection;
};
static_assert(sizeof(MultiDriftSelectPage) == 0x6d0);

} // namespace UI
