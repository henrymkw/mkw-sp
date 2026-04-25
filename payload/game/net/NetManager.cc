#include "NetManager.hh"

#include "game/net/records/Room.hh"

extern "C" {
#include <revolution.h>
#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/MatchMaking.h>
}

#include <cstring>

#include "game/ui/SectionManager.hh"

namespace Net {

SearchRegion NetManager::getSearchRegion() {
    // This would need to be modified for custom regions
    switch (REGION) {
    case REGION_P:
        return SEARCH_REGION_EU;
    case REGION_E:
        return SEARCH_REGION_NA;
    case REGION_J:
        return SEARCH_REGION_JP;
    case REGION_K:
        return SEARCH_REGION_KOR;
    default:
        assert("Invalid region!");
        return SEARCH_REGION_NONE;
    }
}

void NetManager::connectToAnybodyAsync() {
    if (connectToRoomManager()) {
        // In vanilla, this function only gets called when searching for public rooms
        // Because of that, we can assume that a non-ww room type is regional and can't be a private
        // room
        bool isWW =
                m_roomType == RoomType::VersusWorldWide || m_roomType == RoomType::BattleWorldWide;
        SearchRegion region = isWW ? SEARCH_REGION_WW : getSearchRegion();

        bool isVS =
                m_roomType == RoomType::VersusWorldWide || m_roomType == RoomType::VersusRegional;
        GameMode mode = isVS ? GAME_MODE_VS : GAME_MODE_BATTLE;

        if (!sendSearchRoomRequest(region, mode)) {
            SP_LOG("Search room request failed! Region: %d, Mode: %d", region, mode);
        }
    }
}

void NetManager::connectToGameServerFromGroupId() {
    u32 friendId = currentMMInfo()->hostFriendId;
    FriendStatusIcon icon = getFriendStatusIcon(friendId);
    SearchRegion searchRegion = SEARCH_REGION_NONE;

    switch (icon) {
    case FriendStatusIcon::WorldWideVersus:
    case FriendStatusIcon::WorldWideBattle:
        searchRegion = SEARCH_REGION_WW;
        break;
    case FriendStatusIcon::JoinableRegionalVS:
    case FriendStatusIcon::JoinableRegionalBattle:
        searchRegion = getSearchRegion();
        break;
    default:
        SP_LOG("Mismatching search region as friend trying to join!");
        break;
    }

    s32 friendProfileId = DWCi_GetProfileIDFromList(friendId);
    if (connectToRoomManager()) {
        sendJoinFriendRequest(friendProfileId, searchRegion);
    }
}

void NetManager::updateAddedFriendsCallback(void *r3, void *r4, void *r5) {
    REPLACED(updateAddedFriendsCallback)(r3, r4, r5);

    if (!connectToRoomManager()) {
        SP_LOG("Failed to connect to room manager!");
        return;
    }

    u8 localPlayerCount = UI::SectionManager::Instance()->getLocalPlayerCount();
    bool sendResult = sendLocalPlayerCount(localPlayerCount);
    if (!sendResult) {
        SP_LOG("Sending localPlayerCount failed!");
    }
}

void NetManager::scheduleShutdown() {
    m_shutdownScheduled = true;

    resetRoomManagerConnection();
}

void NetManager::init(u8 localPlayerCount) {
    REPLACED(init)(localPlayerCount);

    // initialize the unique packets buffer
    m_outgoingUniquePackets.reset();
}

void NetManager::cancelMatching() {
    // Reset the two race packet handlers active during the globe scene
    if (auto *rh1Handler = RH1Handler::Instance()) {
        rh1Handler->reset();
    }
    if (auto *roomHandler = RoomHandler::Instance()) {
        roomHandler->reset();
    }

    // This will exit the InMatchMaking state during the next iteration of the main loop
    m_voteMMSuspension = VoteMatchMakingSuspended::Disconnected;

    // inform wfc-server we're leaving the room
    sendLeaveFroomRequest();
}

void NetManager::updateMatchMakingInfoAndRating() {
    REPLACED(updateMatchMakingInfoAndRating)();

    if (recvFromRoomManager()) {
        // we received from room manager, this we where we handle
    }
}

const MatchMakingInfo *NetManager::currentMMInfo() const {
    return &m_matchMakingInfos[m_currMMInfo];
}

u8 NetManager::myAid() const {
    return currentMMInfo()->myAid;
}

u32 NetManager::numAids() const {
    return currentMMInfo()->numAids;
}

bool NetManager::aidInUse(u8 aid) const {
    return currentMMInfo()->availableAids.on(aid);
}

bool NetManager::canSendToAid(u8 aid) const {
    return aidInUse(aid) && aid != myAid();
}

bool NetManager::hasFoundMatch() const {
    // We're in a match if my aid is used and theres more than one aid.
    return aidInUse(myAid()) && numAids() > 1;
}

u32 NetManager::getRacePacketSize(u8 aid) {
    u32 size = 0;
    RacePacketHolder *holder = lastSentRaceBuffer(aid);

    for (u8 i = 0; i < 8; i++) {
        size += holder->holder(i)->recordSize();
    }
    return size;
}

void NetManager::createRacePacket() {
    if (!hasFoundMatch()) {
        return;
    }

    // Reset the outgoing packets. new frame, new packets
    m_outgoingUniquePackets.reset();

    for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
        if (!canSendToAid(aid)) {
            continue;
        }

        // Get the current send buffer then flip it so subsequent record exports work
        u8 sendBufferIdx = m_lastSendIdx[aid];
        m_lastSendIdx[aid] ^= 1;

        // Get the send buffer for this aid
        RacePacketHolder *sendBuffer = m_sendRacePackets[sendBufferIdx][aid];

        // Form a header
        Header header;
        memset(&header, 0, sizeof(Header));

        // Set the outgoing header's sizes
        u32 headerSizesMask = 0;
        for (u8 i = 0; i < 8; i++) {
            RecordHolder<void> *rh = sendBuffer->holder(i);

            // Header (idx == 0) always exists and has a size of 0x10
            // Use the actual record size for all other records
            u8 size = i == 0 ? 0x10 : rh->recordSize();
            if (size != 0) {
                headerSizesMask |= 1 << i;
            }
            header.setRecordSize(i, size);
        }

        s32 headerSizesIdx = m_outgoingUniquePackets.maskIdx(headerSizesMask);
        if (headerSizesIdx != -1) {
            // If this exact packet was already seen, just add this aid to the recipient bitmap
            m_outgoingUniquePackets.setRecvAid(headerSizesIdx, aid);
            continue;
        }

        // copy the header to the buffer that actually gets sent
        sendBuffer->header()->copy(&header, sizeof(Header));

        RecordHolder<void> *outgoing = m_outgoingRacePacket[aid];
        outgoing->reset();

        // copy the records in the sendBuffer to the outgoing buffer
        for (u8 i = 0; i < 8; i++) {
            RecordHolder<void> *record = sendBuffer->holder(i);
            if (record->recordSize() != 0) {
                outgoing->append(record->record(), record->recordSize());
                record->reset();
            }
        }

        // We found a unique packet, so push it to be sent
        if (!m_outgoingUniquePackets.push(outgoing, headerSizesMask, aid, myAid())) {
            SP_LOG("Pushing a new packet failed!");
        }
    }
}

void NetManager::sendRacePacket() {
    for (u8 i = 0; i < m_outgoingUniquePackets.count(); i++) {
        sendRacePacketToMKWServer(i);
    }
    m_outgoingUniquePackets.reset();
}

bool NetManager::sendRacePacketToMKWServer(u8 packetIdx) {
    OSTime sentTime = 0;
    const SP::Packet *outgoingPacket = m_outgoingUniquePackets[packetIdx];

    if (outgoingPacket == nullptr) {
        return false;
    }

    if (outgoingPacket->data == nullptr) {
        return false;
    }

    if (outgoingPacket->data->recordSize() == 0) {
        return false;
    }

    // Calc the crc32
    u32 crc32 = NETCalcCRC32(outgoingPacket->data->record(), outgoingPacket->data->recordSize());
    Header *header = reinterpret_cast<Header *>(outgoingPacket->data->record());
    header->crc32 = crc32;

    void *recordToSend = outgoingPacket->data->record();

    // Try to send
    bool sendResult =
            trySendRacePacketToMKWServer(recordToSend, outgoingPacket->data->recordSize());

    // update time-based send members
    if (sendResult) {
        OSTime lastSentTime = m_timeOfLastSentRace[packetIdx];
        if (lastSentTime != 0) {
            OSTime delta = sentTime - lastSentTime;
            m_timeBetweenSendingPackets[packetIdx] = delta;
        }

        m_aidLastSentTo = packetIdx;
        m_timeOfLastSentRace[packetIdx] = sentTime;
    }

    // always reset the outgoing buffer
    outgoingPacket->data->reset();
    return sendResult;
}

void NetManager::processRacePacket(u8 aid, u8 *packet, u32 size) {
    Header *header = reinterpret_cast<Header *>(packet);
    u32 origCrc32 = header->crc32;
    header->crc32 = 0;
    u32 calcCrc32 = NETCalcCRC32(header, size);

    // make sure the packet isn't corrupted
    if (origCrc32 == calcCrc32) {
        // update time based structs
        OSTime aidLastRecvTime = m_timeOfLastRecvRace[aid];
        if (aidLastRecvTime != 0) {
            m_timeBetweenRecvPackets[aid] = OSGetTime() - aidLastRecvTime;
        }
        m_timeOfLastRecvRace[aid] = OSGetTime();

        // data for other packet is right after the header, so add the
        // packet[i] size to this to get a specific offset
        u8 *dataPacketPtr = reinterpret_cast<u8 *>(header);
        for (u32 i = 0; i < std::size(header->recordSizes); i++) {
            if (header->recordSizes[i] != 0) {
                // reset and copy the recieved packet into recv structs
                m_recvRacePackets[m_lastRecvIdx[aid][i] ^ 1][aid]->holder(i)->reset();
                m_recvRacePackets[m_lastRecvIdx[aid][i] ^ 1][aid]->holder(i)->copy(dataPacketPtr,
                        header->recordSizes[i]);

                // increment the data pointer to the next packet offset
                dataPacketPtr += header->recordSizes[i];

                // flip the last recieved buffer idx
                m_lastRecvIdx[aid][i] ^= 1;
            }
        }
    } else {
        SP_LOG("Invalid Checksum!");
    }
}

} // namespace Net
