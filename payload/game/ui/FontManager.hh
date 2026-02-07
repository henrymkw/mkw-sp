#pragma once

#include <Common.hh>

#include "game/ui/Font.h"

namespace UI {

class FontManager {
public:
private:
    REPLACE void init();
    void REPLACED(init)();

    Font *m_fonts[6];
};
static_assert(sizeof(FontManager) == 0x18);

} // namespace UI