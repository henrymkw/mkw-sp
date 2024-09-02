#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class CtrlMenuInstructionText : public LayoutUIControl {
public:
    CtrlMenuInstructionText();
    ~CtrlMenuInstructionText() override;
    void solveAnim(double currFrame, Element *element) override;
    void vf_28() override;
    const char *getTypeName() override;

    REPLACE void load();
    void setMessage(s32 messageId, MessageInfo *info = nullptr);
};
static_assert(sizeof(CtrlMenuInstructionText) == 0x174);

} // namespace UI
