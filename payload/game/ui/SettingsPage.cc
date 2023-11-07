#include "SettingsPage.hh"

#include "game/host_system/Scene.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsCategorySwapPage.hh"
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

SettingsPage::SettingsPage() = default;

SettingsPage::~SettingsPage() = default;

void SettingsPage::onInit() {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    bool isRace = Section::GetSceneId(sectionId) == System::SceneId::Race;
    SP_LOG("Sizeof SettingsPage: %d", sizeof(SettingsPage));
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    initChildren(7 + std::size(m_settingButtons) + !!blackBack() + !isRace);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, instructionText(), 0);
    insertChild(2, &m_backButton, 0);
    insertChild(3, &m_arrowUp, 0);
    insertChild(4, &m_arrowDown, 0);
    insertChild(5, &m_categorySwap, 0);
    insertChild(6, &m_categorySwapPlusIcon, 0);
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        insertChild(7 + i, &m_settingButtons[i], 0);
    }
    if (!isRace) {
        insertChild(7 + std::size(m_settingButtons), &m_menuObiTopNoCurve, 0);
    }
    if (blackBack()) {
        insertChild(7 + std::size(m_settingButtons) + !isRace, blackBack(), 0);
    }

    m_pageTitleText.load(false);

    if (blackBack()) {
        instructionText()->load("bg", "ObiInstructionTextPopup", "ObiInstructionTextPopup",
                nullptr);
        m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);
    } else if (isRace) {
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

    setCategoryInfo(1);
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
    m_categorySwapPlusIcon.load("control", "ClassChange", "ClassChange", nullptr);
    if (!isRace) {
        m_menuObiTopNoCurve.load("bg", "MenuObiTopNoCurve", "MenuObiTopNoCurve", nullptr);
    }

    auto *sectionManager = UI::SectionManager::Instance();
    auto &registeredPadManager = sectionManager->registeredPadManager();
    u32 flags = registeredPadManager.getFlags(0);
    u32 padType = REGISTERED_PAD_FLAGS_GET_TYPE(flags);
    u32 messageId = padType == REGISTERED_PAD_TYPE_GC ? 2306 : 2305;
    MessageInfo info = {};
    info.messageIds[0] = messageId;
    m_categorySwapPlusIcon.setMessageAll(3012, &info);
    // u32 flags = RegisteredPadManager_getFlags(&s_sectionManager->registeredPadManager, 0);
    // u32 padType = REGISTERED_PAD_FLAGS_GET_TYPE(flags);
    // u32 messageId = padType == REGISTERED_PAD_TYPE_GC ? 2306 : 2305;
    // MessageInfo info = {};
    // info.messageIds[0] = messageId;
    // m_categorySwapPlusIcon.setMessageAll(3012, &info);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Down, &m_onUp, false, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Up, &m_onDown, false, false);
    m_arrowUp.setFrontHandler(&m_onSettingsWheelButtonFront, false);
    m_arrowDown.setFrontHandler(&m_onSettingsWheelButtonFront, false);
    m_categorySwap.setFrontHandler(&m_onSettingsWheelButtonFront, true);
    m_categorySwap.setDeselectHandler(&m_onSettingsWheelButtonDeselect, true);
    m_categorySwap.setSelectHandler(&m_onSettingsWheelButtonSelect, true);

    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        m_settingButtons[i].setFrontHandler(&m_onSettingsWheelButtonFront, false);
        m_settingButtons[i].setSelectHandler(&m_onSettingsWheelButtonSelect, false);
        m_settingButtons[i].setDeselectHandler(&m_onSettingsWheelButtonDeselect, false);
        m_settingButtons[i].setVisible(true);
        setMessages(i);
        m_settingButtons[i].m_index = i;
    }

    m_arrowUp.m_index = std::size(m_settingButtons);
    m_arrowDown.m_index = m_arrowUp.m_index + 1;
    m_categorySwap.m_index = m_arrowDown.m_index + 1;

    m_pageTitleText.setMessage(10076);

    m_settingButtons[0].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[1].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[3].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[4].setPaneVisible("pause_w_fade_n", false);

    m_selected = 2;
    m_settingIndex = m_categoryInfo.settingIndex + m_selected;
}

void SettingsPage::onActivate() {
    auto *saveManager = System::SaveManager::Instance();
    u32 settingIndex = m_categoryInfo.settingIndex + m_selected;
    auto &e = SP::ClientSettings::entries[settingIndex];

    if (e.valueNames) {
        instructionText()->setMessageAll(
                e.valueExplanationMessageIds[saveManager->getSetting(settingIndex)]);
    } else {
        u32 chosen = saveManager->getSetting(settingIndex) - e.valueOffset;
        (*m_settingOptionIds[2]).valueChosen = chosen;
        MessageInfo info{};
        info.intVals[0] = chosen;
        instructionText()->setMessageAll(e.valueExplanationMessageIds[0], &info);
    }

    u32 categoryId =
            SP::ClientSettings::categoryMessageIds[static_cast<u32>(m_categoryInfo.categoryIndex)];
    m_categorySwap.setMessageAll(categoryId);

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

    if (button->m_index == m_arrowUp.m_index) {
        onDown(0);
    } else if (button->m_index == m_arrowDown.m_index) {
        onUp(0);
    } else if (button->m_index == m_categorySwap.m_index) {
        push(PageId::SettingsCategorySwap, Anim::Next);
    } else {
        const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[m_settingIndex];
        // TODO: make one of these for each menuType
        if (entry.menuType == SP::Settings::MenuType::Number) {
            push(PageId::SettingsNumberOptions, Anim::Next);
        } else if (entry.menuType == SP::Settings::MenuType::OptionAndDescription) {
            push(PageId::SettingsOptions, Anim::Next);
        } else if (entry.menuType == SP::Settings::MenuType::LargeOption) {
            push(PageId::SettingsLargeOptions, Anim::Next);
        }
    }
    // TODO: set the arrows to be visible again after b is pressed
}

void SettingsPage::setMessages(u32 buttonIndex) {
    auto *saveManager = System::SaveManager::Instance();
    m_settingButtons[buttonIndex].setMessage("setting_name", *m_settingNameIds[buttonIndex]);
    if ((*m_settingOptionIds[buttonIndex]).valueChosen != -1) {
        const SP::ClientSettings::Entry &entry =
                SP::ClientSettings::entries[(*m_settingOptionIds[buttonIndex]).settingIndex];
        MessageInfo info{};
        info.intVals[0] = saveManager->getSetting((*m_settingOptionIds[buttonIndex]).settingIndex) -
                entry.valueOffset;
        m_settingButtons[buttonIndex].setMessage("current_option", entry.valueMessageIds[0], &info);
    } else {
        m_settingButtons[buttonIndex].setMessage("current_option",
                (*m_settingOptionIds[buttonIndex]).messageId);
    }
}

void SettingsPage::setInstructionText() {
    const auto &e = SP::ClientSettings::entries[m_settingIndex];
    auto *saveManager = System::SaveManager::Instance();
    if (e.valueNames) {
        instructionText()->setMessageAll(
                e.valueExplanationMessageIds[saveManager->getSetting(m_settingIndex)]);
    } else {
        u32 chosen = saveManager->getSetting(m_settingIndex) - e.valueOffset;
        (*m_settingOptionIds[2]).valueChosen = chosen;
        MessageInfo info{};
        info.intVals[0] = chosen;
        instructionText()->setMessageAll(e.valueExplanationMessageIds[0], &info);
    }
}

void SettingsPage::onUp(u32 /* localPlayerId */) {
    u32 tempMessageId = *m_settingNameIds[0];
    m_settingNameIds.pop_front();
    m_settingNameIds.push_back(static_cast<const u32 &&>(tempMessageId));
    auto tempOptionId = *m_settingOptionIds[0];
    m_settingOptionIds.pop_front();
    m_settingOptionIds.push_back(static_cast<const optionId &&>(tempOptionId));
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        setMessages(i);
    }
    // TODO: Can we use a modulo operation to make this more efficient?
    if (m_selected == m_settingNameIds.count() - 1) {
        m_selected = 0;
    } else {
        m_selected++;
    }
    m_settingIndex = m_categoryInfo.settingIndex + m_selected;
    setInstructionText();
}

void SettingsPage::onDown(u32 /* localPlayerId */) {
    u32 tempMessageId = *m_settingNameIds[m_settingNameIds.count() - 1];
    m_settingNameIds.pop_back();
    m_settingNameIds.push_front(static_cast<const u32 &&>(tempMessageId));
    auto tempOptionId = (*m_settingOptionIds[m_settingOptionIds.count() - 1]);
    m_settingOptionIds.pop_back();
    m_settingOptionIds.push_front(static_cast<const optionId &&>(tempOptionId));
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        setMessages(i);
    }
    // TODO: Can we use a modulo operation to make this more efficient?
    if (m_selected == 0) {
        m_selected = m_settingNameIds.count() - 1;

    } else {
        m_selected--;
    }
    m_settingIndex = m_categoryInfo.settingIndex + m_selected;
    setInstructionText();
}

void SettingsPage::onSettingsWheelButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
}

void SettingsPage::onSettingsWheelButtonDeselect(PushButton * /* button */,
        u32 /* localPlayerId */) {}

void SettingsPage::setCategoryInfo(u32 categoryIndex) {
    assert(categoryIndex < magic_enum::enum_count<SP::ClientSettings::Category>());

    CategoryInfo info{};
    info.categoryIndex = categoryIndex;
    bool seenYet = false;
    for (u32 i = 0; i < magic_enum::enum_count<SP::ClientSettings::Setting>(); i++) {
        if (static_cast<u32>(SP::ClientSettings::entries[i].category) != categoryIndex) {
            continue;
        }
        if (SP::ClientSettings::entries[i].hidden) {
            continue;
        }
        if (SP::ClientSettings::entries[i].valueCount == 0) {
            continue;
        }
        if (!seenYet) {
            info.settingIndex = i;
            seenYet = true;
        }
        info.settingCount++;
    }
    m_categoryInfo.categoryIndex = info.categoryIndex;
    m_categoryInfo.settingCount = info.settingCount;
    m_categoryInfo.settingIndex = info.settingIndex;
    m_settingIndex = m_categoryInfo.settingIndex + m_selected;
}

void SettingsPage::setCategoryValues(u32 categoryIndex) {
    auto *saveManager = System::SaveManager::Instance();
    for (u32 i = 0; i < SP::ClientSettings::entryCount; i++) {
        const SP::ClientSettings::Entry &entry = SP::ClientSettings::entries[i];
        if (entry.category != static_cast<SP::ClientSettings::Category>(categoryIndex) ||
                entry.valueCount == 0 || entry.hidden) {
            continue;
        }
        u32 chosen = saveManager->getSetting(i) - entry.valueOffset;
        if (entry.valueNames) {
            optionId option;
            option.messageId = entry.valueMessageIds[chosen];
            option.valueChosen = -1;
            option.settingIndex = i;
            m_settingOptionIds.push_back(static_cast<const optionId &&>(option));
        } else {
            optionId option;
            option.messageId = entry.valueMessageIds[0];
            option.valueChosen = chosen;
            option.settingIndex = i;
            m_settingOptionIds.push_back(static_cast<const optionId &&>(option));
        }
        m_settingNameIds.push_back(static_cast<const u32 &&>(entry.messageId));
    }
}

void SettingsPage::clearMessageLists() {
    m_settingNameIds.reset();
    m_settingOptionIds.reset();
    m_selected = 2;
}

void SettingsPage::setMiddleButton(u32 settingIndex) {
    const auto &entry = SP::ClientSettings::entries[settingIndex];
    u32 chosen = System::SaveManager::Instance()->getSetting(settingIndex) - entry.valueOffset;
    (*m_settingOptionIds[2]).messageId = entry.valueMessageIds[chosen];

    if (entry.valueNames) {
        m_settingButtons[2].setMessage("current_option", (*m_settingOptionIds[2]).messageId);
    } else {
        (*m_settingOptionIds[2]).valueChosen = chosen;
        MessageInfo info{};
        info.intVals[0] = chosen;
        m_settingButtons[2].setMessage("current_option", entry.valueMessageIds[0], &info);
    }
    setInstructionText();
}

void SettingsPage::setButtons() {
    u32 categoryId =
            SP::ClientSettings::categoryMessageIds[static_cast<u32>(m_categoryInfo.categoryIndex)];
    m_categorySwap.setMessageAll(categoryId);
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        m_settingButtons[i].setMessage("setting_name", *m_settingNameIds[i]);
        MessageInfo info{};
        info.intVals[0] = (*m_settingOptionIds[i]).valueChosen;
        m_settingButtons[i].setMessage("current_option", (*m_settingOptionIds[i]).messageId, &info);
    }
    setInstructionText();
}

SettingsPage::CategoryInfo SettingsPage::getCategoryInfoGetter() {
    return m_categoryInfo;
}

u32 SettingsPage::getSelectedSetting() {
    return m_selected;
}

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
