#include "SettingsNumberOptions.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"

#include <cstdio>

namespace UI {

SettingsNumberOptionsPage::SettingsNumberOptionsPage() = default;

SettingsNumberOptionsPage::~SettingsNumberOptionsPage() = default;

void SettingsNumberOptionsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(35);
    for (u8 i = 0; i < 30; i++) {
        insertChild(i, &m_options[i], 0);
    }

    insertChild(30, &m_backButton, 0);
    insertChild(31, &m_arrowLeft, 0);
    insertChild(32, &m_arrowRight, 0);
    insertChild(33, &m_blackBackControl, 0);
    insertChild(34, &m_instructionText, 0);

    u8 buttonId = 0;
    for (u8 i = 0; i < 6; i++) {
        for (u8 j = 0; j < 5; j++) {
            char variant[14];
            snprintf(variant, sizeof(variant), "Button%hhu_%hhu", i, j);
            m_options[buttonId].load("button", "NumberGridMenu", variant, 0x1, false, false);
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

    m_arrowLeft.load("button", "NumberMenuArrowLeft", "ButtonArrowLeft", 0x1, false, true);
    m_arrowRight.load("button", "NumberMenuArrowRight", "ButtonArrowRight", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    for (u8 i = 0; i < 30; i++) {
        m_options[i].setFrontHandler(&m_onOptionButtonFront, false);
        m_options[i].m_index = i;
    }
    // TODO: Don't hard code indicies
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_arrowLeft.setFrontHandler(&m_onOptionButtonFront, false);
    m_arrowRight.setFrontHandler(&m_onOptionButtonFront, false);
    m_backButton.m_index = 30;
    m_arrowLeft.m_index = 31;
    m_arrowRight.m_index = 32;
}

void SettingsNumberOptionsPage::onActivate() {
    // TODO: Select the current option
    m_options[0].selectDefault(0);
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    auto categoryInfo = settingsPage->getCategoryInfo();
    u32 settingIndexLocal = settingsPage->getSelectedSetting();
    u32 settingIndex = categoryInfo.settingIndex + settingIndexLocal;
    const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[settingIndex];
    if (entry.valueCount <= 30) {
        m_arrowLeft.setVisible(false);
        m_arrowRight.setVisible(false);
    }

    for (u8 i = 0; i < 30; i++) {
        if (i >= entry.valueCount - 1) {
            m_options[i].setVisible(false);
            continue;
        }
        // TODO: set buttons with index greater than num of options to non visible
        MessageInfo info{};
        info.intVals[0] = i + entry.valueOffset;
        m_options[i].setMessageAll(entry.valueMessageIds[0], &info);
    }
}

void SettingsNumberOptionsPage::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsNumberOptionsPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void SettingsNumberOptionsPage::onOptionButtonFront(PushButton *button, u32 /* localPlayerId */) {
    if (button->m_index < 30) {
        // Get the setting and set its value
        // TODO: Implement category switch to actually test this thing
        auto *settingsPage =
                SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
        auto categoryInfo = settingsPage->getCategoryInfo();
        u32 localSettingIndex = settingsPage->getSelectedSetting();
        auto *saveManager = System::SaveManager::Instance();
        u32 settingIndex = categoryInfo.settingIndex + localSettingIndex;
        saveManager->setSetting(settingIndex, button->m_index);
    } else if (button->m_index == 30) {
        // Back button
    } else if (button->m_index == 31) {
        // Left arrow
        // TODO: Implement a sheet system, something like this
        // numSheets = entry.valueCount / 30 + (entry.valueCount % 30 != 0)
        // m_currSheet-- % numSheets (but if --results in a negative reset it back to numSheets)
        // MessageInfo info{}
        // info.intVals[0] = m_currSheet * 30 + entry.valueOffset
        // m_options[i].setMessageAll(entry.valueMessageIds[0], &info)
    } else if (button->m_index == 32) {
        // Right arrow
        // TODO: Implement a sheet system, something like this
        // numSheets = entry.valueCount / 30 + (entry.valueCount % 30 != 0)
        // m_currSheet++ % numSheets
        // MessageInfo info{}
        // info.intVals[0] = m_currSheet * 30 + entry.valueOffset
        // m_options[i].setMessageAll(entry.valueMessageIds[0], &info)
    }
}

} // namespace UI