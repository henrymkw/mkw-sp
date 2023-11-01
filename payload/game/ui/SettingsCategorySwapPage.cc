#include "SettingsCategorySwapPage.hh"

#include <Common.hh>

// #include "game/system/SaveManager.hh"
// #include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
// #include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "sp/settings/ClientSettings.hh"

#include <cstdio>

namespace UI {

SettingsCategorySwapPage::SettingsCategorySwapPage() = default;

SettingsCategorySwapPage::~SettingsCategorySwapPage() = default;

void SettingsCategorySwapPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(14);
    for (u8 i = 0; i < 10; i++) {
        insertChild(i, &m_categories[i], 0);
    }

    insertChild(10, &m_backButton, 0);
    insertChild(11, &m_blackBackControl, 0);
    insertChild(12, &m_arrowLeft, 0);
    insertChild(13, &m_arrowRight, 0);

    u8 buttonId = 0;
    for (u8 i = 0; i < 5; i++) {
        for (u8 j = 0; j < 2; j++) {
            char variant[18];
            snprintf(variant, sizeof(variant), "CategoryButton%hhu_%hhu", i, j);
            m_categories[buttonId].load("button", "CategorySwapPage", variant, 0x1, false, false);
            buttonId++;
        }
    }
    m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);

    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;

    m_arrowLeft.load("button", "NumberMenuArrowLeft", "ButtonArrowLeft", 0x1, false, true);
    m_arrowRight.load("button", "NumberMenuArrowRight", "ButtonArrowRight", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    for (u8 i = 0; i < 10; i++) {
        m_categories[i].setFrontHandler(&m_onButtonFront, false);
        m_categories[i].m_index = i;
    }

    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_backButton.m_index = 10;

    m_arrowLeft.setFrontHandler(&m_onButtonFront, false);
    m_arrowRight.setFrontHandler(&m_onButtonFront, false);
    m_arrowLeft.m_index = 11;
    m_arrowRight.m_index = 12;

    m_categories[0].selectDefault(0);
}

void SettingsCategorySwapPage::onActivate() {
    for (u8 i = 0; i < 10; i++) {
        // TODO: Fix the hardcode
        if (i >= 9) {
            m_categories[i].setVisible(false);
            m_categories[i].setMessageAll(6602);
            continue;
        }
        m_categories->setVisible(true);
        m_categories[i].setMessageAll(SP::ClientSettings::categoryMessageIds[i]);
    }
}

void SettingsCategorySwapPage::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsCategorySwapPage::onBackButtonFront(PushButton *button, u32 /*localPlayerId*/) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void SettingsCategorySwapPage::onButtonFront(PushButton *button, u32 /*localPlayerId*/) {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    settingsPage->clearMessageLists();
    settingsPage->getCategoryInfo2(button->m_index);
    settingsPage->setCategoryValues(button->m_index);
    settingsPage->setButtons();
}

} // namespace UI
