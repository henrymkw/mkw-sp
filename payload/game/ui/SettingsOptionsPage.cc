#include "SettingsOptionsPage.hh"

namespace UI {

SettingsOptionsPage::SettingsOptionsPage() = default;

SettingsOptionsPage::~SettingsOptionsPage() = default;

void SettingsOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(3);
    insertChild(0, &m_optionButton, 0);
    insertChild(1, &m_blackBackControl, 0);
    insertChild(2, &m_backButton, 0);

    m_optionButton.load("button", "SettingsOptionAndDescription", "OptionButtonAndDescription0",
            0x1, false, false);
    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;
    m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);

    m_optionButton.selectDefault(0);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
}

void SettingsOptionsPage::onActivate() {}

void SettingsOptionsPage::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsOptionsPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId*/) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

} // namespace UI
