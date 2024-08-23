#pragma once

#include "game/ui/page/SPRankingDownloadPage.hh"

namespace UI {

class SPRankingGhostDownloadPage : public SPRankingDownloadPage {
public:
    SPRankingGhostDownloadPage();
    ~SPRankingGhostDownloadPage() override;

    void onActivate() override;

private:
    void transition(State state) override;
};

} // namespace UI
