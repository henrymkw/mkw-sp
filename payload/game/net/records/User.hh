#pragma once

#include <Common.hh>

namespace Net {

struct UserRecord {
    u8 _00[0xb8 - 0x00];
    u16 vr;
    u16 br;
    u8 _bc[0xc0 - 0xbc];
};
static_assert(sizeof(UserRecord) == 0xc0);

class UserHandler {
public:
    bool prepared();

    u16 myVR();

    u16 myBR();

    static UserHandler *Instance() {
        return s_instance;
    }

private:
    bool m_prepared;
    u8 _001[0x008 - 0x001];
    UserRecord m_recordToSend;
    u8 _0c8[0x9f0 - 0x0c8];

    static UserHandler *s_instance;
};
static_assert(sizeof(UserHandler) == 0x9f0);

} // namespace Net
