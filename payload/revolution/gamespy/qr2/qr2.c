#include "qr2.h"

#include <sp/net/MKW-Server.h>

void qr2_shutdown(qr2 qr2) {
    // Reset mkw-server related globals
    resetMKWServerInfo();

    REPLACED(qr2_shutdown)(qr2);
}
