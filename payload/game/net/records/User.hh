#pragma once

#include <Common.hh>

namespace Net {

struct UserRecord {
    u8 _00[0xc0 - 0x00];
};
static_assert(sizeof(UserRecord) == 0xc0);

class UserHandler {
public:
private:
    u8 _000[0x9f0 - 0x000];

    static UserHandler *s_instance;
};
static_assert(sizeof(UserHandler) == 0x9f0);

} // namespace Net
