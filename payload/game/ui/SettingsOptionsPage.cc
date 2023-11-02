#include "SettingsOptionsPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"

#include <cstdio>

namespace UI {

SettingsOptionsPage::SettingsOptionsPage() = default;

SettingsOptionsPage::~SettingsOptionsPage() = default;

void SettingsOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(4 + std::size(m_options));

    insertChild(0, &m_blackBackControl, 0);
    insertChild(1, &m_backButton, 0);
    insertChild(2, &m_instructionText, 0);
    insertChild(3, &m_settingTitleText, 0);

    for (u8 i = 0; i < std::size(m_options); i++) {
        insertChild(i + 4, &m_options[i], 0);
    }

    for (u8 i = 0; i < std::size(m_options); i++) {
        char variant[13];
        snprintf(variant, sizeof(variant), "Button4_%hhu", i);
        m_options[i].load("button", "SettingsOptionAndDescription", variant, 0x1, false, false);
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

    m_settingTitleText.load("button", "SubMenuSettingTitle", "SettingTitleTop", nullptr);

    m_options[0].selectDefault(0);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (u32 i = 0; i < 5; i++) {
        m_options[i].setFrontHandler(&m_onOptionButtonFront, false);
        m_options[i].m_index = i;
    }
}

void SettingsOptionsPage::onActivate() {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    auto categoryInfo = settingsPage->getCategoryInfo();
    u32 settingIndexLocal = settingsPage->getSelectedSetting();
    u32 settingIndex = categoryInfo.settingIndex + settingIndexLocal;
    const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[settingIndex];
    u32 chosen = System::SaveManager::Instance()->getSetting(settingIndex) - entry.valueOffset;
    m_options[chosen].selectDefault(0);
    m_settingTitleText.setMessageAll(entry.messageId);

    for (u8 j = 0; j != 5; j++) {
        if (j >= entry.valueCount) {
            m_options[j].setVisible(false);
            continue;
        }
        m_options[j].setVisible(true);
        m_options[j].setMessage("text", entry.valueMessageIds[j]);
        m_options[j].setMessage("text_00", entry.valueExplanationMessageIds[j]);
    }
}

void SettingsOptionsPage::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsOptionsPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void SettingsOptionsPage::onOptionButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    auto categoryInfo = settingsPage->getCategoryInfo();
    u32 localSettingIndex = settingsPage->getSelectedSetting();
    auto *saveManager = System::SaveManager::Instance();
    u32 settingIndex = categoryInfo.settingIndex + localSettingIndex;
    saveManager->setSetting(settingIndex, button->m_index);
}

} // namespace UI
