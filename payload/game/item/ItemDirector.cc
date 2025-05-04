#include "ItemDirector.hh"

namespace Item {

void ItemDirector::processInput(bool isPressed) {
    m_players->calcItemWheel(isPressed);
}

ItemDirector *ItemDirector::Instance() {
    return s_instance;
}

} // namespace Item
