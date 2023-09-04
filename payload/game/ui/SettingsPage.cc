#include "SettingsPage.hh"

#include "game/host_system/Scene.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/page/RaceMenuPage.hh"

namespace UI {

SettingsPage::SettingsPage() = default;

SettingsPage::~SettingsPage() = default;

void SettingsPage::onInit() {
    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    auto m_categoryInfo = getCategoryInfo(1);

    auto category = static_cast<SP::ClientSettings::Category>(m_categoryInfo.categoryIndex);
    for (u32 i = 0; i < SP::ClientSettings::entryCount; i++) {
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
    }

    initChildren(3 + std::size(m_settingButtons) + !!blackBack());
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, instructionText(), 0);
    insertChild(2, &m_backButton, 0);

    if (blackBack()) {
        insertChild(3, blackBack(), 0);
    }
    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        insertChild(3 + i + !!blackBack(), &m_settingButtons[i], 0);
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

    for (u32 i = 0; i < std::size(m_settingButtons); i++) {
        char variant[13];
        snprintf(variant, sizeof(variant), "WheelButton%u", i);
        m_settingButtons[i].load("button", "SettingsWheel", variant, 0x1, false, false);
    }

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

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

    m_pageTitleText.setMessage(10076);

    m_settingButtons[0].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[1].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[3].setPaneVisible("pause_w_fade_n", false);
    m_settingButtons[4].setPaneVisible("pause_w_fade_n", false);
}

void SettingsPage::onActivate() {
    instructionText()->setMessageAll(0);

    m_settingButtons[2].selectDefault(0);
    m_buttonIndex = 2;
    m_selected = 2;
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

void SettingsPage::onSettingsWheelButtonFront(PushButton * /* button */, u32 /* localPlayerId */) {
    push(PageId::SettingsOptions, Anim::Next);
}

void SettingsPage::onSettingsWheelButtonSelect(PushButton *button, u32 /* localPlayerId */) {
    bool isMovingDown =
            ((button->m_index == 0 && m_buttonIndex == 4) || (button->m_index > m_buttonIndex)) &&
            !(button->m_index == 4 && m_buttonIndex == 0);

    if (isMovingDown) {
        u32 temp = *m_settingNameIds[m_settingNameIds.count() - 1];
        m_settingNameIds.pop_back();
        m_settingNameIds.push_front(static_cast<const u32 &&>(temp));
        for (u32 i = 0; i < std::size(m_settingButtons); i++) {
            m_settingButtons[i].setMessage("setting_name", *m_settingNameIds[i]);
        }
        if (m_selected == 0) {
            m_selected = m_settingNameIds.count() - 1;

        } else {
            m_selected--;
        }
    } else {
        u32 temp = *m_settingNameIds[0];
        m_settingNameIds.pop_front();
        m_settingNameIds.push_back(static_cast<const u32 &&>(temp));
        for (u32 i = 0; i < std::size(m_settingButtons); i++) {
            m_settingButtons[i].setMessage("setting_name", *m_settingNameIds[i]);
        }
        if (m_selected == m_settingNameIds.count() - 1) {
            m_selected = 0;
        } else {
            m_selected++;
        }
    }
    m_buttonIndex = button->m_index;
}

void SettingsPage::onSettingsWheelButtonDeselect(PushButton * /* button */,
        u32 /* localPlayerId */) {}

SettingsPage::CategoryInfo SettingsPage::getCategoryInfo(u32 sheetIndex) const {
    u32 controlCount = std::size(m_settingButtons);
    CategoryInfo info{};
    u32 sheetCount = 0;
    u32 categoryCount = magic_enum::enum_count<SP::ClientSettings::Category>();
    for (; info.categoryIndex < categoryCount; info.categoryIndex++) {
        auto category = static_cast<SP::ClientSettings::Category>(info.categoryIndex);
        u32 settingCount = 0;
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

} // namespace UI
