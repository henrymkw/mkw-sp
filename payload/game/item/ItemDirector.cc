#include "ItemDirector.hh"

extern "C" {
#include <revolution.h>
}

namespace Item {

void ItemDirector::processInput(bool isPressed) {
    m_players->calcItemWheel(isPressed);
}

} // namespace Item
