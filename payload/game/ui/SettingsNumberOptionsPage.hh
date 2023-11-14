#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class SettingsNumberOptionsPage : public Page {
public:
    SettingsNumberOptionsPage();
    ~SettingsNumberOptionsPage() override;

    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onOptionButtonFront(PushButton *button, u32 localPlayerId);
    void onOptionButtonSelect(PushButton *button, u32 localPlayerId);
    void refresh();

    MultiControlInputManager m_inputManager;

    BlackBackControl m_blackBackControl;
    LayoutUIControl m_instructionText;
    LayoutUIControl m_settingTitleText;

    PushButton m_options[30];
    PushButton m_arrowLeft;
    PushButton m_arrowRight;

    // m_selectedValue is the selected setting by its value
    u32 m_selectedValue;
    // m_selectedIndex is the index of m_selectedValue in m_options
    u32 m_selectedIndex;
    u32 m_numSheets;
    u32 m_currSheet;

    CtrlMenuBackButton m_backButton;

    template <typename T>
    using H = typename T::template Handler<SettingsNumberOptionsPage>;

    H<MultiControlInputManager> m_onBack{this, &SettingsNumberOptionsPage::onBack};
    H<PushButton> m_onBackButtonFront{this, &SettingsNumberOptionsPage::onBackButtonFront};
    H<PushButton> m_onOptionButtonFront{this, &SettingsNumberOptionsPage::onOptionButtonFront};
    H<PushButton> m_onOptionButtonSelect{this, &SettingsNumberOptionsPage::onOptionButtonSelect};
};

} // namespace UI
