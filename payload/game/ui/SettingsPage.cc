#include "SettingsPage.hh"

#include "game/host_system/Scene.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsCategorySwapPage.hh"
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

SettingsPage::SettingsPage() = default;

SettingsPage::~SettingsPage() = default;

void SettingsPage::onInit() {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    bool isRace = Section::GetSceneId(sectionId) == System::SceneId::Race;
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
    for (u8 i = 0; i < std::size(m_settingButtons); i++) {
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
    m_categorySwapPlusIcon.load("control", "CategorySwapButtonPlusIcon", "ClassChange", nullptr);
    if (!isRace) {
        m_menuObiTopNoCurve.load("bg", "MenuObiTopNoCurve", "MenuObiTopNoCurve", nullptr);
        m_menuObiTopNoCurve.m_zIndex = -10.0f;
    }

    auto *sectionManager = UI::SectionManager::Instance();
    auto &registeredPadManager = sectionManager->registeredPadManager();
    u32 flags = registeredPadManager.getFlags(0);
    u32 padType = REGISTERED_PAD_FLAGS_GET_TYPE(flags);
    u32 messageId = padType == REGISTERED_PAD_TYPE_GC ? 2306 : 2305;
    MessageInfo info = {};
    info.messageIds[0] = messageId;
    m_categorySwapPlusIcon.setMessageAll(3012, &info);
    m_categorySwapPlusIcon.m_zIndex = 10.0f;

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Down, &m_onUp, true, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Up, &m_onDown, true, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Option, &m_onCategorySwap, false, false);
    m_arrowUp.setFrontHandler(&m_onSettingsWheelButtonFront, false);
    m_arrowUp.setMessageAll(10469);

    m_arrowDown.setFrontHandler(&m_onSettingsWheelButtonFront, false);
    m_arrowDown.setMessageAll(10470);
    m_categorySwap.setFrontHandler(&m_onSettingsWheelButtonFront, true);
    m_categorySwap.setDeselectHandler(&m_onSettingsWheelButtonDeselect, true);
    m_categorySwap.setSelectHandler(&m_onSettingsWheelButtonSelect, true);

    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    for (u8 i = 0; i < std::size(m_settingButtons); i++) {
        m_settingButtons[i].setFrontHandler(&m_onSettingsWheelButtonFront, false);
        m_settingButtons[i].setSelectHandler(&m_onSettingsWheelButtonSelect, false);
        m_settingButtons[i].setDeselectHandler(&m_onSettingsWheelButtonDeselect, false);
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

    m_selected = 0;
}

void SettingsPage::onActivate() {
    m_settingIndex = m_categoryInfo.settingIndex + m_selected;
    u32 categoryId =
            SP::ClientSettings::categoryMessageIds[static_cast<u32>(m_categoryInfo.categoryIndex)];
    m_categorySwap.setMessageAll(categoryId);
    m_settingButtons[2].selectDefault(0);
    refreshMessages();
}

void SettingsPage::refreshMessages() {
    s32 settingIndex = m_selected - 2;
    auto *saveManager = System::SaveManager::Instance();
    for (u8 i = 0; i < std::size(m_settingButtons); i++) {
        s32 idx = settingIndex + i;
        if (idx < 0) {
            m_settingButtons[i].setVisible(false);
            continue;
        }
        // TODO: Fix C cast
        if (idx > (s32)(m_categoryInfo.settingCount - 1)) {
            m_settingButtons[i].setVisible(false);
            continue;
        }

        m_settingButtons[i].setMessage("setting_name", m_settingNameMessageIds[idx]);
        if (m_settingOptionInfo[idx].valueChosen == -1) {
            m_settingButtons[i].setMessage("current_option", m_settingOptionInfo[idx].messageId);
        } else {
            const auto &entry = SP::ClientSettings::entries[m_settingOptionInfo[idx].settingIndex];
            MessageInfo info{};
            info.intVals[0] = saveManager->getSetting(m_settingOptionInfo[idx].settingIndex);
            m_settingButtons[i].setMessage("current_option", entry.valueMessageIds[0], &info);
        }

        m_settingButtons[i].setVisible(true);
    }
    setInstructionText();
}

u32 SettingsPage::getSettingIndex() {
    return m_settingIndex;
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
    if (button->m_index == m_arrowUp.m_index) {
        onDown(0);
    } else if (button->m_index == m_arrowDown.m_index) {
        onUp(0);
    } else if (button->m_index == m_categorySwap.m_index) {
        push(PageId::SettingsCategorySwap, Anim::Next);
    } else {
        const auto &entry = SP::ClientSettings::entries[m_settingIndex];
        auto *saveManager = System::SaveManager::Instance();

        // Can probably write this more clean but it will do for now
        // Informs the user that vanilla mode must be turned off to change certain settings
        if (entry.vanillaValue) {
            auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VanillaMode>();
            if (setting == SP::ClientSettings::VanillaMode::Enable) {
                auto section = SectionManager::Instance()->currentSection();
                auto messagePopup = section->page<PageId::MessagePopup>();

                messagePopup->reset();
                messagePopup->setWindowMessage(20048);

                push(PageId::MessagePopup, Anim::None);
                return;
            }
        }

        if (entry.menuType == SP::Settings::MenuType::Number) {
            push(PageId::SettingsNumberOptions, Anim::Next);
        } else if (entry.menuType == SP::Settings::MenuType::OptionAndDescription) {
            push(PageId::SettingsOptions, Anim::Next);
        } else if (entry.menuType == SP::Settings::MenuType::LargeOption) {
            push(PageId::SettingsLargeOptions, Anim::Next);
        } else if (entry.menuType == SP::Settings::MenuType::Slider) {
            push(PageId::SettingsSlider, Anim::Next);
        }
    }
}

void SettingsPage::setInstructionText() {
    const auto &entry = SP::ClientSettings::entries[m_settingIndex];
    auto *saveManager = System::SaveManager::Instance();
    if (entry.valueNames) {
        instructionText()->setMessageAll(
                entry.valueExplanationMessageIds[saveManager->getSetting(m_settingIndex)]);
    } else {
        u32 chosen = saveManager->getSetting(m_settingIndex);
        m_settingOptionInfo[m_selected].valueChosen = chosen;
        MessageInfo info{};
        info.intVals[0] = chosen;
        instructionText()->setMessageAll(entry.valueExplanationMessageIds[0], &info);
    }
}

void SettingsPage::onCategorySwap(u32 /*localPlayerId*/) {
    push(PageId::SettingsCategorySwap, Anim::Next);
}

void SettingsPage::onUp(u32 /* localPlayerId */) {
    if (m_selected == m_categoryInfo.settingCount - 1) {
        return;
    }
    m_selected++;
    refreshMessages();
    m_settingIndex = m_categoryInfo.settingIndex + m_selected;
    setInstructionText();
}

void SettingsPage::onDown(u32 /* localPlayerId */) {
    if (m_selected == 0) {
        return;
    }
    m_selected--;
    refreshMessages();
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
        const auto &entry = SP::ClientSettings::entries[i];
        if (static_cast<u32>(entry.category) != categoryIndex) {
            continue;
        }
        if (entry.hidden) {
            continue;
        }
        if (entry.valueCount == 0) {
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
    u32 j = 0;
    for (u32 i = 0; i < SP::ClientSettings::entryCount; i++) {
        const auto &entry = SP::ClientSettings::entries[i];
        if (entry.category != static_cast<SP::ClientSettings::Category>(categoryIndex)) {
            continue;
        }
        if (entry.valueCount == 0) {
            continue;
        }
        if (entry.hidden) {
            continue;
        }
        u32 chosen = saveManager->getSetting(i) - entry.valueOffset;
        optionId option;
        option.settingIndex = i;
        if (entry.valueNames) {
            option.messageId = entry.valueMessageIds[chosen];
            // We set this to -1 to indicate this setting isn't a number
            option.valueChosen = -1;
        } else {
            option.messageId = entry.valueMessageIds[0];
            option.valueChosen = chosen + entry.valueOffset;
        }
        m_settingNameMessageIds[j] = entry.messageId;
        m_settingOptionInfo[j] = option;
        j++;
    }
}

void SettingsPage::changeCategory(u32 categoryIndex) {
    m_selected = 0;

    setCategoryInfo(categoryIndex);
    setCategoryValues(categoryIndex);
    refreshMessages();
    u32 categoryId =
            SP::ClientSettings::categoryMessageIds[static_cast<u32>(m_categoryInfo.categoryIndex)];
    m_categorySwap.setMessageAll(categoryId);
    m_settingButtons[2].selectDefault(0);
}

void SettingsPage::setMiddleButton(u32 settingIndex) {
    const auto &entry = SP::ClientSettings::entries[settingIndex];
    u32 chosen = System::SaveManager::Instance()->getSetting(settingIndex);
    m_settingOptionInfo[m_selected].messageId = entry.valueMessageIds[chosen];

    if (entry.valueNames) {
        m_settingButtons[2].setMessage("current_option", m_settingOptionInfo[m_selected].messageId);
    } else {
        m_settingOptionInfo[m_selected].valueChosen = chosen;
        MessageInfo info{};
        info.intVals[0] = chosen;
        m_settingButtons[2].setMessage("current_option", entry.valueMessageIds[0], &info);
    }
    setInstructionText();
}

SettingsPage::CategoryInfo SettingsPage::getCategoryInfoGetter() {
    // TODO: Remove
    return m_categoryInfo;
}

u32 SettingsPage::getSelectedSetting() {
    // TODO: Remove
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
