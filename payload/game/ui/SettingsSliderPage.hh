#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/ScrollBar.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class SettingsSliderPage : public Page {
public:
    SettingsSliderPage();
    ~SettingsSliderPage() override;

    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onScrollBarChange(ScrollBar *scrollBar, u32 localPlayerId, u32 chosen);

    MultiControlInputManager m_inputManager;

    BlackBackControl m_blackBackControl;
    LayoutUIControl m_instructionText;
    LayoutUIControl m_settingTitleText;
    CtrlMenuBackButton m_backButton;
    LayoutUIControl m_option;

    ScrollBar m_scrollBar;

    u32 m_numOptions;
    u32 m_chosen;

    template <typename T>
    using H = typename T::template Handler<SettingsSliderPage>;

    H<MultiControlInputManager> m_onBack{this, &SettingsSliderPage::onBack};
    H<PushButton> m_onBackButtonFront{this, &SettingsSliderPage::onBackButtonFront};
    H<ScrollBar> m_onScrollBarChange{this, &SettingsSliderPage::onScrollBarChange};
};

} // namespace UI
