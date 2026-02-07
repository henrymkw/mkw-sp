#pragma once

#include <Common.hh>

namespace Net {

// Each value yields a different error screen
enum DisconnectType {
    None = 0x0,
    ErrorCode = 0x1, // standard error code screen
    BadMiiName = 0x2,
    CantJoinFriend = 0x3,     // sends user back to friend roster
    DiskEjected = 0x4,        // user ejected disk during login
    UnrecoverableError = 0x5, // informs user to reboot
};

struct DisconnectInfo {
    DisconnectType type;
    s32 code;
};

} // namespace Net
