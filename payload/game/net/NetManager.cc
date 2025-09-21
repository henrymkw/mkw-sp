#include "NetManager.hh"

extern "C" {
#include <revolution/net/NETDigest.h>
}

#include <cstring>

namespace Net {

bool NetManager::IsValidSendableAid(u8 aid) const {
    if ((((1 << aid) & m_matchMakingInfos[m_currMMInfo].fullAidBitmap) != 0) &&
            (aid != m_matchMakingInfos[m_currMMInfo].myAid)) {
        return true;
    }
    return false;
}

void NetManager::SendRacePacket() {
    // Set the magic and aid here since this is right before the crc32 is calculated
    // This function already loops over all aids to send the packets, so I don't feel great
    // about adding another loop, but this function isn't close to byte matched, so its either this
    // or we cant check the checksum when receiving packets. Hopefully this is temporary.
    for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
        if (IsValidSendableAid(aid)) {
            // patch the first byte of the packet to 0xb (magic)

            // set the 4th byte to our aid. another hack but i dont know how
            // userRecvCallback() can know who sent the packet otherwise
            // vanilla uses the sender's address to determine this, but
            // ofc that completely breaks when the server is always the sender for everyone
            m_outgoingRACEPacket[aid]->PatchPacketForMKWServer(
                    m_matchMakingInfos[m_currMMInfo].myAid);
        }
    }
    REPLACED(SendRacePacket)();
}

void NetManager::ProcessRACEPacket(u8 aid, HEADERPacket *header, u32 size) {
    u32 origCrc32 = header->crc32;
    header->crc32 = 0;
    u32 calcCrc32 = NETCalcCRC32(header, size);

    // make sure the packet isn't corrupted
    if (origCrc32 == calcCrc32) {
        // update time based structs
        OSTime aidLastRecvTime = m_timeOfLastRecvRACE[aid];
        if (aidLastRecvTime != 0) {
            m_timeBetweenRecvPackets[aid] = OSGetTime() - aidLastRecvTime;
        }
        m_timeOfLastRecvRACE[aid] = OSGetTime();

        // data for other packet is right after the header, so add the
        // packet[i] size to this to get a specific offset
        u8 *dataPacketPtr = reinterpret_cast<u8 *>(header);
        for (u32 i = 0; i < std::size(header->packetSizes); i++) {
            if (header->packetSizes[i] != 0) {
                // reset and copy the recieved packet into recv structs
                m_recvRACEPackets[m_lastRecvIdx[aid][i] ^ 1][aid]->getPacketHolder(i)->reset();
                m_recvRACEPackets[m_lastRecvIdx[aid][i] ^ 1][aid]->getPacketHolder(i)->copy(
                        dataPacketPtr, header->packetSizes[i]);

                // increment the data pointer to the next packet offset
                dataPacketPtr += header->packetSizes[i];

                // flip the last recieved buffer idx
                m_lastRecvIdx[aid][i] ^= 1;
            }
        }
    } else {
        SP_LOG("Invalid Checksum!");
    }
}

} // namespace Net
