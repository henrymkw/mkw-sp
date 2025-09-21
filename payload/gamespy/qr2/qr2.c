#include "qr2.h"

#include <revolution/dwc/DWCTransport.h>

void qr2_shutdown(qr2 qr2) {
    // Reset mkw-server related globals
    hasMKWServerAddress = false;
    hasSentMKWServerAddressRequest = false;
    mkwServerIp = 0;
    mkwServerPort = 0;

    REPLACED(qr2_shutdown)(qr2);
}
