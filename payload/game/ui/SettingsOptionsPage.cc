#include "SettingsOptionsPage.hh"

namespace UI {

SettingsOptionsPage::SettingsOptionsPage() = default;

SettingsOptionsPage::~SettingsOptionsPage() = default;

void SettingsOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(1);
    insertChild(0, &m_optionButton, 0);

    m_optionButton.load("button", "SettingsOptionAndDescription", "OptionButtonAndDescription0", 0x1, false, false);

}

void SettingsOptionsPage::onActivate() {

    m_optionButton.selectDefault(0);
}

} // namespace UI
