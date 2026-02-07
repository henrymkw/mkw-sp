#pragma once

#include "Font.h"

extern Font *sDebugFont;

typedef struct {
    Font *fonts[6];
} FontManager;
static_assert(sizeof(FontManager) == 0x18);

// REPLACE void FontManager_init(FontManager *this);
// void REPLACED(FontManager_init)(FontManager *this);
