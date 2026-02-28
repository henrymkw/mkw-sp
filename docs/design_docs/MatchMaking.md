# Match Making Design Doc 

## Background

This document outlines the motivation and design for the match making rewrite.

### Introduction

The motivation is to rewrite the match making system to be more catered to MKW and MKW-Server. The base game's match making system is designed to setup players and their peer-to-peer communications. Since MKW-Server is client-server, rewriting the match making system allows us to remove the existing peer-to-peer logic. This will give us more control and flexibility than the existing match making system.

### Goals

## Definitions

| Term | Definition |
| ---- | ---------- |
| Match Making | The process of matching players into a room |
| MKW-Server | MKW-Client Server Project |
| WiiLink WFC | Also referred to as "wfc-server", or just "the server". Open source Nintendo Wi-Fi Connection replacement. Used by MKW-Server for authentication, friends, and more. |
| Player | User connected to online (client) |
| Public Room | Room that anyone can join |
| Private Room | Room that only players that have the host added can join |
| Host | Player that starts a room. Only applicable to private rooms. They decide the room settings |
| Guest | A player that joins a room |
| DWC | Netcode library used in the base game. A number of DWC functions pertaining to match making will be replaced. |
| NatNeg (NN) | Process in the base game that sets up communication between clients. Since MKW-Server is client-server, this will be removed |
| Server Browser | Process in the base game (and server) used to match players into public rooms. It will be repurposed and renamed. |
| Aid | A player's online id. In the base game, this determines your spot in the room |

## Planned Implementation

This will be developed in parts:

1. Creating a Private Room
   - Acceptance Criteria:
     - A player can create a private room
     - A friend with a open room will show up on friend rosters (doesn't need to be able to join)
2. Joining a Private Room
   - Acceptance Criteria:
     - A player can join a private room
     - 12 Players can join a room (and no more)
3. Creating a Public Room
   - Acceptance Criteria:
     - Players can search for a room, find each other, and begin the match.
     - 12 players can search and create a room
     - WorldWide and the different regional rooms are distinguished
4. Joining a Public Room
   - Acceptance Criteria:
      - A player can join and spectate a on going room.
      - Joining works from both the main online screen and the friend roster screen

Other implementation details to be considered:

- Public room rating filtering: Filtering players by skill is a nice to have, but isn't necessary, especially in the early stages of this project.
- Custom regions: This will be nice to have for stuff like custom track support.
- Joining mid-race in private rooms: Would be convenient if someone in a match disconnects. But could also lead to unwanted players from joining, without a good way to kick them out.

## New Packet Structures

To support the reimplementation, both the client and server will define new packet types.

### Match Packet

Match making information will be transmitted from MKW-Server to players as defined by the following new packet structure, a `Match` packet. It contains information regarding the room a player is in. Players will receive a `Match` packet every second over TCP and store its contents locally. Its structure is detailed bellow:

| Name | Description | Return Value of | Offset | Length |
| ---- | ----------- | --------------- | ------ | ------ |
| Aid Bitmap | Bitmap of the available aids | `DWC_GetAidBitmap()` | 0x00 | 0x4 |
| Aid Count | # of non-guest players | `DWC_GetNumConnectionHost()` | 0x04 | 0x04 |
| Player Count | # of players, including guests | `DWC_GetDirectConnectedAIDBitmap()` | 0x08 | 0x04 |
| Room ID | ID of the room | `DWC_GetGroupId()` | 0x0c | 0x4 |
| Player Aid | Aid of the receiving player | `DWC_GetMyAid()` | 0x10 | 0x1 |
| Host Aid | Aid of the room's host. Used for compatibility reasons | `DWC_GetServerAid()` | 0x11 | 0x1 |
| Match Making Suspended | The match making suspend state of the room | `DWC_GetSuspendMatch()` | 0x12 | 0x1 |
| Cancel Match | If the match is canceled | `DWC_IsValidMatchCancel()` | 0x13 | 0x1 |
| Local Player Count | Local player count for each player, indexed by aid. This is in little endian since `NetManager::updateMatchMakingInfosAndRatings()` expects it to be. | `DWC_GetConnectionUserData()` | 0x14 | 0x30 |

`Match` packet's contents will be parsed and stored into a static variables in [MatchMaking.h](../../payload/sp/net/mkw_server/MatchMaking.h). From here, the DWC functions will be replaced to return the values specified in the above table.

### Match Request Packets

For the player to open a room, join a room, search for a public room, etc., they will send requests to wfc-server. There are different types of requests players can send, which all may specify different parameters. To support this, each type of request will have their own packet type defined bellow:

`CreateRoom` packet:

Request to create a private room. This packet structure is simple.

| Id | Description | Offset | Length |
| -- | ----------- | ------ | ------ |
| CreateRoomId | Request Id for the `CreateRoom` packet. Value is always 0 | 0x0 | 0x4 |

`JoinFroom` packet:

Request to join a friend's private room.

| Id | Description | Offset | Length |
| -- | ----------- | ------ | ------ |
| JoinRoomId | Request Id for the `JoinRoom` packet. Value is always 1 | 0x0 | 0x1 |
| FriendProfileId | ProfileId of the friend to join. | 0x1 | 0x4 |

`SearchPublicRoom` packet:

Request to search for a public room.

| Id | Description | Offset | Length |
| -- | ----------- | ------ | ------ |
| SearchPublicRoomId | Request Id for the `SearchPublicRoom` packet. Value is always 2 | 0x0 | 0x1 |
| Region | Search Region. Value differs on the region to search. Value 0 is regionless (worldwide), 1 is NA, 2, is EU, 3 is JP, 4 is KOR. | 0x1 | 0x1 |

`VoteSuspendMatch` packet:

Vote to suspend match making. Must already be in a room to send, which `wfc-server` will validate.

| Id | Description | Offset | Length |
| -- | ----------- | ------ | ------ |
| VoteMatchSuspendId | Request Id for the `VoteMatchSuspend` packet. Value is always 3 | 0x0 | 0x1 |
| SuspendVote | Vote for match making suspension. 0 is a unsuspend vote, 1 is a suspend vote. | 0x1 | 0x1 |

## Client Code Changes

### Relevant Base Game Functions

Any DWC match making function will have their implementation replaced to return the specified value in the [Match Packet](#match-packet) table. If implemented correctly, the calls to these functions won't change.

List of potentially relevant DWC functions to replace, and what they do. Likely not a complete list:

- `DWC_GetAidBitmap()`: Who is in the room and what aids are taken?
- `DWC_GetNumConnectionHost()`: How many players (not including guests) are in the room?
- `DWC_GetDirectConnectedAIDBitmap()`
- `DWC_GetGroupId()`: What's the ID for my room?
- `DWC_GetMyAid()`: What's my aid? Relevant in checking for self in various areas of the netcode
- `DWC_GetServerAid()`: Aid of the host
- `DWC_GetConnectionUserData()`: Local Player Count
- `DWC_RequestMatchSuspendAsync(bool canJoin)`: Vote to open up match making
- `DWC_GetSuspendMatch()`: Can others join the room? Joining is possible when all players vote that match making is open.
- `DWC_SetupGameServer()`: Creating private room
- `DWC_ConnectToGameServerAsync()`: Joining private room
- `DWC_ConnectToGameServerFromGroupID()`: Joining public room from friend
- `DWC_ConnectToAnyoneAsync()`: Joining a public room
- `DWC_ProcessFriendsMatch()`: Main DWC Loop. Will need to be modified.
- `DWC_CloseAllConnectionsHard()`: Clean up NatNeg and other peer-to-peer stuff
- `DWC_RegisterMatchingStatus()`
- `DWC_IsValidMatchCancel()`: Check if the match has been canceled
- `DWC_CancelMatch()`: Logic to cancel match
- `DWC_RegisterMatchingStatus()`
- `DWC_GetLastErrorEx()`: Might need to modify error handling if vanilla errors popup as result of rewrite.

### Communication

The communication for match making will be done over TCP, from client to server.

### Handlers

A new source file, called `MatchMaking.h/c`, will be made to handle sending a receiving match making packets. This will be in C since DWC functions (written in C will call them).

## Server Code

### Refactors

The term `group` will be renamed to `room` to better reflect MKW. `wfc-server` is generic by nature because it supports multiple games, but since MKW-Server is specific to MKW, and the term room is generally more understandable to the wider MKW community, renaming to `room` will be done to reflect that. Likewise, `session` will be renamed to `player`. On a similar note, the term `serverBrowser` will be renamed to `roomManager`, as that is more fitting for its repurpose.

### Other Sections TODO

TODO

### Potential Issues

Undefined defined number base game functions needed to rewire. This could blow up development and time. Good thing that the number of functions is finite.

## Testing

This will be tested with Dolphin and official hardware.
