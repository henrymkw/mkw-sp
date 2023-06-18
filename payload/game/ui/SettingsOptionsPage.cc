#include "SettingsOptionsPage.hh"

namespace UI {

SettingsOptionsPage::SettingsOptionsPage() = default;

SettingsOptionsPage::~SettingsOptionsPage() = default;

void SettingsOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(2);
    insertChild(0, &m_optionButton, 0);
    insertChild(1, &m_blackBackControl, 0);
    
    m_optionButton.load("button", "SettingsOptionAndDescription", "OptionButtonAndDescription0", 0x1, false, false);
    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;

    m_optionButton.selectDefault(0);
}

void SettingsOptionsPage::onActivate() {
    
}

} // namespace UI
