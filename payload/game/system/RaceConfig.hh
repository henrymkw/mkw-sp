#pragma once

#include <Common.hh>

#include "game/util/Registry.hh"

namespace System {

/// Configuration for the current race.
///
/// For configuration over a set of VS/BT races, see GlobalContext.
class RaceConfig {
public:
    RaceConfig();
    virtual ~RaceConfig();
    virtual void dt(s32 type);

    enum class GameMode {
        OfflineVS = 1,
        TimeAttack = 2,
        OfflineBT = 3,
        Mission = 4,
        OnlinePrivateVS = 7,
        OnlinePrivateBT = 10,
        Awards = 11,
    };

    struct Scenario {
        u8 _000[0xb50 - 0x000];
        GameMode gameMode;
        u8 _b54[0xbf0 - 0xb54];
    };
    static_assert(sizeof(Scenario) == 0xbf0);

    Scenario &raceScenario();

    static RaceConfig *Instance();

private:
    u8 _0004[0x0020 - 0x0004];

public:
    Scenario m_raceScenario;
    u8 _0c10[0x73f0 - 0x0c10];

    static RaceConfig *s_instance;
};
static_assert(sizeof(RaceConfig) == 0x73f0);

} // namespace System
