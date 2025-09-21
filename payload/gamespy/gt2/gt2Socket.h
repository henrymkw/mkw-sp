#pragma once

#include <Common.h>

#include <gamespy/gt2/gt2.h>
#include <gamespy/gt2/gt2Main.h>

/*
    Performs a table look up for a socket's connection
*/
// 0x8010de8c
GT2Connection gti2SocketFindConnection(GT2Socket socket, unsigned int ip, unsigned short port);

/*
   gti2NewSocketConnection() takes in a GT2Socket and GT2Connection
   It will initialize a GT2Connection and call TableEnter(), adding the connection to the hash
   table It will set the passed in GT2Connection reference to the value returned by
   gti2SocketFindConnection()
*/
// 0x8010e17c
GT2Result gti2NewSocketConnection(GT2Socket socket, GT2Connection connection, unsigned int ip,
        unsigned short port);

/*
    gti2CreateSocket initializes a GT2Socket structure. It doesn't add any connections to it though.
    It can fail due to memory allocation or network errors.
*/
// 0x8010ded8
GT2Result gti2CreateSocket(GT2Socket *sock, const char *localAddress, int outgoingBufferSize,
        int incomingBufferSize, gt2SocketErrorCallback callback, GTI2ProtocolType type);

// just calls gti2Listen
// 0x8010a9b0
void gt2Listen(GT2Socket socket, gt2ConnectAttemptCallback callback);

// sets connectAttemptCallback on the socket
// 0x8010e174
void gti2Listen(GT2Socket socket, gt2ConnectAttemptCallback callback);

// 0x8010ac88
GT2ConnectionState gt2GetConnectionState(GT2Connection connection);
