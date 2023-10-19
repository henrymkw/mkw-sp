#include "SettingsOptionsPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

#include <cstdio>

namespace UI {

SettingsOptionsPage::SettingsOptionsPage() = default;

SettingsOptionsPage::~SettingsOptionsPage() = default;

void SettingsOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);

    initChildren(7);
    for (u8 i = 0; i < 5; i++) {
        insertChild(i, &m_options[i], 0);
    }

    insertChild(5, &m_blackBackControl, 0);
    insertChild(6, &m_backButton, 0);

    for (u8 i = 0; i < 5; i++) {
        char variant[10];
        snprintf(variant, sizeof(variant), "Button4_%hhu", i);
        m_options[i].load("button", "SettingsOptionAndDescription", variant, 0x1, false, false);
    }

    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;
    m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);

    m_options[0].selectDefault(0);
    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (u32 i = 0; i < 5; i++) {
        m_options[i].setFrontHandler(&m_onOptionButtonFront, false);
        m_options[i].m_index = i;
    }
}

void SettingsOptionsPage::onActivate() {
    SP_LOG("onActivate() called");
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    auto categoryInfo = settingsPage->getCategoryInfo();
    u32 settingIndexLocal = settingsPage->getSelectedSetting();
    u32 settingIndex = categoryInfo.settingIndex + settingIndexLocal;
    // SP_LOG("settingIndex: %d", settingIdx);
    const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[settingIndex];
    /*SP_LOG("categoryInfo.settingIndex: %d\nsetttingIndexLocal: %d\nsettingIndex: %d",
            categoryInfo.settingIndex, settingIndexLocal, settingIndex);*/

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
    // SP_LOG("onOptionsButtonFront() called");
    // SP_LOG("button->m_index: %d", button->m_index);
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    auto categoryInfo = settingsPage->getCategoryInfo();
    u32 localSettingIndex = settingsPage->getSelectedSetting();
    /*SP_LOG("categoryInfo.settingIndex: %d\nlocalSettingIndex: %d", categoryInfo.settingIndex,
            localSettingIndex); */
    auto *saveManager = System::SaveManager::Instance();
    u32 settingIndex = categoryInfo.settingIndex + localSettingIndex;
    // SP_LOG("settingIndex: %d\nbutton->m_index: %d", settingIndex, button->m_index);

    saveManager->setSetting(settingIndex, button->m_index);
}

} // namespace UI
