#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0x4c - 0x00];
} SBQueryEngine;
static_assert(sizeof(SBQueryEngine) == 0x4c);

typedef struct {
    u8 _000[0x07c - 0x000];
    char *inbuffer;
    s32 inbufferlen;
    u8 _084[0x6b4 - 0x084];
    s32 slsocket;
    u8 _6b8[0x7dc - 0x6b8];
} SBServerList;
static_assert(sizeof(SBServerList) == 0x7dc);

typedef struct {
    SBQueryEngine engine;
    SBServerList list;
    u8 _828[0x840 - 0x828];
} ServerBrowser;
static_assert(sizeof(ServerBrowser) == 0x840);

ServerBrowser *ServerBrowserNewA(const char *queryForGamename, const char *queryFromGamename,
        const char *queryFromKey, s32 queryFromVersion, s32 maxConcUpdates, s32 queryVersion,
        BOOL lanBrowse, void *sbCallback, void *instance);

BOOL SBServerListConnectAndQuery(SBServerList *list, const char *fieldList,
        const char *serverFilter, int options, int maxServers);

BOOL ServerListConnect(SBServerList *list);

void gsifree(void *ptr);

int closesocket(int sock);

void SBServerListDisconnect(SBServerList *list);
