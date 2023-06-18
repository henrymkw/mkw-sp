#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
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
    MultiControlInputManager m_inputManager;

    BlackBackControl m_blackBackControl;

    // UI Elements
    PushButton m_optionButton;
};

} // namespace UI
