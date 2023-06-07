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

    initChildren(3 + !!blackBack() + std::size(m_settingsWheelButton));
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, instructionText(), 0);
    insertChild(2, &m_backButton, 0);
    
    if (blackBack()) {
        insertChild(3, blackBack(), 0);
    }
    //ADD NEW BUTTONS
    for (u32 i = 0; i < std::size(m_settingsWheelButton); i++) {
        insertChild(3 + !!blackBack() + i, &m_settingsWheelButton[i], 0);
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

    //LOAD NEW BUTTONS (put this in loop later)
    m_settingsWheelButton[0].load("button", "SettingsWheel", "WheelButton0", 0x1, false, false);
    m_settingsWheelButton[1].load("button", "SettingsWheel", "WheelButton1", 0x1, false, false);
    m_settingsWheelButton[2].load("button", "SettingsWheel", "WheelButton2", 0x1, false, false);
    m_settingsWheelButton[3].load("button", "SettingsWheel", "WheelButton3", 0x1, false, false);
    m_settingsWheelButton[4].load("button", "SettingsWheel", "WheelButton4", 0x1, false, false);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);

    m_backButton.setFrontHandler(&m_onBackButtonFront, false);


    for (u32 i = 0; i < std::size(m_settingsWheelButton); i++) {
        m_settingsWheelButton[i].setFrontHandler(&m_onSettingsWheelButtonFront, false);
        m_settingsWheelButton[i].setSelectHandler(&m_onSettingsWheelButtonSelect, false);
        m_settingsWheelButton[i].setDeselectHandler(&m_onSettingsWheelButtonDeselect, false);
    }

    m_pageTitleText.setMessage(10076);

    m_settingsWheelButton[1].setMessage("setting_name", 10004);
    m_settingsWheelButton[1].setMessage("current_option", 10005);

    m_settingsWheelButton[0].setPaneVisible("pause_w_fade_n", false);
    m_settingsWheelButton[1].setPaneVisible("pause_w_fade_n", false);
    m_settingsWheelButton[3].setPaneVisible("pause_w_fade_n", false);
    m_settingsWheelButton[4].setPaneVisible("pause_w_fade_n", false);
}

void SettingsPage::onActivate() {
    instructionText()->setMessageAll(0);

    m_settingsWheelButton[2].selectDefault(0);
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
}

void SettingsPage::onSettingsWheelButtonSelect(PushButton * /* button */, u32 /* localPlayerId */) {
}

void SettingsPage::onSettingsWheelButtonDeselect(PushButton * /* button */, u32 /* localPlayerId */) {
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
