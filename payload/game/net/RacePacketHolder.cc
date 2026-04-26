#include "RacePacketHolder.hh"

namespace Net {

void RacePacketHolder::reset() {
    for (u8 i = 0; i < 8; i++) {
        holder(i)->reset();
    }
}

u32 RacePacketHolder::size() {
    u32 total = 0;
    for (u8 i = 0; i < 8; i++) {
        total += holder(i)->recordSize();
    }
    return total;
}

} // namespace Net
