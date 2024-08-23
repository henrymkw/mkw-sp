#pragma once

#include "game/ui/page/SPRankingDownloadPage.hh"

namespace UI {

class SPRankingTopTenDownloadPage : public SPRankingDownloadPage {
public:
    SPRankingTopTenDownloadPage();
    ~SPRankingTopTenDownloadPage() override;

    void onActivate() override;

private:
    void transition(State state) override;
};

} // namespace UI
