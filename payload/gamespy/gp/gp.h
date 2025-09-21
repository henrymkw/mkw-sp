#pragma once

#include <Common.h>

typedef struct {
    void *cb;
    void *param;
} callback;

typedef struct {
    char *buffer;
    int size;
    int len;
    int pos;
} buffer;

typedef struct {
    char errorString[256];             // 0x00
    BOOL infoCaching;                  // 0x100
    BOOL infoCachingBuddyAndBlockOnly; // 0x104
    BOOL simulation;                   // 0x108
    BOOL firewall;                     // 0x10C
    char nick[31];                     // 0x110
    char uniquenick[21];
    char email[51];
    char password[31];
    int sessKey;
    int userid;
    int profileid;
    int partnerID;
    callback callbacks[6];
    u32 cmSocket;
    int connectState;
    buffer socketBuffer;
    char *inputBuffer;
    int inputBufferSize;
    buffer outputBuffer;
    // Replaced by UDP Layer
    // SOCKET peerSocket;
    char mHeader[16];
    unsigned short peerPort;
    int nextOperationID;
    int numSearches;

    u32 lastStatusState;
    unsigned int hostIp;
    unsigned int hostPrivateIp;
    unsigned short queryPort;
    unsigned short hostPort;
    unsigned int sessionFlags;

    char richStatus[256];
    char gameType[33];
    char gameVariant[33];
    char gameMapName[33];

    // New Status Info extended info Keys
    void *extendedInfoKeys;

    // Deprecated
    char lastStatusString[33];
    char lastLocationString[256];

    u32 errorCode;
    u32 fatalError;
    void *diskCache;
    void *operationList;
    void *profileList;
    void *peerList;
    void *callbackList;
    void *lastCallback;
    buffer updateproBuffer;
    buffer updateuiBuffer;
    void *transfers; // matches up to here
    u32 nextTransferID;
    int productID;
    int namespaceID;
    char loginTicket[25];
    u32 quietModeFlags;
    u32 kaTransmit;
} GPIConnection;

u32 gpProcess(GPIConnection *connection);
