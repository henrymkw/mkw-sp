#include <Common.S>

#ifdef DEBUG

PATCH_REPLACE_START(FriendInfoPage_onRemoveFriendButtonClick, 0x0)
    // Return when in debug to prevent accidental removal of friends
    blr
PATCH_REPLACE_END(FriendInfoPage_onRemoveFriendButtonClick, 0x0)


#endif
