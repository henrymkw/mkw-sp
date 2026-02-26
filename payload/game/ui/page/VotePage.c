#include "VotePage.h"

#include <sp/net/mkw_server/MKW-Server.h>

void onInit(void *thisPtr) {
    // Read comment in Room.cc, we get and start sending to mkw-server
    // on the voting page since match making only works in p2p
    // (even this temporary solution is bad since joining public rooms fails)
    if (hasSentMKWServerAddressRequest == false) {
        tryGetMKWServerAddress();
    }
    REPLACED(onInit)(thisPtr);
}
