#include <Common.hh>

extern "C" {
#include <revolutionex/nhttp.h>
#include <vendor/rsa/rsa.h>
#include <vendor/sha256/sha256.h>
}

#define WWFC_DOMAIN "wiilink24.com"
#define PAYLOAD_BLOCK_SIZE 0x20000 // 2^17, 128 kb
#define WWFC_CUSTOM_REGION 0x80005EFC

namespace Net {

enum WWFCPatchLevel : u8 {
    // security patches, required to connect to the server, applied automatically.
    CRITICAL = 0x00,
    // Patches that fix bugs in the game, such as anti-freeze patches.
    BUGFIX = 0x01,
    // Patches required for parity with clients using a regular WWFC patcher.
    PARITY = 0x02,
    // Additional feature, not required to be compatible with regular clients.
    FEATURE = 0x04,
    // General support patches that may be redundant depending on the patcher. Used in cases like
    // URL patches.
    SUPPORT = 0x08,
    // Flag used to disable the patch if it has been already applied.
    DISABLED = 0x10,
};

enum PayloadErrors : s32 {
    OK = 0,
    STAGE0_MISSING_STAGE1 = -20901,
    STAGE0_HASH_MISMATCH = -20902,
    STAGE1_ALLOC = -20910,
    STAGE1_MAKE_REQUEST = -20911,
    STAGE1_RESPONSE = -20912,
    STAGE1_HEADER_CHECK = -20913,
    STAGE1_LENGTH_ERROR = -20914,
    STAGE1_SALT_MISMATCH = -20915,
    STAGE1_GAME_ID_MISMATCH = -20916,
    STAGE1_SIGNATURE_INVALID = -20917,
    STAGE1_WAITING = -20918,
    GAME_MISMATCH = -20930,
};

struct WWFCPayloadHeader {
    char magic[0xC]; // Always "WWFC/Payload"
    u32 total_size;
    u8 signature[0x100]; // RSA-2048 signature
} __attribute__((packed));
static_assert(sizeof(WWFCPayloadHeader) == 0x110);

struct WWFCPayloadInfo {
    u32 format_version;        // Payload format version
    u32 format_version_compat; // Minimum payload format version that this
                               // payload is compatible with
    char name[0xC];            // Payload name (e.g. "RMCPD00")
    u32 version;               // Payload version
    u32 got_start;             // global offset table start
    u32 got_end;               // global offset table end
    u32 fixup_start;
    u32 fixup_end;
    u32 patch_list_offset;
    u32 patch_list_end;
    u32 entry_point;
    u32 entry_point_no_got;
    u32 reserved[0x18 / 4];
    char build_timestamp[0x20];
} __attribute__((packed));
static_assert(sizeof(WWFCPayloadInfo) == 0x70);

struct WWFCPayload {
    WWFCPayloadHeader header;
    u8 salt[SHA256_DIGEST_SIZE];
    WWFCPayloadInfo info;
} __attribute__((packed));
static_assert(sizeof(WWFCPayload) == 0x1A0);

struct WWFCPatch {
    u8 level; // wwfc_patch_level
    u8 type;  // wwfc_patch_type
    u8 reserved[2];
    u32 address;

    union {
        u32 arg0;
        const void *arg0p;
        const u32 *arg0p32;
    };

    u32 arg1;
} __attribute__((packed));

bool GenerateRandomSalt(u8 *out);

/**
 * @brief callback function for the HTTP request, recieves payload
 *
 */
static void OnPayloadReceived(NHTTPError result, NHTTPResponseHandle response, void *userdata);
void REPLACED(DWCi_Auth_SendRequest)(int param_1, wchar_t *param_2, char *param_3, int param_4,
        int param_5, int param_6);
REPLACE void DWCi_Auth_SendRequest(int param_1, wchar_t *param_2, char *param_3, int param_4,
        int param_5, int param_6);

// clang-format off
    // Production payload key
static constexpr u8 PayloadPublicKey[] = {
    0x7d, 0x18, 0x2b, 0x27, 0x9c, 0xab, 0x6b, 0x69, 0x40, 0x8d, 0x11, 0xb7,
    0x5e, 0x76, 0xb2, 0xda, 0xa6, 0x18, 0xa1, 0x0e, 0x5d, 0x5f, 0xf3, 0xc9,
    0x0a, 0x51, 0xfd, 0x69, 0x03, 0xa6, 0x96, 0x70, 0x28, 0xcf, 0x22, 0x4b,
    0xbd, 0xbd, 0xb2, 0x62, 0xaa, 0xa0, 0x07, 0x9a, 0x7a, 0xbf, 0xdc, 0x99,
    0x89, 0x53, 0x4a, 0xfc, 0xfe, 0xa0, 0xf4, 0xa6, 0x46, 0x11, 0xb2, 0x03,
    0xd0, 0xb1, 0xf1, 0x09, 0xb2, 0xe2, 0x3f, 0x64, 0xac, 0x5d, 0x8a, 0x05,
    0x05, 0x52, 0x18, 0xb4, 0xd8, 0x75, 0x44, 0xdb, 0xb7, 0x71, 0x61, 0xa8,
    0x50, 0x15, 0x3c, 0xb0, 0x07, 0x65, 0x7b, 0x95, 0x34, 0xfa, 0xe6, 0x02,
    0xa7, 0x3d, 0xbb, 0xad, 0x44, 0x68, 0x39, 0x36, 0x34, 0x45, 0xf5, 0x16,
    0x75, 0x9e, 0x40, 0xff, 0xf5, 0x64, 0xe6, 0x77, 0x5d, 0x19, 0x22, 0xc8,
    0x87, 0xcd, 0x61, 0x6e, 0x1a, 0xc8, 0xc3, 0xff, 0xc2, 0x72, 0x54, 0x57,
    0xff, 0xa0, 0x12, 0xcf, 0x73, 0x50, 0xc0, 0x54, 0xf0, 0xa2, 0x77, 0x19,
    0x63, 0x56, 0x20, 0x6d, 0x9b, 0x89, 0xd2, 0x22, 0xcc, 0x04, 0x4f, 0xc4,
    0x02, 0xd4, 0x59, 0xb1, 0xdf, 0x80, 0x29, 0xf9, 0xee, 0x12, 0x7d, 0x82,
    0x9a, 0x24, 0xc4, 0x50, 0xfe, 0x05, 0x35, 0xde, 0x33, 0xd9, 0xf0, 0xc0,
    0xa1, 0xdb, 0x1b, 0x80, 0x79, 0x35, 0x3c, 0x64, 0x82, 0x52, 0xdf, 0x81,
    0x15, 0x5d, 0x54, 0x19, 0x5a, 0xa2, 0x60, 0x65, 0xc7, 0x1e, 0xae, 0xaf,
    0xb9, 0x08, 0xc4, 0xf8, 0xce, 0x18, 0x6c, 0x4e, 0xc1, 0x96, 0xeb, 0x58,
    0x6d, 0x98, 0x8b, 0x52, 0xd5, 0x29, 0x93, 0x63, 0xb3, 0x1a, 0x90, 0x3d,
    0xbe, 0xbe, 0xda, 0x6f, 0x57, 0xa3, 0x3c, 0x72, 0x86, 0xfb, 0x58, 0xe0,
    0xd9, 0xd7, 0xd2, 0xc2, 0xa1, 0xec, 0x7c, 0x94, 0x0c, 0x37, 0x15, 0xf8,
    0xa3, 0xfa, 0xa4, 0xef, 0x95, 0xd3, 0x2a, 0x58, 0x96, 0x86, 0x67, 0xeb,
    0x2b, 0x6a, 0x0f, 0x58, 0x8f, 0x56, 0x3e, 0xc7, 0xd3, 0x58, 0xd6, 0x5f,
    0x2b, 0xb7, 0xaf, 0x6f, 0x4f, 0x84, 0x63, 0xc4, 0x73, 0xd4, 0x11, 0x7d,
    0x85, 0xae, 0x86, 0x01, 0xa7, 0x02, 0x65, 0xbd, 0x9e, 0xc7, 0x32, 0x9e,
    0x6e, 0xd8, 0x6a, 0xaa, 0x94, 0x6c, 0xfd, 0x3f, 0x34, 0xb7, 0x0c, 0x84,
    0xe0, 0x6d, 0x35, 0x26, 0xaa, 0x54, 0x94, 0xa6, 0x67, 0x1a, 0x2c, 0x3c,
    0x99, 0x15, 0xeb, 0xa5, 0x6d, 0x4b, 0xb9, 0xca, 0x94, 0x8b, 0xe8, 0x30,
    0x3b, 0xa4, 0x51, 0x0b, 0x89, 0x2b, 0x73, 0x21, 0x5e, 0x4b, 0x6e, 0x98,
    0x57, 0x30, 0x78, 0x20, 0x5a, 0x39, 0x40, 0xbd, 0x9e, 0xb0, 0xd5, 0x9f,
    0x9d, 0x6b, 0x55, 0x00, 0x46, 0xd6, 0xba, 0xc8, 0x84, 0xe0, 0x1b, 0xb8,
    0xf9, 0x07, 0x9d, 0x3f, 0x05, 0x4c, 0xd6, 0xf6, 0xb7, 0x8f, 0x5c, 0xbe,
    0xdf, 0x71, 0x20, 0x30, 0xe3, 0x9c, 0x83, 0x65, 0x34, 0x84, 0x11, 0x90,
    0x3f, 0xc7, 0xa5, 0x5b, 0xef, 0x01, 0xb7, 0x38, 0x34, 0x73, 0xa6, 0x5e,
    0x14, 0xb4, 0xc4, 0x24, 0xa8, 0x56, 0x74, 0x61, 0x75, 0xd5, 0x2b, 0xb9,
    0x94, 0xb0, 0xcf, 0xca, 0x30, 0xe9, 0x9b, 0x5c, 0xc8, 0xfe, 0x58, 0xd9,
    0x0c, 0x5a, 0x59, 0x64, 0x2b, 0x67, 0x99, 0x7b, 0x85, 0x3b, 0x56, 0x37,
    0x45, 0xa5, 0x1a, 0xb9, 0x0f, 0x09, 0x5e, 0xd9, 0x9a, 0x6c, 0xaa, 0xa7,
    0xf0, 0xd0, 0x19, 0xc5, 0xc4, 0xa8, 0xae, 0x59, 0xda, 0xa0, 0xc2, 0xa2,
    0xf3, 0x53, 0x9a, 0x1c, 0x88, 0x3d, 0xeb, 0x6b, 0x3f, 0x1f, 0x23, 0xa5,
    0xc8, 0x14, 0xf2, 0x42, 0x5e, 0x56, 0x6a, 0xea, 0xba, 0x07, 0x48, 0x51,
    0x27, 0xca, 0x1e, 0x19, 0x00, 0x79, 0x15, 0x59, 0xfd, 0x80, 0x67, 0xdc,
    0x78, 0x86, 0xc1, 0x70, 0x36, 0x0c, 0xe0, 0xee, 0x24, 0xfe, 0x0f, 0xf3,
};
// clang-format on

} // namespace Net