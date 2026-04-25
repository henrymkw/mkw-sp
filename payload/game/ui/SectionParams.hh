#pragma once

#include <Common.hh>

namespace UI {

class SectionParams {
public:
    u8 getLocalPlayerCount() const;

private:
    u8 _000[0x124 - 0x000];
    u32 m_localPlayerCount;
    u8 _128[0x510 - 0x128];
};
static_assert(sizeof(SectionParams) == 0x510);

} // namespace UI
