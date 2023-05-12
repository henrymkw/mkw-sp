#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/Page.hh"
#include "game/ui/UpDownControl.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuInstructionText.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

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

protected:
    virtual LayoutUIControl *instructionText() = 0;
    virtual BlackBackControl *blackBack() = 0;

private:
    struct CategoryInfo {
        u32 categoryIndex;
        u32 categorySheetIndex;
        u32 categorySheetCount;
        u32 settingIndex;
    };

    void onBack(u32 localPlayerId);
    void onCategoryControlFront(UpDownControl *control, u32 localPlayerId);
    void onCategoryControlSelect(UpDownControl *control, u32 localPlayerId);
    void onCategoryValueChange(TextUpDownValueControl::TextControl *text, u32 index);
    void onSettingControlChange(UpDownControl *control, u32 localPlayerId, u32 index);
    void onSettingControlFront(UpDownControl *control, u32 localPlayerId);
    void onSettingControlSelect(UpDownControl *control, u32 localPlayerId);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    u32 getSheetCount() const;
    CategoryInfo getCategoryInfo(u32 sheetIndex) const;

    void onSettingsWheelButtonFront(PushButton *button, u32 localPlayerId);
    void onSettingsWheelButtonSelect(PushButton *button, u32 localPlayerId);
    void onSettingsWheelButtonDeselect(PushButton *button, u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<SettingsPage>;

    template <typename T>
    using C = typename T::template ChangeHandler<SettingsPage>;

    //UI elements
    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    UpDownControl m_categoryControl;
    TextUpDownValueControl m_categoryValue;
    UpDownControl m_settingControls[6];
    TextUpDownValueControl m_settingValues[6];
    CtrlMenuBackButton m_backButton;

//Array should work for this?
    PushButton m_settingsWheelButton[5];

    //Input handlers
    H<MultiControlInputManager> m_onBack{this, &SettingsPage::onBack};
    H<UpDownControl> m_onCategoryControlFront{this, &SettingsPage::onCategoryControlFront};
    H<UpDownControl> m_onCategoryControlSelect{this, &SettingsPage::onCategoryControlSelect};
    C<TextUpDownValueControl> m_onCategoryValueChange{this, &SettingsPage::onCategoryValueChange};
    C<UpDownControl> m_onSettingControlChange{this, &SettingsPage::onSettingControlChange};
    H<UpDownControl> m_onSettingControlFront{this, &SettingsPage::onSettingControlFront};
    H<UpDownControl> m_onSettingControlSelect{this, &SettingsPage::onSettingControlSelect};
    H<PushButton> m_onBackButtonFront{this, &SettingsPage::onBackButtonFront};

//Figure out how this works with an array of pushbuttons.
    H<PushButton> m_onSettingsWheelButtonFront{this, &SettingsPage::onSettingsWheelButtonFront};
    H<PushButton> m_onSettingsWheelButtonSelect{this, &SettingsPage::onSettingsWheelButtonSelect};
    H<PushButton> m_onSettingsWheelButtonDeselect{this, &SettingsPage::onSettingsWheelButtonDeselect};


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

private:
    CtrlMenuInstructionText m_instructionText;
    PageId m_replacement;
};

} // namespace UI
