#pragma once

#include <Common.hh>

namespace Net {

enum class FriendStatus : u32 {
    Idle = 0x0, // Is this any different than ONLINE?
    Online = 0x2,
    OpenRoom = 0x3, // or hosting
    PlayingWithFriends = 0x4,
    PlayingPublicVS = 0x5,
    PlayingPublicBattle = 0x8,
    HostingVS = 0xB,
    HostingBattle = 0xC,
    GuestInVSFriendRoom = 0xD,
    GuestInBattleFriendRoom = 0xE,
};

// The enums here are a little confusing and deserve some explanation
// Whereas FriendStatus only makes a destinction between public and private
// rooms, FriendJoinableStatus goes a bit further in whether you can join the
// public room or not.
enum class FriendJoinableStatus : u32 {
    None = 0x0,
    Offline = 0x1,
    Online = 0x2,
    OpenRoom = 0x3,
    PlayingWithFriend = 0x4,
    WorldWideVersus = 0x5,
    JoinableRegionalVS = 0x6,
    UnjoinableRegionalVS = 0x7,
    WorldWideBattle = 0x8,
    JoinableRegionalBattle = 0x9,
    UnjoinableRegionalBattle = 0xA,
    HostingGrandPrix = 0xB,
    HostingBattle = 0xC,
    PlayingWithFriendsRaceCount = 0xD,
    PlayingWithFriendBattleCount = 0xE,
};

struct StatusData {
    u32 roomId; // note to self, this is the one used 80659680, not to be confused
                // with a similar field in matchMakingInfo
    s8 regionId;
    u8 status; // FriendStatus
    u8 playerCount;
    u8 currRace;
};
static_assert(sizeof(StatusData) == 0x8);

struct FriendInfo {
    StatusData statusData;
    u8 dwcFriendStatus; // Updated in NetManager::updateStatusDatas
    bool addedBack;
    u8 _a[0xc - 0xa];
};
static_assert(sizeof(FriendInfo) == 0xc);

} // namespace Net
