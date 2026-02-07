#include "FontManager.hh"

#include "game/ui/FontManager.h"

namespace UI {

void FontManager::init() {
    REPLACED(init)();

    sDebugFont = m_fonts[1];
}

} // namespace UI