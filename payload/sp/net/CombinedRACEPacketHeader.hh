#pragma once

#include <Common.hh>

namespace SP {

struct CombinedRACEPacketHeader {
    u8 magic;      // Magic is currently 0xD
    u8 numPackets; // Number of RACE packets in this combined packet
    u16 totalSize; // Total size of combined packet
    u16 offsets[]; // Offsets to each packet
};

} // namespace SP