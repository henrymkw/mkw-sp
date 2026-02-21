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
| Public Room | Room that anyone can join |
| Private Room | Room that only players that have the host added can join |
| Host | Player that starts a room. Only applicable to private rooms. They decide the room settings |
| Guest | A player that joins a room |
| DWC | Netcode library. The base game calls these functions for most things netcode, including match making. A number of DWC functions pertaining to match making will be replaced. |
| NatNeg | Process in the base game that sets up communication between clients. Since MKW-Server is client-server, this will be removed |
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

Other ideas to consider:

- Public room rating filtering: Filtering players by skill is a nice to have, but isn't necessary, especially in the early stages of this project.
- Custom regions: This will be nice to have for stuff like custom track support.
- Joining mid-race in private rooms: Would be convenient if someone in a match disconnects. But could also lead to unwanted players from joining, without a good way to kick them out.

### Client Existing Code

Any DWC match making functions will have their implementation replaced. However the way these functions are called will not change.

`Net::NetManager::mainNetworkLoop()` calls a number of these functions. Not all are relevant to match making.

`Net::NetManager::updateMatchMakingInfosAndRatings()` also calls many of these functions.

List of potentially relevant DWC functions to replace. Not complete:

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

### Server Existing Code

The server's serverBrowser will be repurposed to handle all match making, not just public.

The term `group` will be renamed to `room` to better reflect MKW. `wfc-server` is generic by nature because it supports multiple games, but since MKW-Server is specific to MKW, and the term room is generally more understandable to the wider MKW community, renaming to `room` will be done to reflect that.

On a similar note, the term `serverBrowser` will be renamed to `roomManager`, as that is more fitting for its repurpose.

Most of the room creating logic will be removed.

### Client New Code

**Communication**: The communication for match making will be done over TCP, from client to server.

**Handlers**: A new source file, called `MatchMaking.h/c`, will be made to handle sending a receiving match making packets. This will be in C since DWC functions (written in C will call them). We will define packet structure here too.

### Server New Code

The server will have most of its server browser logic replaced.

### Potential Issues

Undefined defined number base game functions needed to rewire. This could blow up development and time. Good thing that the number of functions is finite.

## Testing

This will be tested with Dolphin and official hardware.
