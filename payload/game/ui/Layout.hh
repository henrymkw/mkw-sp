#pragma once

#include <nw4r/lyt/lyt_layout.h>
#include <nw4r/lyt/lyt_pane.hh>

namespace UI {

class Layout {
public:
    Layout();
    nw4r::lyt::Pane *findPaneByName(const char *name);

private:
    u32 _00; // vtable
    lyt_Layout m_layout;
    u8 _24[0x28 - 0x24]; // pointer UIResourceAccessor (LayoutResourceAccessor in pulsar)
};
static_assert(sizeof(Layout) == 0x28);

class MainLayout : public Layout {
public:
    void dt(s32 type);

private:
    char m_lytName[64];
    u8 _40;                       // unknown, unlikely padding
    u8 _41[0x44 - 0x41];          // padding
    void *m_textPaneHandlerArray; // need to define new type
    u32 m_textPaneCount;
    void *m_moviePaneHandlerArray; // need to define new type
    u32 m_moviePaneCount;
    u8 _paneManagerHolder[0x9c - 0x7c]; // Need to define new type
};
static_assert(sizeof(MainLayout) == 0x9c);

// Added for documentation purposes, credit to Pulsar
class PictureLayout : public Layout {
public:
private:
    char m_dirName[64];
    u8 _41; // unknown
    char m_lytName[64];
    u8 _a9;              // unknown, could be padding
    u8 _aa[0xac - 0xaa]; // padding
    PictureLayout *m_next;
};
static_assert(sizeof(PictureLayout) == 0xb0);

} // namespace UI
