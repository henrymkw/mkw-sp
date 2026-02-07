#include "RacePacketHolder.hh"

namespace Net {

void RacePacketHolder::reset() {
    for (u8 i = 0; i < 8; i++) {
        getPacketHolder(i)->reset();
    }
}

} // namespace Net