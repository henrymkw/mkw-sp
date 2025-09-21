#include "wiilink.h"

#include <Common.h>

#include <revolution.h>
#include <revolution/dvd.h>
#include <revolution/ios.h>
#include <revolutionex/nhttp.h>

#include "string.h"
#include <stdio.h>

_Alignas(0x20) static u8 s_payloadBlock[PAYLOAD_BLOCK_SIZE + 0x20];
static WWFCPayload *s_payload = NULL;
static bool s_payloadReady = false;
static u8 s_saltHash[SHA256_DIGEST_SIZE];

typedef s32 (*EntryFunction)(WWFCPayload *);
static EntryFunction entry = NULL;

extern s32 s_auth_error;
// TODO: Make a type for this
extern s32 *s_DWCAuthContext;

bool GenerateRandomSalt(u8 *out) {
    // Generate cryptographic random with ES_Sign
    s32 fd = IOS_Open("/dev/es", IPC_OPEN_NONE);
    if (fd < 0) {
        SP_LOG("Failed to open /dev/es: %d", fd);
        return false;
    }

    _Alignas(0x40) u8 dummy[0x20];
    dummy[0x0] = 0x7a;
    _Alignas(0x40) u8 eccCert[0x180];
    _Alignas(0x40) u8 eccSignature[0x3C];

    _Alignas(0x40) IOVector vec[3];
    vec[0].data = &dummy;
    vec[0].size = 1;
    vec[1].data = eccSignature;
    vec[1].size = 0x3C;
    vec[2].data = eccCert;
    vec[2].size = 0x180;

    // ES_Sign
    s32 ret = IOS_Ioctlv(fd, (IOSCommand)(0x30), 1, 2, vec);
    IOS_Close(fd);

    if (ret < 0) {
        SP_LOG("ES_Sign failed: %d", ret);
        return false;
    }

    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, eccSignature, 0x3C);
    SHA256Update(&ctx, eccCert, 0x180);
    memcpy(out, SHA256Final(&ctx), SHA256_DIGEST_SIZE);
    return true;
}

s32 HandleResponse() {
    // ensure magic integrity
    if (memcmp(s_payload->header.magic, "WWFC", 4)) {
        SP_LOG("Invalid payload magic: %.4s", s_payload->header.magic);
        return WL_ERROR_PAYLOAD_STAGE1_HEADER_CHECK;
    }

    // ensure payload sizes are valid
    if (s_payload->header.total_size < sizeof(WWFCPayload) ||
            s_payload->header.total_size > PAYLOAD_BLOCK_SIZE) {
        SP_LOG("Invalid payload size: %d", s_payload->header.total_size);
        return WL_ERROR_PAYLOAD_STAGE1_LENGTH_ERROR;
    }

    // ensure the salt hashes match
    if (memcmp(s_payload->salt, s_saltHash, SHA256_DIGEST_SIZE) != 0) {
        SP_LOG("Payload salt mismatch");
        return WL_ERROR_PAYLOAD_STAGE1_SALT_MISMATCH;
    }

    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, s_payload->salt, s_payload->header.total_size - sizeof(WWFCHeader));
    u8 *hash = SHA256Final(&ctx);

    if (!RSAVerify(&PayloadPublicKey, s_payload->header.signature, hash)) {
        SP_LOG("Invalid payload signature");
        return WL_ERROR_PAYLOAD_STAGE1_SIGNATURE_INVALID;
    }

    // Disable unnecessary patches
    u32 patchMask = WWFC_PATCH_LEVEL_CRITICAL | WWFC_PATCH_LEVEL_SUPPORT;

    WWFCPatch *patch = (WWFCPatch *)((u8 *)s_payload + s_payload->info.patch_list_offset);
    WWFCPatch *patchesEnd = (WWFCPatch *)((u8 *)s_payload + s_payload->info.patch_list_end);

    for (; patch < patchesEnd; patch++) {
        if (patch->level == WWFC_PATCH_LEVEL_CRITICAL || (patch->level & patchMask)) {
            continue;
        }

        // Otherwise disable the patch
        patch->level |= WWFC_PATCH_LEVEL_DISABLED;
    }

    entry = (EntryFunction)((u8 *)s_payload + s_payload->info.entry_point);
    assert(entry != NULL);
    return entry(s_payload);
}

void OnPayloadReceived(NHTTPError result, NHTTPResponseHandle response, void * /*userdata */) {
    // check for valid responce
    if (response == NULL) {
        SP_LOG("NHTTP Response is NULL");
        return;
    }

    // destroy responce
    NHTTPDestroyResponse(response);

    if (result != NHTTP_ERROR_NONE) {
        SP_LOG("NHTTP Error: %d", result);
        return;
    }

    s32 error = HandleResponse();
    if (error != 0) {
        // setting s_auth_error will retry
        SP_LOG("Payload handling error: %d", error);
        s_auth_error = error;
        return;
    }

    s_payloadReady = true;
    // -1 indicates success
    SP_LOG("Payload successfully received and ready");
    s_auth_error = -1;
}

REPLACE void DWCi_Auth_SendRequest(u32 authStage, wchar_t *miiName, char *gameId, u32 r6,
        u64 userId) {
    // call the original function if the payload is ready
    if (s_payloadReady) {
        REPLACED(DWCi_Auth_SendRequest)(authStage, miiName, gameId, r6, userId);
        return;
    }

    s_payload = (WWFCPayload *)s_payloadBlock;
    memset(s_payload, 0, PAYLOAD_BLOCK_SIZE);

    u8 salt[SHA256_DIGEST_SIZE];
    if (!GenerateRandomSalt(salt)) {
        s_auth_error = WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST;
    }

    // convert the salt to characters so we can use it in the url
    const char *hexConv = "0123456789abcdef";
    char saltHex[SHA256_DIGEST_SIZE * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        saltHex[i * 2] = hexConv[salt[i] >> 4];
        saltHex[i * 2 + 1] = hexConv[salt[i] & 0xf];
    }
    saltHex[SHA256_DIGEST_SIZE * 2] = 0;

    char uri[0x100];
    // TODO: theres a better way to write this, gameId is gsbrcd, maybe 0x80000000 can be used
    sprintf(uri, "payload?g=RMC%cD00&s=%s", *(char *)0x80000003, saltHex);

    // Generate salt hash
    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, uri, strlen(uri));
    memcpy(s_saltHash, SHA256Final(&ctx), SHA256_DIGEST_SIZE);

    char url[0x100];
    snprintf(url, sizeof(url), "http://%.*s/%.*s&h=%02x%02x%02x%02x", 63,
            WWFC_DOMAIN, // limit domain to 63 chars
            127, uri,    // limit uri to 127 chars
            s_saltHash[0], s_saltHash[1], s_saltHash[2], s_saltHash[3]);
    SP_LOG("Requesting payload from URL: %s", url);

    // build the request
    void *request =
            NHTTPCreateRequest(url, 0, (void *)s_payload, PAYLOAD_BLOCK_SIZE, OnPayloadReceived, 0);

    // return if theres an error building the request
    if (request == NULL) {
        SP_LOG("Failed to create NHTTP request");
        s_auth_error = WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST;
        return;
    }

    // send the request
    s_DWCAuthContext[0x59E0] = NHTTPSendRequestAsync(request);
}
