#include "qr2.h"

#include <sp/net/mkw_server/MKW-Server.h>

void qr2_shutdown(qr2 qr2) {
    // Reset mkw-server related globals
    SP_LOG("Resetting MKW Server Info");
    resetMKWServerInfo();

    REPLACED(qr2_shutdown)(qr2);
}
