#include "SectionManager.hh"

namespace UI {

u8 SectionManager::getLocalPlayerCount() const {
    return m_sectionParams->getLocalPlayerCount();
}

} // namespace UI
