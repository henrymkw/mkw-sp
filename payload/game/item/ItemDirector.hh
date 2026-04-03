#pragma once

#include <Common.hh>

#include "game/kart/KartItem.hh"

namespace Item {

class ItemDirector {
public:
    void calc();

    void processInput(bool isPressed);

    Kart::KartItem *getKartItem() {
        return m_players;
    }

    static ItemDirector *Instance() {
        return s_instance;
    }

private:
    u8 _000[0x10 - 0x000];
    u8 m_playerCount;
    u8 _011[0x014 - 0x011];
    Kart::KartItem *m_players;
    Kart::KartItem *m_otherLocalOnlinePlayers[12];
    u8 _048[0x430 - 0x048];

    static ItemDirector *s_instance;
};
static_assert(sizeof(ItemDirector) == 0x430);

} // namespace Item
