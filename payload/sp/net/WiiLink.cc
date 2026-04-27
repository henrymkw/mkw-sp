#include "WiiLink.hh"

extern "C" {
#include <revolution.h>
}

#include <stdio.h>
#include <string.h>
namespace MKWServer {
_Alignas(0x20) static u8 s_payloadBlock[PAYLOAD_BLOCK_SIZE + 0x20];
static WWFCPayload *s_payload = NULL;
static bool s_payloadReady = false;
static u8 s_saltHash[SHA256_DIGEST_SIZE];

typedef s32 (*EntryFunction)(WWFCPayload *);
static EntryFunction entry = NULL;

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
    vec[0].size = sizeof(dummy[0]);
    vec[1].data = eccSignature;
    vec[1].size = sizeof(eccSignature);
    vec[2].data = eccCert;
    vec[2].size = sizeof(eccCert);

    // ES_Sign
    s32 ret = IOS_Ioctlv(fd, 0x30, 1, 2, vec);
    IOS_Close(fd);

    if (ret < 0) {
        SP_LOG("ES_Sign failed: %d", ret);
        return false;
    }

    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, eccSignature, sizeof(eccSignature));
    SHA256Update(&ctx, eccCert, sizeof(eccCert));
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
    WWFCPatchLevel patchMask = WWFCPatchLevel::Critical | WWFCPatchLevel::Support |
            WWFCPatchLevel::Bugfix | WWFCPatchLevel::Parity | WWFCPatchLevel::Feature;

    WWFCPatch *patch = (WWFCPatch *)((u8 *)s_payload + s_payload->info.patch_list_offset);
    WWFCPatch *patchesEnd = (WWFCPatch *)((u8 *)s_payload + s_payload->info.patch_list_end);

    for (; patch < patchesEnd; patch++) {
        if (patch->level == WWFCPatchLevel::Critical ||
                static_cast<u8>(patch->level & patchMask) != 0) {
            continue;
        }

        // Otherwise disable the patch
        patch->level |= WWFCPatchLevel::Disabled;
    }

    entry = (EntryFunction)((u8 *)s_payload + s_payload->info.entry_point);
    assert(entry != NULL);
    return entry(s_payload);
}

void OnPayloadReceived(NHTTPError result, NHTTPResponseHandle response, void * /*userdata */) {
    // check for valid response
    if (response == NULL) {
        SP_LOG("NHTTP Response is NULL");
        return;
    }

    // destroy response
    NHTTPDestroyResponse(response);

    if (result != NHTTP_ERROR_NONE) {
        SP_LOG("NHTTP Error: %d", result);
        return;
    }

    s32 error = HandleResponse();
    if (error != 0) {
        // setting s_auth_error will retry
        SP_LOG("Payload handling error: %d", error);
        setAuthError(error);
        return;
    }

    s_payloadReady = true;
    // -1 indicates success
    SP_LOG("Payload successfully received and ready");
    setAuthError(-1);
}

EXTERN_C bool wwfcPayloadReady() {
    return s_payloadReady;
}

EXTERN_C NHTTPRequestHandle createWFCAuthRequest() {
    s_payload = (WWFCPayload *)s_payloadBlock;
    memset(s_payload, 0, PAYLOAD_BLOCK_SIZE);

    u8 salt[SHA256_DIGEST_SIZE];
    if (!GenerateRandomSalt(salt)) {
        setAuthError(WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST);
    }

    // convert the salt to characters so we can use it in the url
    char saltHex[SHA256_DIGEST_SIZE * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        snprintf(saltHex + i * 2, 3, "%02x", salt[i]);
    }

    char uri[0x100];
    // TODO: theres a better way to write this, gameId is gsbrcd, maybe 0x80000000 can be used
    sprintf(uri, "payload?g=RMC%cD00&s=%s", DISK_REGION, saltHex);

    // Generate salt hash
    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, uri, strlen(uri));
    memcpy(s_saltHash, SHA256Final(&ctx), SHA256_DIGEST_SIZE);

    char url[0x100];
    // Cap domain to 63 chars, uri to 127 chars, include first 4 bytes of salt hash
    snprintf(url, sizeof(url), "http://%.63s/%.127s&h=%02x%02x%02x%02x", WWFC_DOMAIN, uri,
            s_saltHash[0], s_saltHash[1], s_saltHash[2], s_saltHash[3]);
    SP_LOG("Requesting payload from URL: %s", url);

    // build the request
    NHTTPRequestHandle request = NHTTPCreateRequest(url, NHTTP_REQUEST_METHOD_GET,
            reinterpret_cast<char *>(s_payload), PAYLOAD_BLOCK_SIZE, OnPayloadReceived, 0);

    if (request == NULL) {
        SP_LOG("Failed to create NHTTP request");
        setAuthError(WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST);
    }

    return request;
}

WWFCPatchLevel operator|(WWFCPatchLevel a, WWFCPatchLevel b) {
    return static_cast<WWFCPatchLevel>(static_cast<u8>(a) | static_cast<u8>(b));
}

WWFCPatchLevel &operator|=(WWFCPatchLevel &a, WWFCPatchLevel b) {
    return a = a | b;
}
WWFCPatchLevel operator&(WWFCPatchLevel a, WWFCPatchLevel b) {
    return static_cast<WWFCPatchLevel>(static_cast<u8>(a) & static_cast<u8>(b));
}
} // namespace MKWServer