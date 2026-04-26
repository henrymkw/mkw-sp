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
| Magic | Magic of this packet. Value is always 0x77846772 ("MTCH") | N/A | 0x00 | 0x4 |
| Aid Bitmap | Bitmap of the available aids | `DWC_GetAidBitmap()` | 0x04 | 0x4 |
| Num Aids | # of non-guest players | `DWC_GetNumConnectionHost()` | 0x08 | 0x04 |
| Direct Connected Aid Bitmap | # of players, including guests | `DWC_GetDirectConnectedAIDBitmap()` | 0x0c | 0x04 |
| Room Id | Id of the room | `DWC_GetGroupId()` | 0x10 | 0x4 |
| My Aid | Aid of the receiving player | `DWC_GetMyAid()` | 0x14 | 0x1 |
| Host Aid | Aid of the room's host | `DWC_GetServerAid()` | 0x15 | 0x1 |
| Suspension State | The match making suspend state of the room | `DWC_GetSuspendMatch()` | 0x16 | 0x1 |
| Match Canceled | If the match is canceled | `DWC_IsValidMatchCancel()` | 0x17 | 0x1 |
| Local Player Counts | Local player count for each player, indexed by aid. This is in little endian since `NetManager::updateMatchMakingInfosAndRatings()` expects it to be. | `DWC_GetConnectionUserData()` | 0x18 | 0x30 |

`Match` packet's contents will be parsed and stored into a static variables in [RoomManager.h](../../payload/sp/net/mkw_server/RoomManager.h). From here, the DWC functions will be replaced to return the values specified in the above table.

### Match Request Packets

For the player to open a room, join a room, search for a public room, etc., they will send requests to wfc-server. These sort of packets can generally be called `Match Request` packets. There are different types of requests players can send, which all may specify different parameters. To support this, each type of request will have their own packet type defined bellow:

### `MatchRequestHeader`

| Name | Description | Offset | Length |
| ---- | ----------- | ------ | ------ |
| Magic | `MatchRequestHeader` magic, always `MREQ`. | 0x0 | 0x4 |
| `MatchRequestType` | Type of match request packet | 0x4 | 0x1 |
| Padding | Padding | 0x5 | 0x3 |
| SearchId | Client SearchId | 0x8 | 0x8 |

### `OpenRoom` packet

Request to create a private room. This packet structure is simple.

| Name | Description | Offset | Length |
| ---- | ----------- | ------ | ------ |
| `MatchRequestHeader` | Header, `MatchRequestType` must be 0 | 0x00 | 0x10 |

### `JoinFriend` packet

Request to join a friend's private room.

| Name | Description | Offset | Length |
| ---- | ----------- | ------ | ------ |
| `MatchRequestHeader` | Header, `MatchRequestType` must be 1 | 0x00 | 0x10 |
| FriendProfileId | ProfileId of the friend to join. | 0x10 | 0x4 |
| SearchRegion | Search Region. Valid values are 0 for private room, 1 for worldwide, 2 for NA, 3 for EU, 4 for JP, 5 for KOR. | 0x10 | 0x1 |
| Padding | Padding | 0x15 | 0x3 |

### `LeaveRoom` packet

Request to leave private room.

| Name | Description | Offset | Length |
| ---- | ----------- | ------ | ------ |
| `MatchRequestHeader` | Header, `MatchRequestType` must be 2 | 0x00 | 0x10 |

### `SuspendRequest` packet

Vote to suspend match making. Must already be in a room to send, which `wfc-server` will validate.

| Id | Description | Offset | Length |
| -- | ----------- | ------ | ------ |
| `MatchRequestHeader` | Header, `MatchRequestType` must be 3 | 0x00 | 0x10 |
| SuspendVote | Vote for match making suspension. 0 is a unsuspend vote, 1 is a suspend vote. | 0x10 | 0x1 |
| Padding | Padding | 0x11 | 0x07 |

### `SearchRoom` packet

Request to search for a public room.

| Name | Description | Offset | Length |
| ---- | ----------- | ------ | ------ |
| `MatchRequestHeader` | Header, `MatchRequestType` must be 4 | 0x00 | 0x10 |
| Region | Search Region. Valid values are 1 for worldwide, 2 for NA, 3 for EU, 4 for JP, 5 for KOR. | 0x10 | 0x1 |
| GameMode | Game mode to search for. 1 is VS, 2 is Battle | 0x11 | 0x1 |
| Padding | Padding | 0x12 | 0x06 |

### `LocalPlayerCount` packet

Informing the server our local player count. This is sent soon after a session is established and doesn't change

| Name | Description | Offset | Length |
| ---- | ----------- | ------ | ------ |
| `MatchRequestHeader` | Header, `MatchRequestType` must be 4 | 0x00 | 0x10 |
| Local Player Count | Number of local players. Valid values are 1 or 2. | 0x10 | 0x1 |
| Padding | Padding | 0x11 | 0x7 |

### `SearchId` Packet

The `SearchId` packet only gets sent from `wfc-server` to other players. It is sent right after the server establishes a connection with the player over UDP (reliable). The searchId, is used as a unique identifier for each player. This is included in the header so `wfc-server` can associate messages over TCP to the underlying player type.

| Name | Description | Offset | Length |
| Magic | "SEARCHID" | 0x0 | 0x8 |
| SearchId | The player's searchId | 0x8 | 0x8 |

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

### Communication

The communication for match making will be done over TCP, from client to server.

### Handlers

A new source file, called `RoomManager.h/c`, will be made to handle sending a receiving match making packets. This will be in C since DWC functions (written in C will call them).

## Server Code

### Refactors

The term `group` will be renamed to `room` to better reflect MKW. `wfc-server` is generic by nature because it supports multiple games, but since MKW-Server is specific to MKW, and the term room is generally more understandable to the wider MKW community, renaming to `room` will be done to reflect that. Likewise, `session` will be renamed to `player`.

### Other Sections TODO

TODO

### Potential Issues

Undefined defined number base game functions needed to rewire. This could blow up development and time. Good thing that the number of functions is finite.

## Testing

This will be tested with Dolphin and official hardware.
