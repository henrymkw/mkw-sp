#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class SettingsLargeOptionsPage : public Page {
public:
    SettingsLargeOptionsPage();
    ~SettingsLargeOptionsPage() override;

    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonFront(PushButton *button, u32 localPlayerId);

    MultiControlInputManager m_inputManager;

    BlackBackControl m_blackBackControl;
    LayoutUIControl m_instructionText;
    LayoutUIControl m_settingTitleText;

    PushButton m_buttons[10];

    u32 m_chosen;
    // TODO: Find out if we need this
    PushButton m_arrowLeft;
    PushButton m_arrowRight;

    CtrlMenuBackButton m_backButton;

    template <typename T>
    using H = typename T::template Handler<SettingsLargeOptionsPage>;

    H<MultiControlInputManager> m_onBack{this, &SettingsLargeOptionsPage::onBack};
    H<PushButton> m_onBackButtonFront{this, &SettingsLargeOptionsPage::onBackButtonFront};
    H<PushButton> m_onButtonFront{this, &SettingsLargeOptionsPage::onButtonFront};
};

} // namespace UI
