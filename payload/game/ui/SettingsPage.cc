#include "SettingsPage.hh"

#include "game/host_system/Scene.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

SettingsPage::SettingsPage() = default;

SettingsPage::~SettingsPage() = default;

void SettingsPage::onInit() {
    SP_LOG("onInit() called");
    SP_LOG("Sizeof SettingsPage: %d", sizeof(SettingsPage));
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    /*for (u32 i = 0; i < SP::ClientSettings::entryCount; i++) {
        if (SP::ClientSettings::entries[i].category != category ||
                SP::ClientSettings::entries[i].valueCount == 0 ||
                SP::ClientSettings::entries[i].hidden) {
            continue;
        }
        const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[i];
        u32 chosen = System::SaveManager::Instance()->getSetting(i) - entry.valueOffset;

        if (entry.valueNames) {
            m_settingOptionIds.push_back(entry.valueExplanationMessageIds[chosen]);
        } else {
            m_settingOptionIds.push_back(10004);
        }
        m_settingNameIds.push_back(static_cast<const u32 &&>(entry.messageId));
    }*/

    initChildren(6 + std::size(m_settingButtons) + !!blackBack());
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, instructionText(), 0);
    insertChild(2, &m_backButton, 0);
    insertChild(3, &m_arrowUp, 0);
    insertChild(4, &m_arrowDown, 0);
    insertChild(5, &m_categorySwap, 0);

    if (blackBack()) {
        insertChild(6, blackBack(), 0);
    }
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        insertChild(6 + i + !!blackBack(), &m_settingButtons[i], 0);
    }

    m_pageTitleText.load(false);

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (blackBack()) {
        instructionText()->load("bg", "ObiInstructionTextPopup", "ObiInstructionTextPopup",
                nullptr);
        m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);
    } else if (Section::GetSceneId(sectionId) == System::SceneId::Race) {
        m_backButton.load("message_window", "Back", "ButtonBack", 0x1, false, true);
        instructionText()->load("bg", "RaceObiInstructionText", "RaceObiInstructionText", nullptr);
    } else {
        m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);
        instructionText()->load("bg", "MenuObiInstructionText", "MenuObiInstructionText", nullptr);
    }
    if (blackBack()) {
        blackBack()->load("message_window", "BlackBack", "BlackBack");
        blackBack()->m_zIndex = -1.0f;
    }

    m_categoryInfo = getCategoryInfo(1);

    // auto category = static_cast<SP::ClientSettings::Category>(m_categoryInfo.categoryIndex);
    setCategoryValues(1);

    for (u8 i = 0; i < std::size(m_settingButtons); i++) {
        char variant[15];
        snprintf(variant, sizeof(variant), "WheelButton%hhu", i);
        if (i == 2) {
            m_settingButtons[i].load("button", "SettingsWheel", variant, 0x1, false, false);
            continue;
        }
        m_settingButtons[i].load("button", "SettingsWheel", variant, 0x1, false, true);
    }

    m_arrowUp.load("button", "ArrowUpDown", "ArrowUp", 0x1, false, true);
    m_arrowDown.load("button", "ArrowUpDown", "ArrowDown", 0x1, false, true);
    m_categorySwap.load("button", "CategorySwapButton", "CategorySwapButton", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Down, &m_onUp, false, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Up, &m_onDown, false, false);

    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        m_settingButtons[i].setFrontHandler(&m_onSettingsWheelButtonFront, false);
        m_settingButtons[i].setSelectHandler(&m_onSettingsWheelButtonSelect, false);
        m_settingButtons[i].setDeselectHandler(&m_onSettingsWheelButtonDeselect, false);
        m_settingButtons[i].setVisible(true);
        m_settingButtons[i].setMessage("setting_name", *m_settingNameIds[i]);
        m_settingButtons[i].setMessage("current_option", *m_settingOptionIds[i]);
        m_settingButtons[i].m_index = i;
    }
    m_arrowUp.setFrontHandler(&m_onSettingsWheelButtonFront, false);
    m_arrowDown.setFrontHandler(&m_onSettingsWheelButtonFront, false);
    m_arrowUp.m_index = std::size(m_settingButtons);
    m_arrowDown.m_index = m_arrowUp.m_index + 1;

    m_pageTitleText.setMessage(10076);

    m_settingButtons[0].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[1].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[3].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[4].setPaneVisible("pause_w_fade_n", false);

    m_selected = 2;
}

void SettingsPage::onActivate() {
    instructionText()->setMessageAll(0);
    instructionText()->setVisible(true);
    SP_LOG("instructionTextChanged");
    auto *saveManager = System::SaveManager::Instance();
    u32 settingIndex = m_categoryInfo.settingIndex + m_selected;
    auto &e = SP::ClientSettings::entries[settingIndex];
    instructionText()->setMessageAll(
            e.valueExplanationMessageIds[saveManager->getSetting(settingIndex) - e.valueOffset]);

    m_settingButtons[2].selectDefault(0);
}

BlackBackControl *SettingsPage::blackBack() {
    return nullptr;
}

void SettingsPage::onBack(u32 /* localPlayerId */) {
    if (m_handler) {
        m_handler->handle(this, nullptr);
    }

    startReplace(Anim::Prev, 0.0f);
}

void SettingsPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    if (m_handler) {
        m_handler->handle(this, button);
    }

    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void SettingsPage::onSettingsWheelButtonFront(PushButton *button, u32 /* localPlayerId */) {
    // Can sort of see the arrows when SettingsOptionsPage is the active page. Commented out until
    // theres a way to get them visible after returning

    // m_arrowUp.setVisible(false);
    // m_arrowDown.setVisible(false);

    SP_LOG("ButtonFront() called");
    if (button->m_index == m_arrowUp.m_index) {
        onUp(0);
    } else if (button->m_index == m_arrowDown.m_index) {
        onDown(0);
    } else {
        const SP::ClientSettings::Entry &entry =
                SP::ClientSettings::entries[m_categoryInfo.settingIndex + m_selected];
        // TODO: make one of these for each menuType
        if (entry.menuType == SP::Settings::MenuType::Number) {
            push(PageId::SettingsNumberOptions, Anim::Next);
        } else if (entry.menuType == SP::Settings::MenuType::OptionAndDescription) {
            push(PageId::SettingsOptions, Anim::Next);
        }
    }
    // TODO: set the arrows to be visible again after b is pressed
}

void SettingsPage::onUp(u32 /* localPlayerId */) {
    u32 temp = *m_settingNameIds[0];
    m_settingNameIds.pop_front();
    m_settingNameIds.push_back(static_cast<const u32 &&>(temp));
    u32 temp2 = *m_settingOptionIds[0];
    m_settingOptionIds.pop_front();
    m_settingOptionIds.push_back(static_cast<const u32 &&>(temp2));
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        m_settingButtons[i].setMessage("setting_name", *m_settingNameIds[i]);
        m_settingButtons[i].setMessage("current_option", *m_settingOptionIds[i]);
    }
    if (m_selected == m_settingNameIds.count() - 1) {
        m_selected = 0;
    } else {
        m_selected++;
    }
    // TODO: yeah i know will fix later
    if (m_buttonIndex == (s32)m_settingNameIds.count() - 1) {
        m_buttonIndex = 0;
    } else {
        m_buttonIndex++;
    }
    SP_LOG("instructionTextChanged");
    u32 settingIndex = m_categoryInfo.settingIndex + m_selected;
    auto &e = SP::ClientSettings::entries[settingIndex];
    auto *saveManager = System::SaveManager::Instance();
    instructionText()->setMessageAll(
            e.valueExplanationMessageIds[saveManager->getSetting(settingIndex) - e.valueOffset]);

    SP_LOG("SettingsPage::onDown(): m_buttonIndex: %d\nm_selected: %d", m_buttonIndex, m_selected);
}

void SettingsPage::onDown(u32 /* localPlayerId */) {
    u32 temp = *m_settingNameIds[m_settingNameIds.count() - 1];
    m_settingNameIds.pop_back();
    m_settingNameIds.push_front(static_cast<const u32 &&>(temp));
    u32 temp2 = *m_settingOptionIds[m_settingOptionIds.count() - 1];
    m_settingOptionIds.pop_back();
    m_settingOptionIds.push_front(static_cast<const u32 &&>(temp2));
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        m_settingButtons[i].setMessage("setting_name", *m_settingNameIds[i]);
        m_settingButtons[i].setMessage("current_option", *m_settingOptionIds[i]);
    }
    if (m_selected == 0) {
        m_selected = m_settingNameIds.count() - 1;

    } else {
        m_selected--;
    }
    if (m_buttonIndex == 0) {
        m_buttonIndex = m_settingNameIds.count() - 1;
    } else {
        m_buttonIndex--;
    }
    SP_LOG("instructionTextChanged");
    u32 settingIndex = m_categoryInfo.settingIndex + m_selected;
    auto &e = SP::ClientSettings::entries[settingIndex];
    auto *saveManager = System::SaveManager::Instance();
    instructionText()->setMessageAll(
            e.valueExplanationMessageIds[saveManager->getSetting(settingIndex) - e.valueOffset]);

    SP_LOG("SettingsPage::onUp(): m_buttonIndex: %d\nm_selected: %d", m_buttonIndex, m_selected);
}

void SettingsPage::onSettingsWheelButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
}

void SettingsPage::onSettingsWheelButtonDeselect(PushButton * /* button */,
        u32 /* localPlayerId */) {}

SettingsPage::CategoryInfo SettingsPage::getCategoryInfo(u32 sheetIndex) const {
    u32 controlCount = std::size(m_settingButtons); // This will always be 5
    SP_LOG("controlCount: %d", controlCount);
    CategoryInfo info{};
    u32 sheetCount = 0;
    u32 categoryCount = magic_enum::enum_count<SP::ClientSettings::Category>();
    SP_LOG("categoryCount: %d", categoryCount);
    for (; info.categoryIndex < categoryCount; info.categoryIndex++) {
        // Looping through the categories, setting the index of the category
        auto category = static_cast<SP::ClientSettings::Category>(info.categoryIndex);
        u32 settingCount = 0;
        // Loop through all the settings
        for (u32 i = 0; i < magic_enum::enum_count<SP::ClientSettings::Setting>(); i++) {
            if (SP::ClientSettings::entries[i].category != category) {
                continue;
            }
            if (SP::ClientSettings::entries[i].hidden) {
                continue;
            }
            if (SP::ClientSettings::entries[i].valueCount == 0) {
                continue;
            }
            if (settingCount % controlCount == 0) {
                SP_LOG("settingCount: %d\ncontrolCount: %d", settingCount, controlCount);
                if (sheetCount == sheetIndex) {
                    info.categorySheetIndex = settingCount / std::size(m_settingButtons);
                    info.settingIndex = i;
                }
                sheetCount++;
            }
            settingCount++;
        }
        if (sheetCount > sheetIndex) {
            info.categorySheetCount = (settingCount + controlCount - 1) / controlCount;
            return info;
        }
    }
    assert(false);
}

void SettingsPage::setCategoryValues(u32 categoryIndex) {
    // auto category = static_cast<SP::ClientSettings::Category>(m_categoryInfo.categoryIndex);
    for (u32 i = 0; i < SP::ClientSettings::entryCount; i++) {
        const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[i];
        if (entry.category != static_cast<SP::ClientSettings::Category>(categoryIndex) ||
                entry.valueCount == 0 || entry.hidden) {
            continue;
        }
        u32 chosen = System::SaveManager::Instance()->getSetting(i) - entry.valueOffset;

        if (entry.valueNames) {
            // instructionText()->setMessageAll(0);
            m_settingOptionIds.push_back(entry.valueMessageIds[chosen]);
        } else {
            m_settingOptionIds.push_back(10004);
        }
        m_settingNameIds.push_back(static_cast<const u32 &&>(entry.messageId));
    }
}

SettingsPage::CategoryInfo SettingsPage::getCategoryInfoGetter() {
    return m_categoryInfo;
}

u32 SettingsPage::getSelectedSetting() {
    return m_selected;
}

// LayoutUIControl *

SettingsPagePopup::SettingsPagePopup() = default;

SettingsPagePopup::~SettingsPagePopup() = default;

void SettingsPagePopup::onInit() {
    SettingsPage::onInit();

    m_popRequested = false;
}

void SettingsPagePopup::beforeCalc() {
    if (state() == State::State4 && m_popRequested) {
        startReplace(Anim::Prev, 0.0f);
        m_popRequested = false;
    }
}

LayoutUIControl *SettingsPagePopup::instructionText() {
    return &m_instructionText;
}

BlackBackControl *SettingsPagePopup::blackBack() {
    return &m_blackBack;
}

void SettingsPagePopup::configure(IHandler *handler) {
    m_handler = handler;
}

void SettingsPagePopup::pop(Anim anim) {
    setAnim(anim);
    m_popRequested = true;
}

SettingsPage::CategoryInfo SettingsPagePopup::getCategoryInfo() {
    return SettingsPage::getCategoryInfoGetter();
}

u32 SettingsPagePopup::getSelectedSetting() {
    return SettingsPage::getSelectedSetting();
}

MenuSettingsPage::MenuSettingsPage() = default;

MenuSettingsPage::~MenuSettingsPage() = default;

PageId MenuSettingsPage::getReplacement() {
    return m_replacement;
}

void MenuSettingsPage::onInit() {
    SettingsPage::onInit();

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (!blackBack() && sectionId == SectionId::LicenseSettings) {
        m_replacement = PageId::LicenseSettingsTop;
    } else {
        m_replacement = PageId::None; // Failsafe
    }
}

void MenuSettingsPage::onActivate() {
    SettingsPage::onActivate();

    if (m_replacement != PageId::None) {
        auto *section = SectionManager::Instance()->currentSection();
        auto *raceMenuPage = section->page(m_replacement)->downcast<RaceMenuPage>();
        if (raceMenuPage != nullptr) {
            raceMenuPage->setReplacement(PageId::None);
        }
    }
}

LayoutUIControl *MenuSettingsPage::instructionText() {
    return &m_instructionText;
}

BlackBackControl *MenuSettingsPage::blackBack() {
    return nullptr;
}

void MenuSettingsPage::configure(IHandler *handler, PageId replacement) {
    m_handler = handler;
    m_replacement = replacement;
}

SettingsPage::CategoryInfo MenuSettingsPage::getCategoryInfo() {
    return SettingsPage::getCategoryInfoGetter();
}

u32 MenuSettingsPage::getSelectedSetting() {
    return SettingsPage::getSelectedSetting();
}

} // namespace UI
