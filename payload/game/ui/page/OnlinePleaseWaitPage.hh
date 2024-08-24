#pragma once

#include <Common.hh>

#include "game/ui/page/MenuPage.hh"

namespace UI {

/**
 * @brief PageId: 0x48. Will need to investigate, SP calls this is PleaseWait, but Pulsar says "tied
 * to one OK button "Start?"". Going with Pulsar's size
 *
 */
class OnlinePleaseWaitPage : public MenuPage {
private:
    u8 _430[0x6cc - 0x430];
};
static_assert(sizeof(OnlinePleaseWaitPage) == 0x6cc);

} // namespace UI