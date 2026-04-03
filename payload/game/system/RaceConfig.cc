#include "RaceConfig.hh"

namespace System {

RaceConfig::Scenario &RaceConfig::raceScenario() {
    return m_raceScenario;
}

RaceConfig *RaceConfig::Instance() {
    return s_instance;
}

} // namespace System
