#pragma once

#include <Common.hh>
#include <common/TVec3.hh>

#include "game/sound/SoundId.hh"
#include "game/ui/ControlGroup.hh"
#include "game/ui/Font.hh"
#include "game/ui/Layout.hh"
#include "game/ui/MessageGroup.hh"
#include "game/ui/MiiGroup.hh"
#include "game/ui/UIAnimator.hh"

namespace UI {

class UIControl {
public:
    UIControl();
    virtual ~UIControl();
    virtual void dt(s32 type);
    virtual void init();         // 0x8063d004
    virtual void calc();         // 0x8063d044
    virtual void draw(int pass); // 0x8063d084

    void initChildren(u32 count);
    void insertChild(u32 index, UIControl *child);
    void setSoundIds(Sound::SoundId inSoundId, Sound::SoundId outSoundId);
    bool playSound(Sound::SoundId soundId, s32 r5);

protected:
    virtual void initSelf(); // blr
    virtual void calcSelf(); // blr

    struct Element {
        Vec3 position;
        Vec2<float> scale;
        u8 opacity;
        u8 _15[0x18 - 0x15];
    };
    static_assert(sizeof(Element) == 0x18);

    /**
     * @brief Seems to update this->element array. Passed in values are used in only one line. Need
     * to look into more.
     */
    virtual void solveAnim(double currFrame, Element *element);
    virtual void vf_24();
    virtual void vf_28();

public:
    virtual const char *getTypeName();

private:
    virtual void vf_30();
    virtual void vf_34();

public:
    void setVisible(bool visible);
    bool getVisible() const;
    const Page *getPage() const;
    Page *getPage();

private:
    Element m_elementPositions[4];
    ControlGroup *m_controlGroup;

public:
    ControlGroup m_children;
    f32 m_zIndex;

private:
    bool m_isHidden;
    u8 _7d[0x80 - 0x7d];
    u32 m_drawPass;
    bool m_animated; 
    u8 _85[0x88 - 0x85]; // padding
    float m_animStartFrame;
    u32 m_onShowSfxId;
    u32 m_onHideSfxId;
};
static_assert(sizeof(UIControl) == 0x98);

class LayoutUIControl : public UIControl {
public:
    LayoutUIControl();
    ~LayoutUIControl() override;
    void init() override;
    void calc() override;
    void draw(int pass) override;

protected:
    void vf_28() override;
    const char *getTypeName() override;
    void vf_30() override;
    virtual void vf_38();

public:
    void setMessage(const char *pane, u32 messageId, MessageInfo *info = nullptr);
    void setMessageAll(u32 messageId, MessageInfo *info = nullptr);
    void clearMessage(const char *pane);
    void setPicture(const char *dstPane, const char *srcPane);
    bool hasPictureSourcePane(const char *pane);
    void setMiiPicture(const char *pane, MiiGroup *miiGroup, size_t index, u32 preset);
    void setPaneVisible(const char *pane, bool visible);
    void load(const char *dir, const char *file, const char *variant, const char *const *groups);
    void setParentPane(const char *pane);

    UIAnimator m_animator;

protected:
    MainLayout m_mainLayout;

private:
    MessageGroup m_specificMessageGroup;
    MessageGroup m_commonMessageGroup;
    // of type PictureLayout, which I will need to define
    PictureLayout *m_pictureLayout;
    // nw4r::lyt::Pane *
    nw4r::lyt::Pane *m_rootPane; // for documentation
};
static_assert(sizeof(LayoutUIControl) == 0x174);

class LayoutUIControlScaleFade : public LayoutUIControl {
public:
    LayoutUIControlScaleFade();
    ~LayoutUIControlScaleFade() override;
    void solveAnim(double currFrame, Element *element) override;
    void vf_28() override;
    const char *getTypeName() override;
};
static_assert(sizeof(LayoutUIControlScaleFade) == 0x174);

} // namespace UI
