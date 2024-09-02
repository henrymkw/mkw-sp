#pragma once

#include <Common.h>

typedef struct OSModuleLink {
    struct OSModuleLink *next;
    struct OSModuleLink *prev;
} OSModuleLink;
static_assert(sizeof(OSModuleLink) == 0x8);

typedef struct {
    u32 id;
    OSModuleLink link;
    u32 numSections;
    u32 sectionInfoOffset;
    u32 nameOffset;
    u32 nameSize;
    u32 version;
} OSModuleInfo;
static_assert(sizeof(OSModuleInfo) == 0x20);

typedef struct {
    OSModuleInfo info;
    u32 bssSize;
    u32 relOffset;
    u32 impOffset;
    u32 impSize;
    u8 prologSection;
    u8 epilogSection;
    u8 unresolvedSection;
    u8 bssSection;
    u32 prolog;
    u32 epilog;
    u32 unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
} OSModuleHeader;
static_assert(sizeof(OSModuleHeader) == 0x4c);

typedef struct {
    u32 offset;
    u32 size;
} OSSectionInfo;
static_assert(sizeof(OSSectionInfo) == 0x8);

typedef struct {
    u32 id;
    u32 offset;
} OSImportInfo;
static_assert(sizeof(OSImportInfo) == 0x8);

s32 OSLink(OSModuleHeader *newModule, void *bss);
// Not actually exposed in the API
void Relocate(OSModuleHeader *existingModule, OSModuleHeader *newModule); // 0x801a6d3c
