#pragma once

#include <Common.hh>

namespace UI {

class SectionManager {
public:
    u8 getLocalPlayerCount();

    static SectionManager *Instance() {
        return s_instance;
    }

private:

    u8 _00[0x9c - 0x00];
    static SectionManager *s_instance;
};
static_assert(sizeof(SectionManager) == 0x9c);

}