# DWC Notes

DWC is the Nintendo Wi-Fi (WFC) Connection networking library. It's a wrapper for the GameSpy SDK, written in C. There are a number of different modules.

## [Main](../payload/revolution/dwc/DWCMain.h)

The Main module is the primary controller for WFC behavior, state management, and other modules.

## [Transport](../payload/revolution/dwc/DWCTransport.h)

The Transport module is a wrapper for GT2 (GameSpy Transport SDK 2). It handles peer-to-peer communication over UDP, both reliable and unreliable messages.

## [Match](../payload/revolution/dwc/DWCMatch.h)

The Match module is used for Matchmaking. This includes public searches (worldwide/regional) and private (frooms) matchmaking. It ensures clients and servers have a synchronized state. It mainly wraps around QR2 (Query and Reporting SDK 2) but also utilizes Natneg and GT2.

## [Friend](../payload/revolution/dwc/DWCFriend.h)

The Friend module is used to add/remove friends, get friend statuses, and set own status. It is a wrapper for the GP (GameSpy Presence and Messaging SDK).

## [Login](../payload/revolution/dwc/DWCLogin.h)

Used when logging on to WFC. It handles first time/existing users by assigning/authenticating user ids.

## [Error](../payload/revolution/dwc/DWCError.h)

The Error module centralizes error state and error codes.

## Misc

Functions are prepended with either `DWC_` or `DWCi_`. The `i` in `DWCi_` stands for 'internal'.

`DWCContext->state` keeps track of an client's *online* state. While there are 7 different states in total, the ones that matter for the purposes of MKW-Server are `DWC_STATE_MATCH_MAKING` and `DWC_STATE_CONNECTED_TO_PEERS`.

`DWC_STATE_MATCH_MAKING` is when a player is looking for other players but isn't connected to any. This includes searching for players in a WorldWide, but haven't found anyone yet, or hosting a private room that no one has joined yet.
