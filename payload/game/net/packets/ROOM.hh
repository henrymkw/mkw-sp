#pragma once

#include <Common.hh>

namespace Net {

class ROOMHandler {
public:
    enum class MessageType : u8 {
        StartRoom = 1,
        AddFriend = 2,
        JoinRoom = 3,
        Chat = 4,
    };

    enum class RoomRole {
        Host = 0x0,
        Guest = 0x1,
    };

private:
    // Credits: https://wiki.tockdom.com/wiki/Network_Protocol/ROOM

    struct __attribute__((packed)) ROOMPacket {
        MessageType messageType;
        // These are parameters, but its use depends on the message type.
        u16 _1;
        u8 _3;
    };
    static_assert(sizeof(ROOMPacket) == 0x4);

    u8 _00[0x80 - 0x00];

    static ROOMHandler *s_instance;
};
static_assert(sizeof(ROOMHandler) == 0x80);

} // namespace Net
