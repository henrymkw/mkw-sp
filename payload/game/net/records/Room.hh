#pragma once

#include <Common.hh>

namespace Net {

class RoomHandler {
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
    struct __attribute__((packed)) RoomRecord {
        MessageType messageType;
        // These are parameters, but its use depends on the message type.
        u16 _1;
        u8 _3;
    };
    static_assert(sizeof(RoomRecord) == 0x4);

public:
private:
    // Credits: https://wiki.tockdom.com/wiki/Network_Protocol/ROOM

    REPLACE void init(RoomRole role);
    void REPLACED(init)(RoomRole role);

    u8 _00[0x80 - 0x00];

    static RoomHandler *s_instance;
};
static_assert(sizeof(RoomHandler) == 0x80);

} // namespace Net
