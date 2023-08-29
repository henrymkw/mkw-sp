#include "SettingsOptionsPage.hh"

namespace UI {

SettingsOptionsPage::SettingsOptionsPage() = default;

SettingsOptionsPage::~SettingsOptionsPage() = default;

void SettingsOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(7);
    insertChild(0, &m_optionButton0, 0);
    insertChild(1, &m_optionButton1, 0);
    insertChild(2, &m_optionButton2, 0);
    insertChild(3, &m_optionButton3, 0);
    insertChild(4, &m_optionButton4, 0);
    insertChild(5, &m_blackBackControl, 0);
    insertChild(6, &m_backButton, 0);

    m_optionButton0.load("button", "SettingsOptionAndDescription", "Button4_0",
            0x1, false, false);
    m_optionButton1.load("button", "SettingsOptionAndDescription", "Button4_1",
            0x1, false, false);
    m_optionButton2.load("button", "SettingsOptionAndDescription", "Button4_2",
            0x1, false, false);
    m_optionButton3.load("button", "SettingsOptionAndDescription", "Button4_3",
            0x1, false, false);
    m_optionButton4.load("button", "SettingsOptionAndDescription", "Button4_4",
            0x1, false, false);
    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;
    m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);

    m_optionButton0.selectDefault(0);
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
