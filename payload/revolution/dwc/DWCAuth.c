#include "DWCAuth.h"

#include <sp/net/WiiLink.h>

void DWCi_Auth_SendRequest(u32 authStage, wchar_t *miiName, char *gameId, u32 r6, u64 userId) {
    // call the original function if the payload is ready
    if (wwfcPayloadReady()) {
        REPLACED(DWCi_Auth_SendRequest)(authStage, miiName, gameId, r6, userId);
        return;
    }

    NHTTPRequestHandle request = createWFCAuthRequest();
    if (request == NULL) {
        return;
    }
    s_DWCAuthControl->handle = NHTTPSendRequestAsync(request);
}

void setAuthError(s32 error) {
    s_auth_error = error;
}
