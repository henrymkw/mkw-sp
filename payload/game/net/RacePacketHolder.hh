#pragma once

#include <Common.hh>

#include "game/net/PacketHolder.hh"
#include "game/net/records/Event.hh"
#include "game/net/records/Header.hh"
#include "game/net/records/Item.hh"
#include "game/net/records/RH1.hh"
#include "game/net/records/RH2.hh"
#include "game/net/records/RaceData.hh"
#include "game/net/records/User.hh"

namespace Net {

enum class RACEPacketRecordIdx : u8 {
    Header = 0,
    RACEHeader1 = 1,
    RACEHeader2 = 2,
    RoomSelect = 3, // Room is used when the friend room is open, select otherwise
    RaceData = 4,
    User = 5,
    Item = 6,
    Event = 7,
};

class RacePacketHolder {
public:
    template <typename T>
    PacketHolder<T> *getPacketHolder(RACEPacketRecordIdx idx) {
        switch (idx) {
        case RACEPacketRecordIdx::Header:
            return reinterpret_cast<PacketHolder<T> *>(m_header);
        case RACEPacketRecordIdx::RACEHeader1:
            return reinterpret_cast<PacketHolder<T> *>(m_raceHeader1);
        case RACEPacketRecordIdx::RACEHeader2:
            return reinterpret_cast<PacketHolder<T> *>(m_raceHeader2);
        case RACEPacketRecordIdx::RoomSelect:
            return reinterpret_cast<PacketHolder<T> *>(m_roomSelect);
        case RACEPacketRecordIdx::RaceData:
            return reinterpret_cast<PacketHolder<T> *>(m_raceData);
        case RACEPacketRecordIdx::User:
            return reinterpret_cast<PacketHolder<T> *>(m_user);
        case RACEPacketRecordIdx::Item:
            return reinterpret_cast<PacketHolder<T> *>(m_item);
        case RACEPacketRecordIdx::Event:
            return reinterpret_cast<PacketHolder<T> *>(m_event);
        default:
            return nullptr;
        }
    }

    PacketHolder<Header> *header() {
        return m_header;
    }

    PacketHolder<RH1Record> *rh1() {
        return m_raceHeader1;
    }

    PacketHolder<RH2Record> *rh2() {
        return m_raceHeader2;
    }

    PacketHolder<void> *roomSelect() {
        return m_roomSelect;
    }

    PacketHolder<RaceDataRecord> *raceData() {
        return m_raceData;
    }

    PacketHolder<UserRecord> *user() {
        return m_user;
    }

    PacketHolder<ItemRecord> *item() {
        return m_item;
    }

    PacketHolder<EventRecord> *event() {
        return m_event;
    }

    template <typename T>
    PacketHolder<T> *getPacketHolder(u8 idx) {
        return getPacketHolder<T>(static_cast<RACEPacketRecordIdx>(idx));
    }

    PacketHolder<void> *getPacketHolder(u8 idx) {
        return getPacketHolder<void>(static_cast<RACEPacketRecordIdx>(idx));
    }

    u32 getRecordSize(u8 idx) {
        return getPacketHolder<void>(idx)->packetSize();
    }

    void reset();

    void applyMKWServerHeader(u8 aid);

private:
    PacketHolder<Header> *m_header;
    PacketHolder<RH1Record> *m_raceHeader1;
    PacketHolder<RH2Record> *m_raceHeader2;
    // TODO: Room is present while in a room that hasn't started and select is for voting
    // so whats the best way to represent this?
    PacketHolder<void> *m_roomSelect;
    PacketHolder<RaceDataRecord> *m_raceData;
    PacketHolder<UserRecord> *m_user;
    PacketHolder<ItemRecord> *m_item;
    PacketHolder<EventRecord> *m_event;
};
static_assert(sizeof(RacePacketHolder) == 0x20);

} // namespace Net