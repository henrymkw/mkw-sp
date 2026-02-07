#pragma once

#include <Common.hh>

namespace Net {

struct EventRecord {
    u8 _00[0xf8 - 0x00];
};
static_assert(sizeof(EventRecord) == 0xf8);

class EventHandler {
private:
    u8 _0000[0x2b88 - 0x0000];

    static EventHandler *s_instance;
};
static_assert(sizeof(EventHandler) == 0x2b88);

} // namespace Net