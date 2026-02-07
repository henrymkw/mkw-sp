#include "NetManager.hh"

extern "C" {
#include <revolution.h>
#include <sp/net/MKW-Server.h>
}

#include <cstring>

#include <sp/net/CombinedRACEPacketHeader.hh>

namespace Net {

NetManager *NetManager::construct(EGG::ExpHeap *heap) {
    s_instance = REPLACED(construct)(heap);
    return s_instance;
}

void NetManager::connect() {
    REPLACED(connect)();

    if (DWC_SetUserRecvCallback(processBufferedRACEPacketCB) == false) {
        SP_LOG("Failed to set buffered user receive callback");
    }
}

bool NetManager::isValidSendableAid(u8 aid) const {
    if ((((1 << aid) & m_matchMakingInfos[m_currMMInfo].availableAids) != 0) &&
            (aid != m_matchMakingInfos[m_currMMInfo].myAid)) {
        return true;
    }
    return false;
}

bool NetManager::hasFoundMatch() const {
    bool inMatch = false;

    bool isMyAidInMatch = (1 << m_matchMakingInfos[m_currMMInfo].myAid) &
            m_matchMakingInfos[m_currMMInfo].availableAids;
    // were in a match if my aid is in the room and we have connected to another
    // console
    if (isMyAidInMatch && m_matchMakingInfos[m_currMMInfo].numConnectedConsoles > 1) {
        inMatch = true;
    }
    return inMatch;
}

u32 NetManager::getRACEPacketSize(u8 aid) {
    u32 size = 0;
    RacePacketHolder *holder = lastSentRACEBuffer(aid);

    for (u8 i = 0; i < 8; i++) {
        size += holder->getPacketHolder(i)->packetSize();
    }
    return size;
}

void NetManager::formRacePacket() {
    REPLACED(formRacePacket)();
}

void NetManager::sendRacePacket() {
    OSTime timeSinceLastSendMs;

    if (m_aidLastSentTo == INVALID_AID) {
        timeSinceLastSendMs = MAX_TIME;
    } else {
        OSTime lastSentTime = m_timeOfLastSentRACE[m_aidLastSentTo];
        if (lastSentTime == 0) {
            timeSinceLastSendMs = MAX_TIME;
        } else {
            OSTime delta = OSGetTime() - lastSentTime;
            timeSinceLastSendMs = OSTicksToMilliseconds(delta);
        }
    }

    if (timeSinceLastSendMs <= 16) {
        return;
    }

    u8 currentAid = m_aidLastSentTo;

    for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
        currentAid++;
        if (currentAid >= MAX_PLAYER_COUNT) {
            currentAid = 0;
        }

        if (!isValidSendableAid(currentAid)) {
            continue;
        }

        PacketHolder<void> *outgoingPacket = m_outgoingRACEPacket[currentAid];

        // patch the header for MKW server
        if (hasMKWServerAddress) {
            applyMKWServerHeader(reinterpret_cast<u8 *>(outgoingPacket->packet()),
                    m_matchMakingInfos[m_currMMInfo].myAid);
        }

        bool sentSuccessfully = false;

        if (outgoingPacket->packetSize() != 0) {
            u32 crc32 = NETCalcCRC32(outgoingPacket->packet(), outgoingPacket->packetSize());
            Header *header = reinterpret_cast<Header *>(outgoingPacket->packet());
            header->crc32 = crc32;

            bool sendResult = trySendRACEPacketToMKWServer(outgoingPacket->packet(),
                    outgoingPacket->packetSize());

            if (!sendResult) {
                sendResult = DWC_SendUnreliable(currentAid, outgoingPacket->packet(),
                        outgoingPacket->packetSize());
            }

            if (sendResult == true) {
                OSTime lastSentTime = m_timeOfLastSentRACE[currentAid];

                if (lastSentTime != 0) {
                    OSTime delta = OSGetTime() - lastSentTime;
                    m_timeBetweenSendingPackets[currentAid] = delta;
                }

                m_aidLastSentTo = currentAid;
                m_timeOfLastSentRACE[currentAid] = OSGetTime();

                sentSuccessfully = true;
            }

            outgoingPacket->reset();
        }

        if (sentSuccessfully) {
            break;
        }
    }
}

void NetManager::processBufferedRACEPacket(u8 *buffer, u32 size) {
    SP::CombinedRACEPacketHeader *combinedRACEPacketHeader =
            reinterpret_cast<SP::CombinedRACEPacketHeader *>(buffer);
    u32 processedSize = 0;
    for (u8 i = 0; i < combinedRACEPacketHeader->numPackets; i++) {
        u16 packetOffset = combinedRACEPacketHeader->offsets[i];
        Header *header = reinterpret_cast<Header *>(
                reinterpret_cast<u8 *>(combinedRACEPacketHeader) + packetOffset);
        if (header->magic != 0xb) {
            SP_LOG("Invalid Buffered RACE Packet Magic!");
            return;
        }
        u32 packetSize = header->getSize();

        if (processedSize + packetSize <= size) {
            processRACEPacket(header->aid, reinterpret_cast<u8 *>(header), packetSize);
            processedSize += packetSize;
        } else {
            SP_LOG("Buffered RACE Packet processing out of bounds!");
            break;
        }
    }
}

void NetManager::processRACEPacket(u8 aid, u8 *packet, u32 size) {
    Header *header = reinterpret_cast<Header *>(packet);
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

void processBufferedRACEPacketCB(u8 aid, u8 *buffer, u32 size) {
    if (hasMKWServerAddress || aid == 0xff) {
        Net::NetManager::Instance()->processBufferedRACEPacket(buffer, size);
    } else {
        Net::NetManager::Instance()->processRACEPacket(aid, buffer, size);
    }
}
