#pragma once

#include <Common.hh>

#include "game/ui/SectionParams.hh"

namespace UI {

class SectionManager {
public:
    u8 getLocalPlayerCount();

    static SectionManager *Instance() {
        return s_instance;
    }

private:
    u8 _00[0x98 - 0x00];
    SectionParams *m_sectionParams;

    static SectionManager *s_instance;
};
static_assert(sizeof(SectionManager) == 0x9c);

} // namespace UI
