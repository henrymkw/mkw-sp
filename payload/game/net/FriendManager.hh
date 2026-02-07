#pragma once

#include <Common.hh>

namespace Net {

#pragma once

#include <Common.hh>

namespace Net {

class FriendRosterManager {
private:
    static FriendRosterManager *s_instance;

    u8 _000[0x3e8 - 0x000];
};
static_assert(sizeof(FriendRosterManager) == 0x3e8);

} // namespace Net
