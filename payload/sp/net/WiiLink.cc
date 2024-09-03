#include "WiiLink.hh"
#include <revolutionex/nhttp.h>

extern "C" {
#include <revolution.h>
#include <revolution/dvd.h>
#include <revolution/ios.h>
}
#include <cstdio>
#include <cstring>
#include <string>

extern s32 s_auth_error;
extern s32 *s_auth_work;

namespace Net {

static u8 s_saltHash[SHA256_DIGEST_SIZE];
static Net::WWFCPayload *s_payload;
static u8 s_payloadBlock[PAYLOAD_BLOCK_SIZE];
static bool s_payloadReady = false;
typedef s32 (*EntryFunction)(Net::WWFCPayload *);
static EntryFunction entry;

bool GenerateRandomSalt(u8 *out) {
    // Generate cryptographic random with ES_Sign
    s32 fd = IOS_Open("/dev/es", IPC_OPEN_NONE);
    if (fd < 0) {
        return false;
    }

    u8 dummy = 0x7a;
    u8 eccCert[0x180];
    u8 eccSignature[0x3C];

    IOVector vec[3];
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
        return false;
    }

    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, eccSignature, 0x3C);
    SHA256Update(&ctx, eccCert, 0x180);
    memcpy(out, SHA256Final(&ctx), SHA256_DIGEST_SIZE);
    return true;
}

void OnPayloadReceived(NHTTPError result, NHTTPResponseHandle response, void * /* userdata */) {
    s32 error = 0;
    if (response == NULL) {
        return;
    }

    NHTTPDestroyResponse(response);

    if (result != NHTTP_ERROR_NONE) {
        return;
    }

    if (*reinterpret_cast<u32 *>(s_payload) != 0x57574643 /* WWFC */) {
        error = PayloadErrors::STAGE1_HEADER_CHECK;
    }

    if (s_payload->header.total_size < sizeof(WWFCPayload) ||
            s_payload->header.total_size > PAYLOAD_BLOCK_SIZE) {
        error = PayloadErrors::STAGE1_LENGTH_ERROR;
    }

    if (memcmp(s_payload->salt, s_saltHash, SHA256_DIGEST_SIZE) != 0) {
        error = PayloadErrors::STAGE1_SALT_MISMATCH;
    }

    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, (const u8 *)s_payload + sizeof(WWFCPayloadHeader),
            s_payload->header.total_size - sizeof(WWFCPayloadHeader));
    u8 *hash = SHA256Final(&ctx);

    if (!RSAVerify(reinterpret_cast<const RSAPublicKey *>(PayloadPublicKey),
                s_payload->header.signature, hash)) {
        error = PayloadErrors::STAGE1_SIGNATURE_INVALID;
    }

    for (u32 i = 0; i < PAYLOAD_BLOCK_SIZE; i += 0x20) {
        __asm__ __volatile__(
                "dcbf %0, %1\n\t"
                "sync\n\t"
                "icbi %0, %1\n\t"
                "isync\n\t"
                :
                : "r"(i), "r"(s_payload)
                : "memory");
    }

    entry = reinterpret_cast<EntryFunction>(
            reinterpret_cast<u8 *>(s_payload) + s_payload->info.entry_point);
    assert(entry != NULL);

    error = entry(s_payload);
    if (error != 0) {
        s_auth_error = error;
        return;
    }

    s_payloadReady = true;
    s_auth_error = -1; // This error code will retry auth
}

void DWCi_Auth_SendRequest(int param_1, wchar_t *param_2, char *param_3, int param_4, int param_5,
        int param_6) {
    if (s_payloadReady) {
        REPLACED(DWCi_Auth_SendRequest)(param_1, param_2, param_3, param_4, param_5, param_6);
        return;
    }
    // Align the payload block to a 32-byte boundary
    s_payload = reinterpret_cast<WWFCPayload *>((reinterpret_cast<u32>(s_payloadBlock) + 31) & ~31);
    memset(s_payload, 0, PAYLOAD_BLOCK_SIZE);

    u8 salt[SHA256_DIGEST_SIZE];
    if (!GenerateRandomSalt(salt)) {
        s_auth_error = PayloadErrors::STAGE1_MAKE_REQUEST;
    }
    static const char *hexConv = "0123456789abcdef";
    char saltHex[SHA256_DIGEST_SIZE * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        saltHex[i * 2] = hexConv[salt[i] >> 4];
        saltHex[i * 2 + 1] = hexConv[salt[i] & 0xf];
    }
    saltHex[SHA256_DIGEST_SIZE * 2] = 0;

    char uri[0x100];
    sprintf(uri, "payload?g=RMC%cD00&s=%s", *reinterpret_cast<char *>(0x80000003), saltHex);

    // Generate salt hash
    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, uri, strlen(uri));
    memcpy(s_saltHash, SHA256Final(&ctx), SHA256_DIGEST_SIZE);

    char url[0x124];
    sprintf(url, "http://nas.%s/%s&h=%02x%02x%02x%02x", WWFC_DOMAIN, uri, s_saltHash[0],
            s_saltHash[1], s_saltHash[2], s_saltHash[3]);

    void *request = NHTTPCreateRequest(url, NHTTPRequestMethod::NHTTP_REQUEST_METHOD_GET,
            reinterpret_cast<char *>(s_payload), PAYLOAD_BLOCK_SIZE, OnPayloadReceived, 0);

    if (request == NULL) {
        s_auth_error = PayloadErrors::STAGE1_MAKE_REQUEST;
        return;
    }

    s_auth_work[0x59E0 / 4] = NHTTPSendRequestAsync(request);
}

} // namespace Net
