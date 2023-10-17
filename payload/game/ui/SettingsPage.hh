#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/UpDownControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"
#include "sp/CircularBuffer.hh"

namespace UI {

class SettingsPage : public Page {
public:
    class IHandler {
    private:
        virtual void dummy_00() {}
        virtual void dummy_04() {}

    public:
        virtual void handle(SettingsPage *page, PushButton *button) = 0;
    };

    template <typename T>
    class Handler : public IHandler {
    public:
        Handler(T *object, void (T::*function)(SettingsPage *, PushButton *)) {
            m_object = object;
            m_function = function;
        }

        void handle(SettingsPage *page, PushButton *button) override {
            (m_object->*m_function)(page, button);
        }

    private:
        T *m_object;
        void (T::*m_function)(SettingsPage *, PushButton *);
    };

    SettingsPage();
    SettingsPage(const SettingsPage &) = delete;
    SettingsPage(SettingsPage &&) = delete;
    ~SettingsPage() override;

    void onInit() override;
    void onActivate() override;

    void setReplacement(PageId pageId);

    struct CategoryInfo {
        u32 categoryIndex;
        u32 categorySheetIndex;
        u32 categorySheetCount;
        u32 settingIndex;
    };

    CategoryInfo getCategoryInfoGetter();
    u32 getSelectedSetting();

protected:
    virtual LayoutUIControl *instructionText() = 0;
    virtual BlackBackControl *blackBack() = 0;

private:
    void onBack(u32 localPlayerId);
    void onUp(u32 localPlayerId);
    void onDown(u32 localPlayerId);

    void onBackButtonFront(PushButton *button, u32 localPlayerId);

    void onSettingsWheelButtonFront(PushButton *button, u32 localPlayerId);
    void onSettingsWheelButtonSelect(PushButton *button, u32 localPlayerId);
    void onSettingsWheelButtonDeselect(PushButton *button, u32 localPlayerId);

    CategoryInfo getCategoryInfo(u32 sheetIndex) const;

    template <typename T>
    using H = typename T::template Handler<SettingsPage>;

    template <typename T>
    using C = typename T::template ChangeHandler<SettingsPage>;

    // UI elements
    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;

    CtrlMenuBackButton m_backButton;
    PushButton m_arrowUp;
    PushButton m_arrowDown;
    PushButton m_categorySwap;

    CategoryInfo m_categoryInfo;

    s32 m_buttonIndex;
    u32 m_selected;
    PushButton m_settingButtons[5];
    SP::CircularBuffer<u32, 32> m_settingNameIds;
    SP::CircularBuffer<s32, 32> m_settingOptionIds;

    H<MultiControlInputManager> m_onBack{this, &SettingsPage::onBack};
    H<MultiControlInputManager> m_onUp{this, &SettingsPage::onUp};
    H<MultiControlInputManager> m_onDown{this, &SettingsPage::onDown};

    H<PushButton> m_onBackButtonFront{this, &SettingsPage::onBackButtonFront};

    H<PushButton> m_onSettingsWheelButtonFront{this, &SettingsPage::onSettingsWheelButtonFront};
    H<PushButton> m_onSettingsWheelButtonSelect{this, &SettingsPage::onSettingsWheelButtonSelect};
    H<PushButton> m_onSettingsWheelButtonDeselect{this,
            &SettingsPage::onSettingsWheelButtonDeselect};

protected:
    IHandler *m_handler = nullptr;
};

class SettingsPagePopup : public SettingsPage {
public:
    SettingsPagePopup();
    ~SettingsPagePopup() override;

    void onInit() override;
    void beforeCalc() override;

    LayoutUIControl *instructionText() override;
    BlackBackControl *blackBack() override;

    void configure(IHandler *handler);
    void pop(Anim anim);

    SettingsPage::CategoryInfo getCategoryInfo();
    u32 getSelectedSetting();

private:
    LayoutUIControl m_instructionText;
    BlackBackControl m_blackBack;
    bool m_popRequested;
};

class MenuSettingsPage : public SettingsPage {
public:
    MenuSettingsPage();
    ~MenuSettingsPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;

    LayoutUIControl *instructionText() override;
    BlackBackControl *blackBack() override;

    void configure(IHandler *handler, PageId replacement);

    SettingsPage::CategoryInfo getCategoryInfo();
    u32 getSelectedSetting();

private:
    CtrlMenuInstructionText m_instructionText;
    PageId m_replacement;
};

} // namespace UI
