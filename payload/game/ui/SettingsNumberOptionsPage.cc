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
    for (u8 i = 0; i < std::size(m_options); i++) {
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
        m_instructionText.load("bg", "BlackBackObiInstructionText", "RaceObiInstructionText",
                nullptr);
        m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);
    }

    m_arrowLeft.load("button", "NumberMenuArrowLeft", "ButtonArrowLeft", 0x1, false, true);
    m_arrowRight.load("button", "NumberMenuArrowRight", "ButtonArrowRight", 0x1, false, true);

    m_settingTitleText.load("button", "SubMenuSettingTitle", "SettingTitleTop", nullptr);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    for (u8 i = 0; i < std::size(m_options); i++) {
        m_options[i].setFrontHandler(&m_onOptionButtonFront, false);
        m_options[i].setSelectHandler(&m_onOptionButtonSelect, false);
        m_options[i].m_index = i;
    }

    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_arrowLeft.setFrontHandler(&m_onOptionButtonFront, false);
    m_arrowRight.setFrontHandler(&m_onOptionButtonFront, false);
    m_arrowLeft.setSelectHandler(&m_onOptionButtonSelect, false);
    m_arrowRight.setSelectHandler(&m_onOptionButtonSelect, false);
    m_backButton.m_index = std::size(m_options);
    m_arrowLeft.m_index = m_backButton.m_index + 1;
    m_arrowRight.m_index = m_arrowLeft.m_index + 1;
    m_selectedIndex = 0;
    m_arrowLeft.setPointerOnly(false);
    m_arrowRight.setPointerOnly(false);
}

void SettingsNumberOptionsPage::onActivate() {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    u32 settingIndex = settingsPage->getSettingIndex();
    const auto &entry = SP::ClientSettings::entries[settingIndex];
    // Index of the button thats chosen
    m_selectedValue = System::SaveManager::Instance()->getSetting(settingIndex) - entry.valueOffset;
    m_selectedIndex = m_selectedValue % std::size(m_options);
    // This is the actual number of the selected setting
    m_options[m_selectedIndex].selectDefault(0);
    m_settingTitleText.setMessageAll(entry.messageId);
    m_instructionText.setVisible(true);
    m_numSheets = 1;
    m_currSheet = (System::SaveManager::Instance()->getSetting(settingIndex) - entry.valueOffset) /
            std::size(m_options);
    while (entry.valueCount > m_numSheets * std::size(m_options)) {
        m_numSheets++;
    }
    MessageInfo info{};
    info.intVals[0] = m_selectedIndex + entry.valueOffset + (m_currSheet * std::size(m_options));
    m_instructionText.setMessageAll(entry.valueExplanationMessageIds[0], &info);
    if (entry.valueCount <= std::size(m_options)) {
        m_arrowLeft.setVisible(false);
        m_arrowRight.setVisible(false);
	m_arrowLeft.setPlayerFlags(0);
	m_arrowRight.setPlayerFlags(0);
    } else {
        m_arrowLeft.setVisible(true);
        m_arrowRight.setVisible(true);
  	m_arrowLeft.setPlayerFlags(0);
	m_arrowRight.setPlayerFlags(0);
    }
    refresh();
}

void SettingsNumberOptionsPage::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsNumberOptionsPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void SettingsNumberOptionsPage::onOptionButtonSelect(PushButton *button, u32 /* localPlayerId */) {
    SP_LOG("onOptionButtonSelect(): button->m_index: %d", button->m_index);
    if (button->m_index == m_arrowRight.m_index || button->m_index == m_arrowLeft.m_index) {
        m_instructionText.setVisible(false);
        return;
    }
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    u32 settingIndex = settingsPage->getSettingIndex();
    const auto &entry = SP::ClientSettings::entries[settingIndex];
    MessageInfo info{};
    info.intVals[0] = button->m_index + entry.valueOffset + (30 * m_currSheet);
    m_instructionText.setMessageAll(entry.valueExplanationMessageIds[0], &info);
    m_instructionText.setVisible(true);
}

void SettingsNumberOptionsPage::refresh() {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    u32 settingIndex = settingsPage->getSettingIndex();
    const auto &entry = SP::ClientSettings::entries[settingIndex];

    u32 offset = (m_currSheet)*std::size(m_options);
    for (u8 i = 0; i < std::size(m_options); i++) {
        if (i + offset >= entry.valueCount - (entry.valueOffset == 0)) {
            m_options[i].setVisible(false);
            m_options[i].setPlayerFlags(0);
            continue;
        }
        if (i + offset == m_selectedValue) {
            m_options[i].setPaneVisible("checkmark", true);
            SP_LOG("setting option %d as marked", i);
	} else {
            m_options[i].setPaneVisible("checkmark", false);
        }
        m_options[i].setVisible(true);
        m_options[i].setPlayerFlags(1);
        MessageInfo info{};

        info.intVals[0] = offset + i + entry.valueOffset;
        m_options[i].setMessageAll(entry.valueMessageIds[0], &info);
    }
    m_options[m_selectedIndex].selectDefault(0);
}

void SettingsNumberOptionsPage::onOptionButtonFront(PushButton *button, u32 /* localPlayerId */) {
    SP_LOG("onOptionButtonFront(): button->m_index: %d", button->m_index);
    if (button->m_index < 30) {
        // Get the setting and set its value
        // TODO: Implement category switch to actually test this thing
        auto *settingsPage =
                SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
        u32 settingIndex = settingsPage->getSettingIndex();
        const auto &entry = SP::ClientSettings::entries[settingIndex];
        m_selectedValue = button->m_index + entry.valueOffset + (30 * m_currSheet);
        System::SaveManager::Instance()->setSetting(settingIndex, m_selectedValue);
        settingsPage->setMiddleButton(settingIndex);
        for (u8 i = 0; i < 30; i++) {
            m_options[m_selectedIndex].setPaneVisible("checkmark", false);
        }
        m_selectedIndex = button->m_index;
        m_options[m_selectedIndex].setPaneVisible("checkmark", true);
        MessageInfo info{};
        info.intVals[0] = m_selectedValue;
        m_instructionText.setMessageAll(entry.valueExplanationMessageIds[0], &info);
    } else if (button->m_index == m_arrowRight.m_index) {
        if (m_currSheet == m_numSheets - 1) {
            m_currSheet = 0;
        } else {
            m_currSheet++;
        }
        refresh();
        m_arrowRight.selectDefault(0);
    } else if (button->m_index == m_arrowLeft.m_index) {
        if (m_currSheet == 0) {
            m_currSheet = m_numSheets - 1;
        } else {
            m_currSheet--;
        }
        refresh();
        m_arrowLeft.selectDefault(0);
    }
}

} // namespace UI
