#include "SettingsCategorySwapPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "sp/settings/ClientSettings.hh"

namespace UI {

SettingsCategorySwapPage::SettingsCategorySwapPage() = default;

SettingsCategorySwapPage::~SettingsCategorySwapPage() = default;

void SettingsCategorySwapPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    SP_LOG("onInit called");

    initChildren(13);
    for (u8 i = 0; i < 10; i++) {
        insertChild(i, &m_categories[i], 0);
    }

    insertChild(10, &m_backButton, 0);
    insertChild(11, &m_blackBackControl, 0);
    insertChild(12, &m_instructionText, 0);

    u8 buttonId = 0;
    for (u8 i = 0; i < 2; i++) {
        for (u8 j = 0; j < 5; j++) {
            char variant[18];
            snprintf(variant, sizeof(variant), "CategoryButton%hhu_%hhu", i, j);
            SP_LOG("before id: %d i: %d j: %d variant: %s", buttonId, i, j, variant);
            // TODO: Fix crash
            m_categories[buttonId].load("button", "CategorySwapPage", variant, 0x1, false, false);

            // This will print once everything works
            SP_LOG("after id: %d i: %d j: %d", buttonId, i, j);
            buttonId++;
        }
    }

    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (Section::GetSceneId(sectionId) == System::SceneId::Race) {
        m_backButton.load("message_window", "Back", "ButtonBack", 0x1, false, true);
        m_instructionText.load("bg", "RaceObiInstructionText", "RaceObiInstructionText", nullptr);
    } else {
        m_instructionText.load("bg", "MenuObiInstructionText", "MenuObiInstructionText", nullptr);
        m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);
    }

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    for (u8 i = 0; i < 10; i++) {
        m_categories[i].setFrontHandler(&m_onButtonFront, false);
        m_categories[i].m_index = i;
    }

    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
}

void SettingsCategorySwapPage::onActivate() {
    for (u8 i = 0; i < 10; i++) {
        // TODO: Fix the hardcode
        if (i >= 9) {
            m_categories[i].setVisible(false);
            continue;
        }
        m_categories[i].setMessageAll(SP::ClientSettings::categoryMessageIds[i]);
    }
}

void SettingsCategorySwapPage::onBack(u32 /* localPlayerId */) {}

void SettingsCategorySwapPage::onBackButtonFront(PushButton * /*button*/, u32 /*localPlayerId*/) {}

void SettingsCategorySwapPage::onButtonFront(PushButton * /*button*/, u32 /*localPlayerId*/) {}

} // namespace UI
