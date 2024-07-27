#pragma once

#include <Common.hh>

namespace System {

/**
 * @brief TODO: Is this just RKNetController??
 *
 */
class NetManager {
public:
    // TODO: ctors and dtors

private:
    /**
     * @brief Original implementation should be documented in more detail. It inits SO, also checks
     * if the user has connected for the first time before connecting to a server. This is hooked to
     * set FriendManager's user data before connecting. Without this, the game will always assign a
     * new profile when connecting to WWFC for the first time on a playthrough. Something in SP
     * relating to licenses is causing a need for this.
     *
     * @return REPLACE
     */
    REPLACE void Connect();
    void REPLACED(Connect)(); // 0x80658B9C
    // unknown data members
};

} // namespace System