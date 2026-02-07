#include "ItemDirector.hh"

extern "C" {
#include <revolution.h>
}

namespace Item {

ItemDirector *ItemDirector::CreateInstance() {
    auto *itemDirector = REPLACED(CreateInstance)();
    if (!itemDirector) {
        SP_LOG("ItemDirector::CreateInstance: Failed to create instance!");
    }
    s_instance = itemDirector;
    return s_instance;
}

void ItemDirector::processInput(bool isPressed) {
    m_players->calcItemWheel(isPressed);
}

} // namespace Item
