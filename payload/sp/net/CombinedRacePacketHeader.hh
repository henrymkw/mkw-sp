#pragma once

#include <Common.hh>

namespace SP {

struct CombinedRacePacketHeader {
    u8 magic;      // Magic is currently 0xD
    u8 numPackets; // Number of Race packets in this combined packet
    u16 totalSize; // Total size of combined packet
    u16 offsets[]; // Offsets to each packet
};

} // namespace SP
