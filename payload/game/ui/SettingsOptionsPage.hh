#pragma once

#include "game/ui/Page.hh"

namespace UI {

class SettingsOptionsPage : public Page {
public:
    SettingsOptionsPage();
    ~SettingsOptionsPage() override;

    void onInit() override;

private:
    MenuInputManager m_inputManager;
};

} // namespace UI
