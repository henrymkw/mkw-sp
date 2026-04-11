#include "NetManager.hh"

#include "game/net/records/Room.hh"

extern "C" {
#include <revolution.h>
#include <sp/net/mkw_server/MKW-Server.h>
#include <sp/net/mkw_server/MatchMaking.h>
}

#include <cstring>

#include <game/system/GameScene.hh>

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

void NetManager::handleError() {
    REPLACED(handleError)();

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

const MatchMakingInfo *NetManager::currentMMInfo() const {
    return &m_matchMakingInfos[m_currMMInfo];
}

u8 NetManager::myAid() const {
    return currentMMInfo()->myAid;
}

u32 NetManager::availableAids() const {
    return currentMMInfo()->availableAids;
}

u32 NetManager::numAids() const {
    return currentMMInfo()->numAids;
}

bool NetManager::canSendToAid(u8 aid) const {
    if ((((1 << aid) & availableAids()) != 0) && (aid != myAid())) {
        return true;
    }
    return false;
}

bool NetManager::hasFoundMatch() const {
    bool inMatch = false;

    bool isMyAidInMatch = (1 << myAid()) & availableAids();
    // were in a match if my aid is in the room and we have connected to another
    // console
    if (isMyAidInMatch && numAids() > 1) {
        inMatch = true;
    }
    return inMatch;
}

u32 NetManager::getRacePacketSize(u8 aid) {
    u32 size = 0;
    RacePacketHolder *holder = lastSentRaceBuffer(aid);

    for (u8 i = 0; i < 8; i++) {
        size += holder->getPacketHolder(i)->packetSize();
    }
    return size;
}

void NetManager::sendRacePacket() {
    for (u8 aid = 0; aid < MAX_PLAYER_COUNT; aid++) {
        if (!canSendToAid(aid)) {
            continue;
        }

        // TODO: Bug! Different players in the room can receive different records.
        // This returns after the first successful send, resulting in many players being skipped
        // over!
        if (sendRacePacketToAid(aid)) {
            return;
        }
    }
}

bool NetManager::sendRacePacketToAid(u8 aid) {
    OSTime sentTime = 0;
    PacketHolder<void> *outgoingPacket = m_outgoingRacePacket[aid];

    if (outgoingPacket->packetSize() == 0) {
        return false;
    }
    // Patch header for mkw-server. TODO: Move to createRacePacket()
    if (!applyMKWServerHeader(outgoingPacket->packet(), myAid())) {
        return false;
    }

    // Calc the crc32
    u32 crc32 = NETCalcCRC32(outgoingPacket->packet(), outgoingPacket->packetSize());
    Header *header = reinterpret_cast<Header *>(outgoingPacket->packet());
    header->crc32 = crc32;

    // Try to send
    bool sendResult =
            trySendRacePacketToMKWServer(outgoingPacket->packet(), outgoingPacket->packetSize());

    // update time-based send members
    if (sendResult) {
        OSTime lastSentTime = m_timeOfLastSentRace[aid];
        if (lastSentTime != 0) {
            OSTime delta = sentTime - lastSentTime;
            m_timeBetweenSendingPackets[aid] = delta;
        }

        m_aidLastSentTo = aid;
        m_timeOfLastSentRace[aid] = sentTime;
    }

    // always reset the outgoing buffer
    outgoingPacket->reset();
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
        for (u32 i = 0; i < std::size(header->packetSizes); i++) {
            if (header->packetSizes[i] != 0) {
                // reset and copy the recieved packet into recv structs
                m_recvRacePackets[m_lastRecvIdx[aid][i] ^ 1][aid]->getPacketHolder(i)->reset();
                m_recvRacePackets[m_lastRecvIdx[aid][i] ^ 1][aid]->getPacketHolder(i)->copy(
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
