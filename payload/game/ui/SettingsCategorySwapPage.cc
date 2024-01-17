#include "SettingsCategorySwapPage.hh"

#include <Common.hh>

#include "game/system/SaveManager.hh"
#include "game/ui/SettingsPage.hh"

#include "sp/settings/ClientSettings.hh"

#include <cstdio>

namespace UI {

SettingsCategorySwapPage::SettingsCategorySwapPage() = default;

SettingsCategorySwapPage::~SettingsCategorySwapPage() = default;

void SettingsCategorySwapPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(15);
    for (u8 i = 0; i < 10; i++) {
        insertChild(i, &m_categories[i], 0);
    }

    insertChild(10, &m_backButton, 0);
    insertChild(11, &m_blackBackControl, 0);
    insertChild(12, &m_arrowLeft, 0);
    insertChild(13, &m_arrowRight, 0);
    insertChild(14, &m_instructionText, 0);

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
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    bool isRace = Section::GetSceneId(sectionId) == System::SceneId::Race;
    m_arrowLeft.load("button", "NumberMenuArrowLeft", "ButtonArrowLeft", 0x1, false, true);
    m_arrowRight.load("button", "NumberMenuArrowRight", "ButtonArrowRight", 0x1, false, true);
    if (isRace) {
        // m_instructionText.load("bg", "MenuObiInstructionText", "MenuObiInstructionText",
        // nullptr);
        m_instructionText.load("bg", "RaceObiInstructionText", "RaceObiInstructionText", nullptr);
    } else {
        m_instructionText.load("bg", "MenuObiInstructionText", "MenuObiInstructionText", nullptr);
    }
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Option, &m_onClose, false, false);
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
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    u32 categoryIndex = settingsPage->getCategoryInfoGetter().categoryIndex;
    m_categories[categoryIndex].selectDefault(0);
    // m_categories[m_chosen].setPaneVisible("checkmark", true);
    if (std::size(m_categories) > magic_enum::enum_count<SP::ClientSettings::Category>()) {
        m_arrowLeft.setVisible(false);
        m_arrowRight.setVisible(false);
    }

    for (u8 i = 0; i < std::size(m_categories); i++) {
        // TODO: Fix hardcode
        if (i >= magic_enum::enum_count<SP::ClientSettings::Category>()) {
            m_categories[i].setVisible(false);
            m_categories[i].setPlayerFlags(0);
            m_categories[i].setMessageAll(6602);
            continue;
        }
        m_categories[i].setVisible(true);
        m_categories[i].setPlayerFlags(1);
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

void SettingsCategorySwapPage::onClose(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsCategorySwapPage::onButtonFront(PushButton *button, u32 /*localPlayerId*/) {
    SP_LOG("newCategory: %d", button->m_index);
    if (button->m_index == m_arrowLeft.m_index) {
        // TODO: Implement sheet system
        return;
    } else if (button->m_index == m_arrowRight.m_index) {
        // TODO: Implement a sheet system
        return;
    }
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    // clear messages, change fields of SettingsPage, and fall to prev page
    settingsPage->changeCategory(button->m_index);
    // settingsPage->clearMessageLists();
    // settingsPage->setCategoryInfo(button->m_index);
    // settingsPage->setCategoryValues(button->m_index);
    // settingsPage->setButtons();
    onBack(0);
    // m_categories[m_chosen].setPaneVisible("checkmark", false);
    // m_chosen = button->m_index;
    // m_categories[m_chosen].setPaneVisible("checkmark", true);
}

} // namespace UI
