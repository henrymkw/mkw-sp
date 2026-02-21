# Countdown Design Document

## Background

### Introduction

MKW-Server has an issue where players often don't start simultaneously, causing a lag-start. This gives certain players a competitive advantage over others so it's a high priority fix.

### Goals

The goal is for all player's countdown to begin roughly at the same time.

## Definitions

| Term | Definition |
| ---- | ---------- |
| MKW-Server | Mario Kart Wii-Server, an experimental client-server implementation of Mario Kart Wii's netcode |
| Intro-Camera | The camera rotation upon entering a race, preceding the countdown |
| Countdown | The countdown from three to start the race |
| Lag-start | When clients start at different times than each other |

## Planned Implementation

The plan is to reimplement the countdown logic in a client-server manner.

### Existing Game Code

The function that will be modified is `MiscPacketHandler::isEveryoneInRace()`. It returns `true` when all players have loaded into the race, and thus can start the countdown. For unclear reasons, MKW-Server breaks this function, and players lag-start often.

### New Code

Both the client and server will use new, custom, packets.

#### `Ready` Packet

| Offset | Name | Explanation | Length |
| ------ | ---- | ----------- | ------ |
| 0x0 | Magic | `READ`, the magic for a `Ready` packet | 4 |

#### `Start` Packet

| Offset | Name | Explanation | Length |
| ------ | ---- | ----------- | ------ |
| 0x0 | Magic | `STAR`, the magic for this packet | 4 |

#### Client

A client will send a `Ready` packet when they are ready to begin the countdown. A client is ready to begin the countdown when their intro camera is finished rotating. We can use the same logic as in the base game, by checking if three seconds has passed after `RaceConfig::RaceScenario.m_Settings.m_CameraMode == GameplayNoIntro` returns true to determine if ready. This isn't perfect, as lag-starts exist in the base-game with this implementation. Adjustments can be made as needed.

After sending the `Ready` packet, clients listen for the server in `GT2UnrecognizedMessageCallback()` for a `Start` packet. If the client receives a `Start` packet before it sends out a `Ready` packet, then this is an error.

#### Server

To keep track of the clients that have sent a `Ready` packet, a bool member (named `isReady`) will be added to the `Player` type. The server will listen for client's `Ready` packet in `Room.readLoop()`, verifying the client is in the room, and set `isReady` when received. When `isReady` is true for all players, the server will broadcast a `Start` packet to all players, and reset all `isReady` to false.

### Potential Issues

- Synchronization and latency may still be problem points and might not be simple to fix.
  - Client's whose game is running too slow or too fast can cause synchronization issues, and should be kicked. This logic isn't implemented at time of writing, unfortunately.
  - The time it takes to receive a `Start` packet can vary from client to client due to latency, resulting in lag-starts. This could be a difficult issue to fix. The infrastructure to synchronize time between clients and servers isn't implemented at time of writing.
- This should be reliable, but the infrastructure for reliable messages also isn't implemented yet.

## Testing

Tests will be performed on both official hardware and Dolphin (preferably from different regions). The percentage of 'noticeable' lag-starts will be the main measurement success, lower being desirable.
