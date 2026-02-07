#include "RaceConfig.hh"

namespace System {

RaceConfig::Scenario &RaceConfig::raceScenario() {
    return m_raceScenario;
}

RaceConfig *RaceConfig::CreateInstance() {
    s_instance = REPLACED(CreateInstance)();
    return s_instance;
}

RaceConfig *RaceConfig::Instance() {
    return s_instance;
}

} // namespace System
