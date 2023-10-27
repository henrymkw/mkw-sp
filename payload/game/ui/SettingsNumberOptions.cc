#include "SettingsNumberOptions.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

#include <cstdio>

namespace UI {

SettingsNumberOptionsPage::SettingsNumberOptionsPage() = default;

SettingsNumberOptionsPage::~SettingsNumberOptionsPage() = default;

void SettingsNumberOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(34);
    for (u8 i = 0; i < 30; i++) {
        insertChild(i, &m_options[i], 0);
    }

    insertChild(30, &m_backButton, 0);
    insertChild(31, &m_blackBackControl, 0);

    insertChild(32, &m_arrowLeft, 0);
    insertChild(33, &m_arrowRight, 0);

    u8 buttonId = 0;
    for (u8 i = 0; i < 5; i++) {
        for (u8 j = 0; j < 6; j++) {
            char variant[11];
            snprintf(variant, sizeof(variant), "Button%hhu_%hhu", i, j);
            m_options[buttonId].load("button", "NumberGridMenu", variant, 0x1, false, false);
            buttonId++;
        }
    }

    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;
    m_backButton.load("button", "Back", "BackButtonPopup", 0x1, false, true);

    m_arrowLeft.load("button", "NumberMenuArrowLeft", "ButtonArrowLeft", 0x1, false, true);
    m_arrowRight.load("button", "NumberMenuArrowRight", "ButtonArrowRight", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    for (u8 i = 0; i < 30; i++) {
        m_options[i].setFrontHandler(&m_onOptionButtonFront, false);
        m_options[i].m_index = i;
    }
}

void SettingsNumberOptionsPage::onActivate() {
    m_options[0].selectDefault(0);
    for (u8 i = 0; i < 30; i++) {
        m_options[i].setMessageAll(6602);
    }
}

void SettingsNumberOptionsPage::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsNumberOptionsPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void SettingsNumberOptionsPage::onOptionButtonFront(PushButton * /* button */,
        u32 /* localPlayerId */) {}

} // namespace UI