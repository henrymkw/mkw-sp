# MKW-Server

MKW-Server (Mario Kart Wii - Server) is an experimental rework of Mario Kart Wii's (MKW) netcode. As the name suggests, the netcode is reworked from peer-to-peer to client-server. At the time of writing, only race packets are sent to the server, so it's not entirely client-server, but this will change at some point. This document will explain its purpose and implementation.

## Purpose

The purpose of MKW-Server is to improve the base game's netcode. The following are areas that MKW-Server aims to improve:

### Latency Improvements

As mentioned, MKW's vanilla netcode is peer-to-peer during races. The netcode works relatively well, but there are issues. Players (clients) send a race packet over UDP to one other player at a time. As a result, latency scales with player count ([source](https://wiki.tockdom.com/wiki/Network_Protocol#Point_to_Point_Communication)). MKW-Server fixes this in theory as players send race packets to a single server rather than a single player at a time. MKW-Server has significantly improved hardware over a Wii, and is able to perform strenuous operations that a Wii can't. As a result of a centralized structure and improved hardware, in-game lag should decrease in theory.

### Security

As of time of writing (initial release), MKW-Server's security is not good. But it has potential to be an improvement as a result of the server architecture. Here are some of the ways where MKW-Server has potential:

- Packet Verification: The server is much faster compared to a Wii. This means packets can be parsed and verified. The exact verifications aren't implemented and undefined at time of writing.
- Item Decision: To decide items in MKW, the base game refers to an item probability table that is stored on disk (`ItemSlot.bin`). This alone is a severe anti-cheat problem, and has been a major point of pain throughout competitive MKW's history. Clients cannot be trusted to not tamper with it, so the plan is to transfer the responsibility of deciding items to MKW-Server. This would require through testing and reverse-engineering to prevent behavior deriving from the base game.

## Implementation

### Client

As of time of writing, the client's patches are a hacky mess and needs to be improved. The patches are implemented using MKW-SP, though other patchers, including the WFC Payload, can be used in the future. Here are the current patches at time of writing:

- `NetManager::sendRACEPacket()`: Change the first four bytes of the race packet's magic to `0xB00{myAid}`. This function is chosen because the CRC32 check is calculated and stored in the packet here. Adding myAid is needed currently (though is a hack) since the receiver needs to know who sent the packet. Vanilla figures this out from the senders address, but MKW-Server fundamentally breaks this.
- `SOSendTo`: Detect race packets, and change the outgoing address to the MKW-Server. This again is hacky, but a proper implementation requires degutting large chunks of DWC and GT2. This keeps the DWC and GT2 structures intact by swapping out the address at a location of low abstraction.
- `DWCi_GT2UnrecognizedMessageCallback()`: Used to call `DWCTransport->userRecvCallback()`, which contains `NetManager::processRACEPacket()`. `NetManager::processRACEPacket()` takes in an aid, which is why the patch on the 4th byte is needed (at the time of writing).
- `DWCi_TransportProcess`: Nop branches to `DWC_CloseConnectionHard()`. This patch is unideal and results from the lack of understanding of DWC and the GameSpy SDK. The game timeouts as a result of not hearing back from other clients, and calls `DWC_CloseConnectionHard()`. Since MKW-Server is client-server, this timeout is irrelevant. This patch should be removed once a proper fix is understood.

### Server

TODO:

## Notes

It should be stressed that MKW-Server is *very* experimental. There are a lot of pros, but these are all in theory. How it will end up working in practice is completely unknown. It could completely flop, but the potential upsides make it worth trying out.
