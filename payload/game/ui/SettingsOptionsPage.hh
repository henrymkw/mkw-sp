#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class SettingsOptionsPage : public Page {
public:
    SettingsOptionsPage();
    ~SettingsOptionsPage() override;

    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onOptionButtonFront(PushButton *button, u32 localPlayerId);

    MultiControlInputManager m_inputManager;

    BlackBackControl m_blackBackControl;
    LayoutUIControl m_instructionText;
    LayoutUIControl m_settingTitleText;

    u32 m_chosen;
    // CtrlMenuInstructionText m_instructionText;
    //  UI Elements
    PushButton m_options[5];

    CtrlMenuBackButton m_backButton;

    template <typename T>
    using H = typename T::template Handler<SettingsOptionsPage>;
    H<MultiControlInputManager> m_onBack{this, &SettingsOptionsPage::onBack};
    H<PushButton> m_onBackButtonFront{this, &SettingsOptionsPage::onBackButtonFront};
    H<PushButton> m_onOptionButtonFront{this, &SettingsOptionsPage::onOptionButtonFront};
};

} // namespace UI
