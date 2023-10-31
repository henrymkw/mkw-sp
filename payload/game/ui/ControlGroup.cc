#include "ControlGroup.hh"

#include "game/ui/UIControl.hh"

#include <cstring>

namespace UI {

void ControlGroup::init() {
    for (s32 i = 0; i < m_size; i++) {
        assert(m_data[i]);
        m_data[i]->init();
    }
}

ControlGroup::~ControlGroup() {
    dt(-1);
}

void ControlGroup::insert(s32 index, UIControl *child, u32 drawPass) {
    REPLACED(insert)(index, child, drawPass);
}

void ControlGroup::logDebug(int depth) {
    const int MAX_DEPTH = 16;
    if (depth > MAX_DEPTH) {
        return;
    }
    char spaces[MAX_DEPTH * 2 + 1];
    memset(spaces, '-', depth * 2);
    spaces[depth * 2] = '\0';
    for (s32 i = 0; i < m_size; ++i) {
        UIControl *it = m_dataSorted[i];
        const char *name = it->getTypeName();
        if (it->m_children.m_size == 0) {
            OSReport("    |-%s%s\n", spaces, name);
        } else {
            OSReport("    |-%s%s(#Children: %d)\n", spaces, name, it->m_children.m_size);
            it->m_children.logDebug(depth + 1);
        }
    }
}

} // namespace UI
