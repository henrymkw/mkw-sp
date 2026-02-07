#include "eggSystem.h"

#include <game/system/Console.h>

void EGG_ProcessMeter_draw(void *processMeter) {
    REPLACED(EGG_ProcessMeter_draw)(processMeter);

    // until the settings infrastructure and ui is implemented, we
    // just have to comment/uncomment this line to toggle the in-game console.
    // Console_draw();
}