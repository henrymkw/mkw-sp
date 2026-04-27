#pragma once

#include <Common.hh>

extern "C" {
#include <revolutionex/nhttp.h>
#include <vendor/rsa/rsa.h>
#include <vendor/sha256/sha256.h>
}
namespace MKWServer {
// clang-format off
#ifdef LOCAL_MKW_SERVER
    #define WWFC_DOMAIN "nwfc.wiinoma.com" // nwfc.wiinoma.com points to localhost
    #define WFC_SERVER_ADDRESS 0x7f000001
#elif TEST_MKW_SERVER
    #define WWFC_DOMAIN "mkw-cs.xyz"
    #define WFC_SERVER_ADDRESS 0x327438d3
#else
    #define WWFC_DOMAIN "mkw-server.xyz"
    #define WFC_SERVER_ADDRESS 0x607e6b90 // mkw-server.xyz
#endif

static const RSAPublicKey PayloadPublicKey = {
    #ifdef LOCAL_MKW_SERVER
        #include <TESTIncludeRSAPublicPayloadKey.txt>
    #elif TEST_MKW_SERVER
        #include <TESTServerIncludeRSAPublicPayloadKey.txt>
    #else
        #include <PRODIncludeRSAPublicPayloadKey.txt>
    #endif
};
// clang-format on

#define PAYLOAD_BLOCK_SIZE 0x20000

bool GenerateRandomSalt(u8 *out);

s32 HandleResponse();

void OnPayloadReceived(NHTTPError result, NHTTPResponseHandle response, void *userdata);

struct WWFCHeader {
    char magic[0xC]; // Always "WWFC/Payload"
    u32 total_size;
    u8 signature[0x100]; // RSA-2048 signature
} __attribute__((packed));

struct WWFCPayloadInfo {
    u32 format_version;        // Payload format version
    u32 format_version_compat; // Minimum payload format version that this
                               // payload is compatible with
    char name[0xC];            // Payload name (e.g. "RMCPD00")
    u32 version;               // Payload version
    u32 got_start;
    u32 got_end;
    u32 fixup_start;
    u32 fixup_end;
    u32 patch_list_offset;
    u32 patch_list_end;
    u32 entry_point;
    u32 entry_point_no_got;
    u32 reserved[0x18 / 4];
    char build_timestamp[0x20];
} __attribute__((packed));

struct WWFCPayload {
    WWFCHeader header;
    u8 salt[SHA256_DIGEST_SIZE];
    WWFCPayloadInfo info;
} __attribute__((packed));

enum class WWFCPatchType : u8 {
    /**
     * Copy bytes specified in `args` to the destination `address`.
     * @param arg0 Pointer to the data to copy from.
     * @param arg1 Length of the data.
     */
    Write = 0,

    /**
     * Write a branch: `address` = b `arg0`;
     * @param arg0 Branch destination address.
     * @param arg1 Not used.
     */
    Branch = 1,

    /**
     * Write a branch with a branch back: `address` = b `arg0`; `arg1` = b
     * `address` + 4;
     * @param arg0 Branch destination address.
     * @param arg1 Address to write the branch back.
     */
    BranchHook = 2,

    /**
     * Write a branch with link: `address` = bl `arg0`
     * @param arg0 Branch destination address.
     * @param arg1 Not used.
     */
    Call = 3,

    /**
     * Write a branch using the count register:
     * `address` = \
     * lis `arg1`, `arg0`\@h; \
     * ori `arg1`, `arg1`, `arg0`\@l; \
     * mtctr `arg1`; \
     * bctr;
     * @param arg0 Branch destination address.
     * @param arg1 Temporary register to use for call.
     */
    BranchCTR = 4,

    /**
     * Write a branch with link using the count register:
     * `address` = \
     * lis `arg1`, `arg0`\@h; \
     * ori `arg1`, `arg1`, `arg0`\@l; \
     * mtctr `arg1`; \
     * bctrl;
     * @param arg0 Branch destination address.
     * @param arg1 Temporary register to use for call.
     */
    BranchCTRLink = 5,
};

// Flags for different patch levels.
enum class WWFCPatchLevel : u8 {

    /**
     * Critical, used for security patches and other things required to connect
     * to the server. This has no value and is always automatically applied.
     */
    Critical = 0, // 0x00

    /**
     * Patches that fix bugs in the game, such as anti-freeze patches.
     */
    Bugfix = 1 << 0, // 0x01

    /**
     * Patches required for parity with clients using a regular WWFC patcher.
     */
    Parity = 1 << 1, // 0x02

    /**
     * Additional feature, not required to be compatible with regular clients.
     */
    Feature = 1 << 2, // 0x04

    /**
     * General support patches that may be redundant depending on the patcher.
     * Used in cases like URL patches.
     */
    Support = 1 << 3, // 0x08

    /**
     * Flag used to disable the patch if it has been already applied.
     */
    Disabled = 1 << 4, // 0x10
};

WWFCPatchLevel operator|(WWFCPatchLevel a, WWFCPatchLevel b);
WWFCPatchLevel &operator|=(WWFCPatchLevel &a, WWFCPatchLevel b);
WWFCPatchLevel operator&(WWFCPatchLevel a, WWFCPatchLevel b);

struct WWFCPatch {
    WWFCPatchLevel level;
    WWFCPatchType type;
    u8 reserved[2];
    u32 address;

    union {
        u32 arg0;
        const void *arg0p;
        const u32 *arg0p32;
    };

    u32 arg1;
} __attribute__((packed));

#define WL_ERROR_PAYLOAD_OK 0
#define WL_ERROR_PAYLOAD_STAGE0_MISSING_STAGE1 -20901
#define WL_ERROR_PAYLOAD_STAGE0_HASH_MISMATCH -20902
#define WL_ERROR_PAYLOAD_STAGE1_ALLOC -20910
#define WL_ERROR_PAYLOAD_STAGE1_MAKE_REQUEST -20911
#define WL_ERROR_PAYLOAD_STAGE1_RESPONSE -20912
#define WL_ERROR_PAYLOAD_STAGE1_HEADER_CHECK -20913
#define WL_ERROR_PAYLOAD_STAGE1_LENGTH_ERROR -20914
#define WL_ERROR_PAYLOAD_STAGE1_SALT_MISMATCH -20915
#define WL_ERROR_PAYLOAD_STAGE1_GAME_ID_MISMATCH -20916
#define WL_ERROR_PAYLOAD_STAGE1_SIGNATURE_INVALID -20917
#define WL_ERROR_PAYLOAD_STAGE1_WAITING -20918
#define WL_ERROR_PAYLOAD_GAME_MISMATCH -20930
} // namespace MKWServer