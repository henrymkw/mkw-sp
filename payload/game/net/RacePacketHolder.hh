#pragma once

#include <Common.hh>

#include "game/net/RecordHolder.hh"
#include "game/net/records/Event.hh"
#include "game/net/records/Header.hh"
#include "game/net/records/Item.hh"
#include "game/net/records/RH1.hh"
#include "game/net/records/RH2.hh"
#include "game/net/records/RaceData.hh"
#include "game/net/records/User.hh"

namespace Net {

enum class RecordType : u8 {
    Header = 0,
    RaceHeader1 = 1,
    RaceHeader2 = 2,
    RoomSelect = 3, // Room is used when the friend room is open, select otherwise
    RaceData = 4,
    User = 5,
    Item = 6,
    Event = 7,
};

class RacePacketHolder {
public:
    template <typename T>
    RecordHolder<T> *holder(RecordType idx) {
        switch (idx) {
        case RecordType::Header:
            return reinterpret_cast<RecordHolder<T> *>(m_header);
        case RecordType::RaceHeader1:
            return reinterpret_cast<RecordHolder<T> *>(m_raceHeader1);
        case RecordType::RaceHeader2:
            return reinterpret_cast<RecordHolder<T> *>(m_raceHeader2);
        case RecordType::RoomSelect:
            return reinterpret_cast<RecordHolder<T> *>(m_roomSelect);
        case RecordType::RaceData:
            return reinterpret_cast<RecordHolder<T> *>(m_raceData);
        case RecordType::User:
            return reinterpret_cast<RecordHolder<T> *>(m_user);
        case RecordType::Item:
            return reinterpret_cast<RecordHolder<T> *>(m_item);
        case RecordType::Event:
            return reinterpret_cast<RecordHolder<T> *>(m_event);
        default:
            return nullptr;
        }
    }

    RecordHolder<Header> *header() {
        return m_header;
    }

    RecordHolder<RH1Record> *rh1() {
        return m_raceHeader1;
    }

    RecordHolder<RH2Record> *rh2() {
        return m_raceHeader2;
    }

    RecordHolder<void> *roomSelect() {
        return m_roomSelect;
    }

    RecordHolder<RaceDataRecord> *raceData() {
        return m_raceData;
    }

    RecordHolder<UserRecord> *user() {
        return m_user;
    }

    RecordHolder<ItemRecord> *item() {
        return m_item;
    }

    RecordHolder<EventRecord> *event() {
        return m_event;
    }

    template <typename T>
    RecordHolder<T> *holder(u8 idx) {
        return holder<T>(static_cast<RecordType>(idx));
    }

    RecordHolder<void> *holder(u8 idx) {
        return holder<void>(static_cast<RecordType>(idx));
    }

    u32 size();

    void reset();

private:
    RecordHolder<Header> *m_header;
    RecordHolder<RH1Record> *m_raceHeader1;
    RecordHolder<RH2Record> *m_raceHeader2;
    // TODO: Room is present while in a room that hasn't started and select is for voting
    // so whats the best way to represent this?
    RecordHolder<void> *m_roomSelect;
    RecordHolder<RaceDataRecord> *m_raceData;
    RecordHolder<UserRecord> *m_user;
    RecordHolder<ItemRecord> *m_item;
    RecordHolder<EventRecord> *m_event;
};
static_assert(sizeof(RacePacketHolder) == 0x20);

} // namespace Net
