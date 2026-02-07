/*
 * Credits:
 * - CLF78: Midnight-Variety-Pack, licensed under the MPL license. See license in
 * licenses/Midnight-Variety-Pack.
 *
 */

#pragma once

#include <Common.h>

#include <revolution.h>
#include <revolution/gamespy/gt2/gt2Callback.h>
#include <revolution/gamespy/qr2/qr2.h>

typedef enum {
    DWC_MATCH_STATE_INIT,
    DWC_MATCH_STATE_CL_WAITING,
    DWC_MATCH_STATE_CL_SEARCH_HOST,
    DWC_MATCH_STATE_CL_WAIT_RESV,
    DWC_MATCH_STATE_CL_NN,
    DWC_MATCH_STATE_CL_GT2,
    DWC_MATCH_STATE_CL_CANCEL_SYN,
    DWC_MATCH_STATE_CL_SYN,
    DWC_MATCH_STATE_CL_SVDOWN_1,
    DWC_MATCH_STATE_CL_SVDOWN_2,
    DWC_MATCH_STATE_CL_SVDOWN_3,
    DWC_MATCH_STATE_CL_SEARCH_GROUPID_HOST,
    DWC_MATCH_STATE_SV_WAITING,
    DWC_MATCH_STATE_SV_OWN_NN,
    DWC_MATCH_STATE_SV_OWN_GT2,
    DWC_MATCH_STATE_SV_CANCEL_SYN,
    DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT,
    DWC_MATCH_STATE_SV_SYN,
    DWC_MATCH_STATE_SV_SYN_WAIT,
    DWC_MATCH_STATE_WAIT_CLOSE,
    DWC_MATCH_STATE_SEARCH_OWN,
    DWC_MATCH_STATE_COUNT,
} DWCMatchState;

typedef enum {
    DWC_MATCH_CMD_RESV = 0x1,
    DWC_MATCH_CMD_RESV_OK = 0x2,
    DWC_MATCH_CMD_RESV_DENY = 0x3,
    DWC_MATCH_CMD_RESV_WAIT = 0x4,
    DWC_MATCH_CMD_RESV_CANCEL = 0x5,
    DWC_MATCH_CMD_TELL_ADDR = 0x6,
    DWC_MATCH_CMD_NEW_PID_AID = 0x7,
    DWC_MATCH_CMD_LINK_CLOSE_REQ = 0x8,
    DWC_MATCH_CMD_LINK_CLOSE_SUC = 0x9,
    DWC_MATCH_CMD_CLOSE_LINK = 0xA,
    DWC_MATCH_CMD_RESV_PRIOR = 0xB,
    DWC_MATCH_CMD_CANCEL = 0xC,
    DWC_MATCH_CMD_CANCEL_SYN = 0xD,
    DWC_MATCH_CMD_CANCEL_SYN_ACK = 0xE,
    DWC_MATCH_CMD_CANCEL_ACK = 0xF,
    DWC_MATCH_CMD_SC_CLOSE_CL = 0x10,
    DWC_MATCH_CMD_POLL_TIMEOUT = 0x11,
    DWC_MATCH_CMD_POLL_TO_ACK = 0x12,
    DWC_MATCH_CMD_SC_CONN_BLOCK = 0x13,
    DWC_MATCH_CMD_SVDOWN_QUERY = 0x52,
    DWC_MATCH_CMD_SVDOWN_ACK = 0x53,
    DWC_MATCH_CMD_SVDOWN_NAK = 0x54,
    DWC_MATCH_CMD_SVDOWN_KEEP = 0x55,
    DWC_MATCH_CMD_SB_RETRY_SEARCH = 0x72,
    DWC_MATCH_CMD_SUSPEND_CTRL = 0x82,
    DWC_MATCH_CMD_USED_SLOTS = 0x83,
} DWCMatchCommandType;

typedef enum {
    DWC_RESV_DENY_ERROR_ROOM_FULL = 0x10,
    DWC_RESV_DENY_ERROR_ROOM_STARTED = 0x11,
    DWC_RESV_DENY_ERROR_ROOM_SUSPEND = 0x12,
} DWCResvDenyErrors;

typedef void (*DWCNewClientCallback)(int friendIndex, void *param);
typedef int (*DWCEvalCallback)(int serverBrowserCount, void *param);
typedef int (*DWCConnectAttemptCallback)(u8 *userData, void *param);
typedef void (*DWCMatchedCallback)(DWCError error, int cancel, int self, int isServer, int index,
        void *param);

typedef struct {
    u8 isQr2;
    u8 retryCount;
    u16 port;
    u32 ip;
    int cookie;
} DWCNNInfo;
static_assert(sizeof(DWCNNInfo) == 0xC);

typedef struct {
    u8 command;
    u8 count;
    u16 port;
    u32 ip;
    u32 data[32];
    int profileId;
    int len;
    OSTime sendTime;
} DWCMatchCommandControl;
static_assert(sizeof(DWCMatchCommandControl) == 0x98);

typedef struct {
    u8 matchType;
    u8 _01[0x04 - 0x01];
    u32 publicIp;
    u16 publicPort;
    u8 _0a[0x0c - 0x0a];
    u32 localIp;
    u16 localPort;
    u8 _12[0x14 - 0x12];
    u8 unk14;
    u8 _15[0x18 - 0x15];
    BOOL playersFriend;
    u8 numLocalPlayers;
    u8 _1d[0x20 - 0x1d];
    u32 resvCheckValue;
} DWCResvPacket;
static_assert(sizeof(DWCResvPacket) == 0x24);

typedef struct {
    u8 maxPlayers;
    u8 _01[0x04 - 0x01];
    u32 senderSlotIndex;
    u32 dwcPid;

    u32 publicIp;
    u16 publicPort;
    u8 _12[0x14 - 0x12];

    u32 localIp;
    u16 localPort;
    u8 _16[0x18 - 0x16];
    u8 unk1c;
    u8 _1d[0x20 - 0x1d];
    u8 numLocalPlayers;
    u8 _21[0x24 - 0x21];
    u32 dwcGroupId;

    u8 receiverSlotIndex;
    u8 _29[0x2c - 0x29];
    u8 consoleCountInRoom;
    u8 _2d[0x30 - 0x2d];
    u32 resvCheckValue;
} DWCResvOkPacket;
static_assert(sizeof(DWCResvOkPacket) == 0x34);

typedef struct {
    u32 localIp;
    u16 localPort; // might be weird due to little endian, so making it a u32
    u8 _06[0x08 - 0x06];
} DWCTellAddrPacket;
static_assert(sizeof(DWCTellAddrPacket) == 0x8);

typedef struct {
    u32 newPid;
    u32 newSlotId; // slot name comes from tockdom
} DWCNewPIDAidPacket;
static_assert(sizeof(DWCNewPIDAidPacket) == 0x8);

typedef struct {
    GT2Connection connection;
    GT2Socket socket;
    GT2Callbacks *gt2Callbacks;
    u8 gt2ConnectCount;
    u8 connectedOnceBefore;
    u8 _00e[0x010 - 0x00e];

    qr2 qrec;
    u8 qr2MatchType;
    u8 qr2MatchTypeExt;
    u8 hostState;
    u8 qr2NumEntry;
    u8 qr2IsReserved;
    u8 _019[0x01c - 0x019];

    u32 suspendMatchBitmap;
    OSTime suspendRequestTick;
    int suspendMatch;
    u8 suspend;
    u8 suspendServerResponse;
    bool isQR2ShutdownScheduled;
    u8 _02f[0x030 - 0x02f];

    DWCNodeInfoList nodeInfoList;
    DWCNodeInfo tempNewNodeInfo;
    DWCNodeInfo tempNewClientInfo;
    int inMeshMaking;
    DWCMatchState lastStateBeforeStopMeshMaking;
    DWCNodeInfo tempNodeInfoBeforeStopMeshMaking;

    u16 qr2Port;
    u8 _6d2[0x6d4 - 0x6d2];
    u32 qr2IP;
    int qr2Reservation;

    void *serverBrowser;
    int serverBrowserUpdateFlag;
    u8 _6e4[0x6e8 - 0x6e4];
    OSTime serverBrowserUpdateTick;
    OSTime serverBrowserUpdateRequestTick;

    u8 natnegRecvCount;
    u8 natnegFailureCount;
    u16 natnegCookieRand;
    int natnegLastCookie;
    s64 natnegFailedTime;
    s64 natnegFinishTime;
    DWCNNInfo natnegInfo;

    DWCMatchState state;
    u8 resvWaitCount;
    u8 closeState;
    u8 cancelState;
    u8 scResvRetryCount;
    u8 synResendCount;
    u8 cancelSynResendCount;
    u8 clWaitTimeoutCount;
    u8 _727[0x72a - 0x727];

    u16 baseLatency;
    u16 cancelBaseLatency;
    u8 _72e[0x730 - 0x72e];
    u32 natnegResvCheckValue;

    int cmdResendFlag;
    s64 cmdResendTick;
    u32 cmdTimeoutTime;
    u8 _744[0x748 - 0x744];
    s64 cmdTimeoutStartTick;

    u32 synAckBit;
    u32 cancelSynAckBit;
    s64 lastSynSent;
    s64 lastCancelSynSent;
    s64 closedTime;
    s64 clWaitTime;

    int profileId;
    u32 reqProfileId;
    int cbEventPid;

    const char *gameName;
    const char *secretKey;

    const DWCFriendData *friendList;
    int friendListLength;
    u8 friendIdxList[64];
    int friendIdxListLength;

    DWCMatchCommandControl cmdCnt;
    DWCMatchedCallback matchedCallback;
    void *matchedCallbackParam;
    DWCNewClientCallback newClientCallback;
    void *newClientCallbackParam;
    DWCEvalCallback evalCallback;
    void *evalCallbackParam;

    u32 serverDownBuff;
    u32 serverDownBase;
    u32 groupId;
    u8 searchGroupIdRetryHostCount;
    u8 connectGroupId;
    u8 serverDownQueryCount;
    u8 serverDownNakCount;
    OSTime serverPollingTime;

    u8 unk8a0;
    u8 unk8a1;
    u8 _8a2[0x8a4 - 0x8a2];

    DWCConnectAttemptCallback connectAttemptCallback;
    u8 connUserData[4];
    void *connectAttemptParam;

    u32 serverPollingUID;
    u8 _8b4[0x8b8 - 0x8b4];
    OSTime suspendWaitTime;
} DWCMatch;
static_assert(sizeof(DWCMatch) == 0x8c0);

BOOL DWCi_ProcessRecvMatchCommand(DWCMatchCommandType command, s32 profileId, u32 publicIp,
        u16 publicPort, void *data, s32 dataLen);

s32 DWCi_SendMatchCommand(DWCMatchCommandType command, s32 profileId, u32 publicIp, u16 publicPort,
        void *commandData, s32 dataLen);

s32 DWCi_SendResvCommand(s32 profileId, s32 delay);

extern DWCMatch *s_dwcMatch;
