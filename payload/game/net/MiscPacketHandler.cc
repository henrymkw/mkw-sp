#include "MiscPacketHandler.hh"

namespace Net {

// REPLACED was used for the sake of testing
void MiscPacketHandler::updateAsRacer() {
    REPLACED(updateAsRacer)();
}

// REPLACED was used for the sake of testing
bool MiscPacketHandler::isEveryoneInRace() const {
    return REPLACED(isEveryoneInRace)();
}

} // namespace Net
