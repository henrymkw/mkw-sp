#pragma once

#include <Common.hh>

namespace System {

class NetManager {

public:
    NetManager() = default;
    ~NetManager() = default;

private:
    REPLACE void Connect();
    void REPLACED(Connect)();
    // unknown data members
};

} // namespace System