#include "SettingsNumberOptionsPage.hh"

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

    initChildren(36);
    for (u8 i = 0; i < 30; i++) {
        insertChild(i, &m_options[i], 0);
    }

    insertChild(30, &m_backButton, 0);
    insertChild(31, &m_arrowLeft, 0);
    insertChild(32, &m_arrowRight, 0);
    insertChild(33, &m_blackBackControl, 0);
    insertChild(34, &m_instructionText, 0);
    insertChild(35, &m_settingTitleText, 0);
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

    m_settingTitleText.load("button", "SubMenuSettingTitle", "SettingTitleTop", nullptr);

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
    m_chosen = 0;
}

void SettingsNumberOptionsPage::onActivate() {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    u32 settingIndex = settingsPage->getSettingIndex();
    const auto &entry = SP::ClientSettings::entries[settingIndex];
    m_chosen = System::SaveManager::Instance()->getSetting(settingIndex) - entry.valueOffset;
    m_options[m_chosen].selectDefault(0);

    m_settingTitleText.setMessageAll(entry.messageId);
    m_instructionText.setVisible(true);

    if (entry.valueCount <= 30) {
        m_arrowLeft.setVisible(false);
        m_arrowRight.setVisible(false);
    }

    for (u8 i = 0; i < 30; i++) {
        if (i >= entry.valueCount - 1) {
            m_options[i].setVisible(false);
            continue;
        }
        m_options[i].setPaneVisible("checkmark", false);

        MessageInfo info{};
        info.intVals[0] = i + entry.valueOffset;
        m_options[i].setMessageAll(entry.valueMessageIds[0], &info);
    }
    m_options[m_chosen].setPaneVisible("checkmark", true);
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
        u32 settingIndex = settingsPage->getSettingIndex();
        System::SaveManager::Instance()->setSetting(settingIndex, button->m_index);
        settingsPage->setMiddleButton(settingIndex);
        m_options[m_chosen].setPaneVisible("checkmark", false);
        m_chosen = button->m_index;
        m_options[m_chosen].setPaneVisible("checkmark", true);
    } else if (button->m_index == m_arrowLeft.m_index) {
        // Left arrow
        // TODO: Implement a sheet system, something like this
        // numSheets = entry.valueCount / 30 + (entry.valueCount % 30 != 0)
        // m_currSheet-- % numSheets (but if --results in a negative reset it back to numSheets)
        // MessageInfo info{}
        // info.intVals[0] = m_currSheet * 30 + entry.valueOffset
        // m_options[i].setMessageAll(entry.valueMessageIds[0], &info)
    } else if (button->m_index == m_arrowRight.m_index) {
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
