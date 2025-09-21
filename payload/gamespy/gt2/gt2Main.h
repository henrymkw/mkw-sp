/*  Copyright (c) 2011, IGN Entertainment, Inc. ("IGN")
    All rights reserved.
*/

#pragma once

#include <Common.h>

#include <gamespy/darray.h>
#include <gamespy/hashtable.h>

#include <revolution/so/so.h>

#include "gt2.h"

#define GTI2_RESPONSE_LEN 32

typedef enum {
    // client-only states
    GTI2AwaitingServerChallenge, // sent challenge, waiting for server's challenge
    GTI2AwaitingAcceptance,      // sent response, waiting for accept/reject from
                                 // server

    // server-only states
    GTI2AwaitingClientChallenge, // receiving challenge from a new client
    GTI2AwaitingClientResponse,  // sent challenge, waiting for client's response
    GTI2AwaitingAcceptReject,    // got client's response, waiting for app to
                                 // accept/reject

    // post-negotiation states
    GTI2Connected, // connected
    GTI2Closing,   // sent a close message (GTI2Close or GTI2Reject), waiting for
                   // confirmation
    GTI2Closed     // connection has been closed, free it as soon as possible
} GTI2ConnectionState;

// message types
typedef enum {
    // reliable messages
    // all start with <magic-string> <type> <serial-number>
    // <expected-serial-number> type is 1 bytes, SN and ESN are 2 bytes each
    GTI2MsgAppReliable,     // reliable application message
    GTI2MsgClientChallenge, // client's challenge to the server (initial
                            // connection request)
    GTI2MsgServerChallenge, // server's response to the client's challenge, and
                            // his challenge to the client
    GTI2MsgClientResponse,  // client's response to the server's challenge
    GTI2MsgAccept,          // server accepting client's connection attempt
    GTI2MsgReject,          // server rejecting client's connection attempt
    GTI2MsgClose,           // message indicating the connection is closing
    GTI2MsgKeepAlive,       // keep-alive used to help detect dropped connections

    GTI2NumReliableMessages,

    // unreliable messages
    GTI2MsgAck = 100, // acknowledge receipt of reliable message(s)
    GTI2MsgNack,      // alert sender to missing reliable message(s)
    GTI2MsgPing,      // used to determine latency
    GTI2MsgPong,      // a reply to a ping
    GTI2MsgClosed     // confirmation of connection closure (GTI2MsgClose or
                      // GTI2MsgReject) - also sent in response to bad messages from
                      // unknown addresses

    // unreliable messages don't really have a message type, just the magic string
    // repeated at the start
} GTI2MessageType;

typedef enum {
    GTI2UdpProtocol,      // UDP socket type for standard sockets
    GTI2VdpProtocol = 2,  // VDP socket type only used for Xbox VDP sockets
    GTI2AdHocProtocol = 3 // socket type only used for PSP Adhoc sockets
} GTI2ProtocolType;

typedef struct GTI2Buffer {
    GT2Byte *buffer; // The buffer's bytes.
    int size;        // Number of bytes in buffer.
    int len;         // Length of actual data in buffer.
} GTI2Buffer;

typedef struct GTI2IncomingBufferMessage {
    int start;                   // the start of the message
    int len;                     // the length of the message
    GTI2MessageType type;        // the type
    unsigned short serialNumber; // the serial number
} GTI2IncomingBufferMessage;

typedef struct GTI2OutgoingBufferMessage {
    int start;                   // the start of the message
    int len;                     // the length of the message
    unsigned short serialNumber; // the serial number
    gsi_time lastSend;           // last time this message was sent
} GTI2OutgoingBufferMessage;

typedef struct GTI2Socket {
    SOCKET socket; // the network socket used for all network communication

    unsigned int ip;     // the ip this socket is bound to
    unsigned short port; // the port this socket is bound to

    HashTable connections;    // the connections that are using this socket
    DArray closedConnections; // connections that are closed no longer get a spot
                              // in the hash table

    GT2Bool close; // if true, a close was attempted inside a callback, and it
                   // should be closed as soon as possible
    GT2Bool error; // if true, there was a socket error using this socket

    int callbackLevel; // if >0, then we're inside a callback (or recursive
                       // callbacks)
    gt2ConnectAttemptCallback connectAttemptCallback; // if set, callback used to handle incoming
                                                      // connection attempts
    gt2SocketErrorCallback socketErrorCallback;       // if set, call this in case of an error
    gt2DumpCallback sendDumpCallback;                 // if set, gets called for every datagram sent
    gt2DumpCallback receiveDumpCallback;              // if set, gets called for every datagram
                                                      // and connection reset received
    gt2UnrecognizedMessageCallback unrecognizedMessageCallback; // if set, gets called for all
                                                                // unrecognized messages

    void *data; // user data

    int outgoingBufferSize; // per-connection buffer sizes
    int incomingBufferSize;

    GTI2ProtocolType protocolType; // set to UDP or VDP protocol depending on the call to
                                   // create socket also used as an offset for VDP sockets
    int protocolOffset;
    GT2Bool broadcastEnabled; // set to true if the socket has already been
                              // broadcast enabled
} GTI2Socket;

typedef struct GTI2Connection {
    // ip and port uniquely identify this connection on this socket
    unsigned int ip;     // the ip on the other side of this connection (network byte order)
    unsigned short port; // the port on the other side of this connection (host byte order)

    GTI2Socket *socket; // the parent socket

    GTI2ConnectionState state; // connection state

    GT2Bool initiated; // if true, the local side of the connection initiated the
                       // connection (client)

    GT2Bool freeAtAcceptReject; // if true, don't free the connection until
                                // accept/reject is called

    GT2Result connectionResult; // the result of the connect attempt

    gsi_time startTime; // the time the connection was created
    gsi_time timeout;   // the timeout value passed into gt2Connect

    int callbackLevel;                // if >0, then we're inside a callback (or recursive
                                      // callbacks)
    GT2ConnectionCallbacks callbacks; // connection callbacks

    char *initialMessage;  // this is the initial message for the client
    int initialMessageLen; // the initial message length

    void *data; // user data

    GTI2Buffer incomingBuffer;     // buffer for incoming data
    GTI2Buffer outgoingBuffer;     // buffer for outgoing data
    DArray incomingBufferMessages; // identifies incoming messages stored in the
                                   // buffer
    DArray outgoingBufferMessages; // identifies outgoing messages stored in the
                                   // buffer

    unsigned short serialNumber;         // serial number of the next message to be sent out
    unsigned short expectedSerialNumber; // the next serial number we're expecting
                                         // from the remote side

    char response[GTI2_RESPONSE_LEN]; // after the challenge is sent during
                                      // negotiation, this is the response we're
                                      // expecting

    gsi_time lastSend;      // the last time something was sent on this connection
    gsi_time challengeTime; // the time the challenge was sent

    GT2Bool pendingAck; // if true, there is an ack waiting to go out, either on
                        // its own or as part of a reliable message

    gsi_time pendingAckTime; // the time at which the pending ack was first set

    DArray sendFilters;    // filters that apply to outgoing data
    DArray receiveFilters; // filters that apply to incoming data

} GTI2Connection;

// gti2NewOutgoingConnection() called when you're the host and someone is joining
// gti2NewIncomingConnection() called when you're the client and you're connecting to a host
// 0x8010a9bc
u32 gt2Connect(GT2Socket socket, GT2Connection *connection, char *remoteAddress, u8 *message,
        int len, int timeout, gt2ConnectedCallback *cb, bool blocking);