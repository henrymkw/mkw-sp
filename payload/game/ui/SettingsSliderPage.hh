#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ScrollBar.hh"

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

    MultiControlInputManager m_inputManager;

    BlackBackControl m_blackBackControl;
    LayoutUIControl m_instructionText;

    // TODO: Fields for this specific page, such as a chosen
    ScrollBar m_scrollBar;

    template <typename T>
    using H = typename T::template Handler<SettingsSliderPage>;

    H<MultiControlInputManager> m_onBack{this, &SettingsSliderPage::onBack};
    H<PushButton> m_onBackButtonFront{this, &SettingsSliderPage::onBackButtonFront};
};

} // namespace UI
