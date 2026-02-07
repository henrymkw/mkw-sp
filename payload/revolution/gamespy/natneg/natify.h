#pragma once

#include <Common.h>

typedef struct {
    u32 privateIp;
    u16 privatePort;
    u8 _07[0x08 - 0x07];

    u32 publicIp;
    u16 publicPort;
    u8 _0d[0xf - 0x0d];
} AddressMapping;
static_assert(sizeof(AddressMapping) == 0x10);

typedef struct {
    char brand[32];
    char model[32];
    char firmware[64];
    BOOL ipRestricted;
    BOOL portRestricted;
    int promiscuity;
    int type;
    int mappingScheme;
    AddressMapping mappings[4];
    BOOL qr2Compatible;
} NAT;
static_assert(sizeof(NAT) == 0xD8);