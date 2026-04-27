#include "Room.hh"

#include <sp/net/mkw_server/MKW-Server.hh>

namespace Net {

void RoomHandler::init(RoomRole role) {
    // for now, we need to be p2p while waiting for a froom to start
    // since others can't join if everyone is connected to mkw-server
    MKWServer::resetMKWServerInfo();
    REPLACED(init)(role);
}

} // namespace Net
