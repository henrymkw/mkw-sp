#pragma once

#include "game/ui/ctrl/race/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceBattlePoint : public CtrlRaceBase {
public:
    CtrlRaceBattlePoint();
    ~CtrlRaceBattlePoint() override;

    REPLACE void init() override;
    REPLACE void calcSelf() override;
    void solveAnim(double currFrame, Element *element) override;
    void vf_28() override;
    const char *getTypeName() override;
    void vf_40() override;

    void load(const char *variant, u32 screenId);

private:
    void refresh(u32 score);

    u32 m_score;
};
static_assert(sizeof(CtrlRaceBattlePoint) == 0x19c);

} // namespace UI
