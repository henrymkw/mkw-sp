#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"

namespace UI {

class SettingsCategorySwapPage : public Page {
public:
    SettingsCategorySwapPage();
    ~SettingsCategorySwapPage() override;

    void onInit() override;
    void onActivate() override;

private:
    void onBack(u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonFront(PushButton *button, u32 localPlayerId);
    // This is for when you press start. Same functino as onBack, different button
    void onClose(u32 localPlayerId);

    MultiControlInputManager m_inputManager;

    BlackBackControl m_blackBackControl;
    LayoutUIControl m_instructionText;

    u32 m_chosen;
    PushButton m_categories[10];

    PushButton m_arrowLeft;
    PushButton m_arrowRight;

    CtrlMenuBackButton m_backButton;

    template <typename T>
    using H = typename T::template Handler<SettingsCategorySwapPage>;

    H<MultiControlInputManager> m_onBack{this, &SettingsCategorySwapPage::onBack};
    H<MultiControlInputManager> m_onClose{this, &SettingsCategorySwapPage::onClose};
    H<PushButton> m_onBackButtonFront{this, &SettingsCategorySwapPage::onBackButtonFront};
    H<PushButton> m_onButtonFront{this, &SettingsCategorySwapPage::onButtonFront};
};

} // namespace UI
