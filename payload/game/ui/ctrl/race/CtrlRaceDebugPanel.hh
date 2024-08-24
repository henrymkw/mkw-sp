#pragma once

#include "game/ui/ctrl/race/CtrlRaceBase.hh"

namespace UI {

class CtrlRaceDebugPanel : public CtrlRaceBase {
public:
    CtrlRaceDebugPanel();
    ~CtrlRaceDebugPanel() override;
    void calcSelf() override;

    void load();
};

} // namespace UI
