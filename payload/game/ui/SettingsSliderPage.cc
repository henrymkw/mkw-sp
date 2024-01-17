#include "SettingsSliderPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/SettingsPage.hh"

namespace UI {

SettingsSliderPage::SettingsSliderPage() = default;

SettingsSliderPage::~SettingsSliderPage() = default;

void SettingsSliderPage::onInit() {
    m_inputManager.init(0x01, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Y);

    // TODO: Fix hard code
    m_numOptions = 11;
    m_chosen = 0;

    initChildren(6);
    insertChild(0, &m_backButton, 0);
    insertChild(1, &m_blackBackControl, 0);
    insertChild(2, &m_instructionText, 0);
    insertChild(3, &m_settingTitleText, 0);
    insertChild(4, &m_option, 0);
    insertChild(5, &m_scrollBar, 0);

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (Section::GetSceneId(sectionId) == System::SceneId::Race) {
        m_backButton.load("message_window", "Back", "ButtonBack", 0x1, false, true);
    } else {
        m_backButton.load("button", "Back", "ButtonBackPopup", 0x1, false, true);
    }

    m_blackBackControl.load("control", "RankingBlackBack", "RankingBlackBack");
    m_blackBackControl.m_zIndex = -150.0f;
    m_instructionText.load("bg", "BlackBackObiInstructionText", "RaceObiInstructionText", nullptr);
    m_settingTitleText.load("button", "SubMenuSettingTitle", "SettingTitleTop", nullptr);
    m_option.load("button", "SliderPageNumberDisplay", "NumberDisplay", NULL);
    m_scrollBar.load(m_numOptions, m_chosen, "button", "CourseSelectScrollBar",
            "SliderSettingScrollBar", 0x1, false, false);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);
    m_scrollBar.setChangeHandler(&m_onScrollBarChange);

    m_backButton.selectDefault(0);
}

void SettingsSliderPage::onActivate() {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    auto *saveManager = System::SaveManager::Instance();

    u32 settingIndex = settingsPage->getSettingIndex();
    const auto &entry = SP::ClientSettings::entries[settingIndex];
    m_settingTitleText.setMessageAll(entry.messageId);

    m_numOptions = entry.valueCount;
    m_chosen = saveManager->getSetting(settingIndex);
    m_scrollBar.reconfigure(m_numOptions, m_chosen, 0x1);

    MessageInfo info{};
    info.intVals[0] = m_chosen;
    info.intVals[1] = m_numOptions - 1;
    m_option.setMessageAll(2009, &info);
}

void SettingsSliderPage::onBack(u32 /* localPlayerId */) {
    startReplace(Anim::Prev, 0.0f);
}

void SettingsSliderPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    f32 delay = button->getDelay();
    startReplace(Anim::Prev, delay);
}

void SettingsSliderPage::onScrollBarChange(ScrollBar * /* scrollBar */, u32 /* localPlayerId*/,
        u32 chosen) {
    auto *settingsPage = SectionManager::Instance()->currentSection()->page<PageId::MenuSettings>();
    auto *saveManager = System::SaveManager::Instance();

    u32 settingIndex = settingsPage->getSettingIndex();
    m_chosen = chosen;
    saveManager->setSetting(settingIndex, m_chosen);
    settingsPage->setMiddleButton(settingIndex);

    MessageInfo info{};
    info.intVals[0] = m_chosen;
    info.intVals[1] = m_numOptions - 1;
    m_option.setMessageAll(2009, &info);
}

} // namespace UI
