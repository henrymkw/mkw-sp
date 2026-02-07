#pragma once

#include <Common.h>

typedef struct {
    u32 magic;
    s32 fstStart;
    s32 fstSize;
    s32 fileStart;
    u8 _10[0x20 - 0x10];
} ARCHeader;
static_assert(sizeof(ARCHeader) == 0x20);

typedef struct {
    void *archiveStartAddr;
    void *FSTStart;
    void *fileStart;
    u32 entryNum;
    char *FSTStringStart;
    u32 FSTLength;
    u32 currDir;
} ARCHandle;
static_assert(sizeof(ARCHandle) == 0x1c);

typedef struct {
    ARCHandle *handle;
    u32 startOffset;
    u32 length;
} ARCFileInfo;
static_assert(sizeof(ARCFileInfo) == 0xc);

typedef struct {
    ARCHandle *handle;
    u32 entryNum;
    u32 location;
    u32 next;
} ARCDir;
static_assert(sizeof(ARCDir) == 0x10);

typedef struct {
    ARCHandle *handle;
    u32 entryNum;
    BOOL isDir;
    char *name;
} ARCDirEntry;
static_assert(sizeof(ARCDirEntry) == 0x10);

BOOL ARCInitHandle(void *arcStart, ARCHandle *handle);

BOOL ARCOpen(ARCHandle *handle, const char *fileName, ARCFileInfo *af);

BOOL ARCFastOpen(ARCHandle *handle, s32 entrynum, ARCFileInfo *af);

s32 ARCConvertPathToEntrynum(ARCHandle *handle, const char *pathPtr);

u32 ARCGetStartOffset(ARCFileInfo *af);

u32 ARCGetLength(ARCFileInfo *af);

BOOL ARCClose(ARCFileInfo *af);

BOOL ARCOpenDir(ARCHandle *handle, const char *dirName, ARCDir *dir);

BOOL ARCFastOpenDir(ARCHandle *handle, s32 entrynum, ARCDir *dir); // Not in the SDK

BOOL ARCReadDir(ARCDir *dir, ARCDirEntry *dirent);

BOOL ARCCloseDir(ARCDir *dir);

BOOL ARCChangeDir(ARCHandle *handle, const char *pathPtr);

void ARCExClone(const ARCFileInfo *src, ARCFileInfo *dst);

void ARCExCloneDir(const ARCDir *src, ARCDir *dst);
