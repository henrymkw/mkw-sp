/*  Copyright (c) 2011, IGN Entertainment, Inc. ("IGN")
    All rights reserved.
*/

#pragma once

#include <Common.h>

#include <gamespy/darray.h>
#include <gamespy/hashtable.h>

#include <revolution/so/so.h>

typedef int GT2Bool;
#define GT2False 0
#define GT2True 1

typedef unsigned char GT2Byte;
typedef unsigned int gsi_time; // must be 32 bits

// a handle to a socket object (can be used to accept connections and initiate
// connections)
typedef struct GTI2Socket *GT2Socket;

// a handle to an object representing a connection to a specific IP and port
// the local endpoint is a GT2Socket
typedef struct GTI2Connection *GT2Connection;

// the result of a GT2 operation
// check individual function definitions to see possible results
// TODO: list possible results wherever this is used
typedef enum {
    GT2Success,           // success
                          // errors:
    GT2OutOfMemory,       // ran out of memory
    GT2Rejected,          // attempt rejected
    GT2NetworkError,      // networking error (could be local or remote)
    GT2AddressError,      // invalid or unreachable address
    GT2DuplicateAddress,  // a connection was attempted to an address that already
                          // has a connection on the socket
    GT2TimedOut,          // time out reached
    GT2NegotiationError,  // there was an error negotiating with the remote side
    GT2InvalidConnection, // the connection didn't exist
    GT2InvalidMessage,    // used for vdp reliable messages containing voice data, no
                          // voice data in reliable messages
    GT2SendFailed         // the send failed,
} GT2Result;

// possible states for any GT2Connection
typedef enum {
    GT2Connecting, // negotiating the connection
    GT2Connected,  // the connection is active
    GT2Closing,    // the connection is being closed
    GT2Closed      // the connection has been closed and can no longer be used
} GT2ConnectionState;

// The cause of the connection being closed.
typedef enum {
    GT2LocalClose,         // The connection was closed with gt2CloseConnection.
    GT2RemoteClose,        // The connection was closed remotely.
                           // errors:
    GT2CommunicationError, // An invalid message was received (it was either
                           // unexpected or wrongly formatted).
    GT2SocketError,        // An error with the socket forced the connection to close.
    GT2NotEnoughMemory     // There wasn't enough memory to store an incoming or
                           // outgoing message.
} GT2CloseReason;

// this callback gets called when there was is an error that forces a socket to
// close all connections that use this socket are terminated, and their
// gt2CloseCallback callbacks will be called before this callback is called
// (with the reason set to GT2SocketError). the socket cannot be used again
// after this callback returns
typedef void (*gt2SocketErrorCallback)(GT2Socket socket);

// Called when the connect has completed.
// If the result is GT2Rejected,
// then message is the message that the
// listener passed to gt2Reject.  If the
// result is anything else, then message
// is NULL and len is 0.
typedef void (*gt2ConnectedCallback)(GT2Connection connection, // The connection object.
        GT2Result result,                                      // Result from connect attempt.
        GT2Byte *message, // If result==GT2Rejected, the reason.  Otherwise, NULL.
        int len           // If result==GT2Rejected, the length of the reason.  Otherwise, 0.
);

// Called when a message is received.
typedef void (*gt2ReceivedCallback)(
        GT2Connection connection, // The connection the message was received on.
        GT2Byte *message,         // The message that was received.  Will be NULL if an
                                  // empty message.
        int len,                  // The length of the message in bytes.  Will be 0 if an empty
                                  // message.
        GT2Bool reliable          // True if this is was sent reliably.
);

// Called when the connection is closed (remotely or locally).
// The connection can no longer be used after this callback returns.
typedef void (*gt2ClosedCallback)(GT2Connection connection, // The connection that was closed.
        GT2CloseReason reason                               // The reason the connection was closed.
);

// When a reply is received for a ping that was sent, this callback is called.
// The latency reported here is the amount of time between when the ping
// was first sent with gt2Ping and when the pong was received.
typedef void (*gt2PingCallback)(GT2Connection connection, // the connection the ping was sent on
        int latency // the round-trip time for the ping, in milliseconds
);

// Callbacks set for each connection.
// The connected callback is ignored
// when this is passed to gt2Accept.
typedef struct {
    gt2ConnectedCallback connected; // Called when gt2Connect is complete.
    gt2ReceivedCallback received;   // Called when a message is received.
    gt2ClosedCallback closed;       // Called when the connection is closed (remotely or locally).
    gt2PingCallback ping;           // Called when a ping reply is received.
} GT2ConnectionCallbacks;

// callback gets called when someone attempts to connect to a socket that is
// listening for new connections. in response to this callback the application
// should call gt2Accept or gt2Reject.  they do not need to be called from
// inside the callback, however they should be called in a timely manner so that
// the remote side does not need to sit around indefinitely waiting for a
// response. the latency is an estimate of the round trip time between
// connections.
typedef void (*gt2ConnectAttemptCallback)(GT2Socket socket, // the socket the attempt came in on
        GT2Connection connection, // a connection object for the incoming connection attempt
        unsigned int ip,          // the IP being used remotely for the connection attempt
        unsigned short port,      // the port being used remotely for the connection attempt
        int latency,              // the approximate latency on the connection
        GT2Byte *message,         // an optional message sent with the attempt.  Will be
                                  // NULL if an empty message.
        int len // the length of the message, in characters.  Will be 0 if an empty
                // message.
);

// Callback for filtering outgoing data.
// Call gt2FilteredSend with the filtered data, either from within the callback
// or later. the message points to the same memory location as the message
// passed to gt2Send (or gt2FilteredSend). so if the call to gt2FilteredSend is
// delayed, it is the filter's responsibility to make sure the data is still
// around when and if it is needed.
typedef void (*gt2SendFilterCallback)(
        GT2Connection connection, // The connection on which the message is being sent.
        int filterID,             // Pass this ID to gt2FilteredSend.
        const GT2Byte *message,   // The message being sent.  Will be NULL if an empty message.
        int len,                  // The length of the message being sent, in bytes. Will be 0 if an
                                  // empty message.
        GT2Bool reliable          // If the message is being sent reliably.
);

// Callback for filtering incoming data.
// Call gt2FilteredRecieve with the filtered data,
// either from within the callback or later.
// the message may point to a memory location supplied to gt2FilteredReceive by
// a previous filter. so if this filter's call to gt2FilteredReceive is delayed,
// it is the filter's responsibility to make sure the data is still around when
// and if it is needed.
typedef void (*gt2ReceiveFilterCallback)(
        GT2Connection connection, // The connection the message was received on.
        int filterID,             // Pass this ID to gtFilteredReceive.
        GT2Byte *message,         // The message that was received.  Will be NULL if an
                                  // empty message.
        int len,                  // The length of the message in bytes.  Will be 0 if an empty
                                  // message.
        GT2Bool reliable          // True if this is a reliable message.
);

// this callback gets called when the sock receives a message that it cannot
// match to an existing connection.  if the callback recognizes the message and
// handles it, it should return GT2True, which will tell the socket to ignore
// the message.  if the callback does not recognize the message, it should
// return GT2False, which tells the socket to let the other side know there is
// no connection.
typedef GT2Bool (
        *gt2UnrecognizedMessageCallback)(GT2Socket socket, // the socket the message was received on
        unsigned int ip,     // the ip of the remote machine the message came from (in
                             // network byte order)
        unsigned short port, // the port on the remote machine (in host byte order)
        GT2Byte *message,    // the message (may be NULL for an empty message)
        int len              // the length of the message (may be 0)
);

// called with either sent or received data
// trying to send a message from within the send dump callback, or letting the
// socket think from within the receive dump callback can cause serious
// problems, and should not be done.
typedef void (*gt2DumpCallback)(GT2Socket socket, // the socket the message was on
        GT2Connection connection,                 // the connection the message was on, or NULL if
                                                  // there is no connection for this message
        unsigned int ip,                          // the remote ip, in network byte order
        unsigned short port,                      // the remote port, in host byte order
        GT2Bool reset,          // if true, the connection has been reset (only used by the
                                // receive callback)
        const GT2Byte *message, // the message (should not be modified)
        int len                 // the length of the message
);

// 0x8010ab64
u32 gt2Send(GT2Connection connection, const char *message, s32 len, bool reliable);

// 0x8010a964
void gt2Think(GT2Socket socket);
