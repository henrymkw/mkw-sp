#include "WiiLink.h"

#ifndef _WIILINK_
#define _WIILINK_

#include <revolution.h>
#include <revolutionex/nhttp.h>
#include <revolution/ios.h>
#include <revolution/dvd.h>

#include "string.h"
#include <stdio.h>

static u8 s_payloadBlock[PAYLOAD_BLOCK_SIZE + 0x20];
static void *s_payload = NULL;
static bool s_payloadReady = false;
static u8 s_saltHash[SHA256_DIGEST_SIZE];

typedef s32 (*EntryFunction)(wwfc_payload *);
static EntryFunction entry = NULL;

extern s32 s_auth_error;
extern s32 *s_auth_work;

bool GenerateRandomSalt(u8 *out)
{
    // Generate cryptographic random with ES_Sign
    s32 fd = IOS_Open("/dev/es", IPC_OPEN_NONE);
    if (fd < 0)
    {
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

    if (ret < 0)
    {
        return false;
    }

    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, eccSignature, 0x3C);
    SHA256Update(&ctx, eccCert, 0x180);
    memcpy(out, SHA256Final(&ctx), SHA256_DIGEST_SIZE);
    return true;
}

s32 HandleResponse(u8 *block)
{
    register wwfc_payload *__restrict payload =
        (wwfc_payload *)block;

    if (*(u32 *)payload != 0x57574643 /* WWFC */)
    {
        return WL_ERROR_PAYLOAD_STAGE1_HEADER_CHECK;
    }

    if (payload->header.total_size < sizeof(wwfc_payload) ||
        payload->header.total_size > PAYLOAD_BLOCK_SIZE)
    {
        return WL_ERROR_PAYLOAD_STAGE1_LENGTH_ERROR;
    }

    if (memcmp(payload->salt, s_saltHash, SHA256_DIGEST_SIZE) != 0)
    {
        return WL_ERROR_PAYLOAD_STAGE1_SALT_MISMATCH;
    }

    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(
        &ctx, (const u8 *)payload + sizeof(wwfc_payload_header),
        payload->header.total_size - sizeof(wwfc_payload_header));
    u8 *hash = SHA256Final(&ctx);

    if (!RSAVerify(
            (const RSAPublicKey *)PayloadPublicKey,
            payload->header.signature, hash))
    {
        return WL_ERROR_PAYLOAD_STAGE1_SIGNATURE_INVALID;
    }

    
    for (register u32 i = 0; i < 0x20000; i += 0x20)
    {
        __asm__ __volatile__ (
            "dcbf %0, %1\n\t"
            "sync\n\t"
            "icbi %0, %1\n\t"
            "isync\n\t"
            : 
            : "r"(i), "r"(payload)
            : "memory"
        );
    }

    // Disable unnecessary patches
    u32 patchMask = WWFC_PATCH_LEVEL_CRITICAL | WWFC_PATCH_LEVEL_BUGFIX |
                    WWFC_PATCH_LEVEL_SUPPORT;
    for (wwfc_patch *patch = (wwfc_patch*)(
                        block + payload->info.patch_list_offset
                    ),
                    *end = (wwfc_patch*)(
                        block + payload->info.patch_list_end
                    );
         patch < end; patch++) {
        if (patch->level == WWFC_PATCH_LEVEL_CRITICAL ||
            (patch->level & patchMask)) {
            continue;
        }

        // Otherwise disable the patch
        patch->level |= WWFC_PATCH_LEVEL_DISABLED;
    }

    entry =
        (EntryFunction)(
            (u8 *)payload + payload->info.entry_point);
    assert(entry != NULL);
    SP_LOG("entry: %p", entry);
    return entry(payload);
}

void OnPayloadReceived(NHTTPError result, NHTTPResponseHandle response, void * /*userdata */)
{
    if (response == NULL)
    {
        return;
    }

    NHTTPDestroyResponse(response);

    if (result != 0)
    {
        return;
    }

    s32 error = HandleResponse((u8 *) s_payload);
    SP_LOG("ran payload error: %d", error);
    if (error != 0)
    {
        s_auth_error = error;
        return;
    }

    s_payloadReady = true;
    s_auth_error = -1; // This error code will retry auth
}

REPLACE void DWCi_Auth_SendRequest(
    int param_1, wchar_t *param_2, char *param_3, int param_4,
    int param_5, int param_6)
{
    SP_LOG("entry: %p", entry);
    if (s_payloadReady)
    {
        REPLACED(DWCi_Auth_SendRequest)(param_1, param_2, param_3, param_4, param_5, param_6);
        return;
    }

    s_payload = (void *)((( (u32)s_payloadBlock) + 31) & ~31);
    memset(s_payload, 0, PAYLOAD_BLOCK_SIZE);

    u8 salt[SHA256_DIGEST_SIZE];
    if (!GenerateRandomSalt(salt))
    {
        s_auth_error = WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST;
    }
    static const char *hexConv = "0123456789abcdef";
    char saltHex[SHA256_DIGEST_SIZE * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++)
    {
        saltHex[i * 2] = hexConv[salt[i] >> 4];
        saltHex[i * 2 + 1] = hexConv[salt[i] & 0xf];
    }
    saltHex[SHA256_DIGEST_SIZE * 2] = 0;

    char uri[0x100];
    sprintf(uri, "payload?g=RMC%cD00&s=%s", *(char *)0x80000003, saltHex);

    // Generate salt hash
    SHA256Context ctx;
    SHA256Init(&ctx);
    SHA256Update(&ctx, uri, strlen(uri));
    memcpy(s_saltHash, SHA256Final(&ctx), SHA256_DIGEST_SIZE);

    char url[0x100];
    sprintf(
        url, "http://nas.%s/%s&h=%02x%02x%02x%02x", WWFC_DOMAIN, uri,
        s_saltHash[0], s_saltHash[1], s_saltHash[2], s_saltHash[3]);


    void *request = NHTTPCreateRequest(
        url, 0, s_payload, PAYLOAD_BLOCK_SIZE, OnPayloadReceived, 0);

    SP_LOG("entry: %p", entry);
    if (request == NULL)
    {
        s_auth_error = WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST;
        return;
    }
    s_auth_work[0x59E0 / 4] = NHTTPSendRequestAsync(request);
}

#endif
