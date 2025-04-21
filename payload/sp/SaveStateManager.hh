#pragma once

#include "game/item/KartItem.hh"
#include "game/kart/KartSaveState.hh"

#include <optional>

namespace SP {

class SaveStateManager {
public:
    void save();
    void reload();
    void processInput(bool isLoadButtonPressed, bool isSaveButtonPressed);

    static void CreateInstance();
    static void DestroyInstance();
    static SaveStateManager *Instance() {
        return s_instance;
    };

private:
    /**
     * @brief Getter for some of the kart's state.
     * 
     * @return std::tuple<Kart::KartAccessor, Kart::VehiclePhysics *, Item::KartItem *> 
     */
    static auto GetKartState();

    u8 m_framesHeld = 0;

    std::optional<Kart::KartSaveState> m_kartSaveState = std::nullopt;

    static SaveStateManager *s_instance;
};

} // namespace SP
