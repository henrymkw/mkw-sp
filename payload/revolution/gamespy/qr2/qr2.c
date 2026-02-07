#include "qr2.h"

#include <revolution/dwc/DWCTransport.h>

#include <sp/net/MKW-Server.h>

void qr2_shutdown(qr2 qr2) {
    // Reset mkw-server related globals
    SP_LOG("Resetting MKW Server Address State on QR2 Shutdown");
    resetMKWServerInfo();

    REPLACED(qr2_shutdown)(qr2);
}
