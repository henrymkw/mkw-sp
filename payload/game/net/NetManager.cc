#include "NetManager.hh"

extern "C" {
#include <revolution.h>
#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/MatchMaking.h>
}

#include <cstring>

#include <game/system/GameScene.hh>

#include <sp/net/CombinedRACEPacketHeader.hh>

namespace Net {

void NetManager::handleError() {
    // were in this state when were searching/in a room
    // and while in a race. Otherwise, we want to make sure were
    // not connected to the room manager (i dont like this this)
    // is called here but i don't have a better way that checks
    // if were exiting match making.
    if (m_connectionState != ConnectionState::InMatchMaking) {
        resetRoomManagerConnection();
    }
}

void NetManager::updateMatchMakingInfoAndRating() {
    REPLACED(updateMatchMakingInfoAndRating)();

    if (recvFromRoomManager()) {
        // we received from room manager, this we where we handle
    }
}

NetManager *NetManager::construct(EGG::ExpHeap *heap) {
    s_instance = REPLACED(construct)(heap);
    return s_instance;
}

void NetManager::connect() {
    REPLACED(connect)();

    // Turning off the combined packets for now, commenting out code until settings are implemented
    /*
    if (DWC_SetUserRecvCallback(processBufferedRACEPacketCB) == false) {
        SP_LOG("Failed to set buffered user receive callback");
    }
    */
}

bool NetManager::canSendToAid(u8 aid) const {
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
    RacePacketHolder *holder = lastSentRaceBuffer(aid);

    for (u8 i = 0; i < 8; i++) {
        size += holder->getPacketHolder(i)->packetSize();
    }
    return size;
}

void NetManager::sendRacePacket() {
    // these get set once we send to mkw-server
    bool sentSuccessfully = false;
    OSTime sentTime = 0;
    for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
        if (!canSendToAid(aid)) {
            continue;
        }

        PacketHolder<void> *outgoingPacket = m_outgoingRACEPacket[aid];
        // patch the header for MKW server
        if (hasMKWServerAddress) {
            applyMKWServerHeader(reinterpret_cast<u8 *>(outgoingPacket->packet()),
                    m_matchMakingInfos[m_currMMInfo].myAid);
        }

        if (outgoingPacket->packetSize() != 0) {
            u32 crc32 = NETCalcCRC32(outgoingPacket->packet(), outgoingPacket->packetSize());
            Header *header = reinterpret_cast<Header *>(outgoingPacket->packet());
            header->crc32 = crc32;

            // we only want to send once a frame, the loop is mainly here to update the structs for
            // other players.
            if (!sentSuccessfully) {
                sentSuccessfully =
                        DWC_SendUnreliable(aid, reinterpret_cast<u8 *>(outgoingPacket->packet()),
                                outgoingPacket->packetSize());
                sentTime = OSGetTime();
            }
            if (sentSuccessfully) {
                OSTime lastSentTime = m_timeOfLastSentRACE[aid];
                if (lastSentTime != 0) {
                    OSTime delta = sentTime - lastSentTime;
                    m_timeBetweenSendingPackets[aid] = delta;
                }

                m_aidLastSentTo = aid;
                m_timeOfLastSentRACE[aid] = sentTime;
            }

            outgoingPacket->reset();
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
