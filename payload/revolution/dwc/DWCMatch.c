#include "DWCMatch.h"

#include <string.h>

BOOL DWCi_ProcessRecvMatchCommand(DWCMatchCommandType command, s32 profileId, u32 publicIp,
        u16 publicPort, void *data, s32 dataLen) {
    switch (command) {
    case DWC_MATCH_CMD_RESV:
        // copy commandData in a DWCResvCommand
        // DWCResvPacket *resvPacket = (DWCResvPacket *)data;
        SP_LOG("Received DWC_MATCH_CMD_RESV\n");
        break;
    case DWC_MATCH_CMD_RESV_OK:
        // DWCResvOkPacket *okPacket = (DWCResvOkPacket *)data;
        SP_LOG("Received DWC_MATCH_CMD_RESV_OK\n");
        break;
    case DWC_MATCH_CMD_RESV_DENY:
        // DWCResvDenyErrors *err = (DWCResvDenyErrors *)data;
        SP_LOG("Received DWC_MATCH_CMD_RESV_DENY\n");
        break;
    case DWC_MATCH_CMD_RESV_WAIT:
    case DWC_MATCH_CMD_RESV_CANCEL:
        SP_LOG("DWC_MATCH_CMD_RESV_WAIT or DWC_MATCH_CMD_RESV_CANCEL received");
        break;
    case DWC_MATCH_CMD_TELL_ADDR:
        // DWCTellAddrPacket *tellAddrPacket = (DWCTellAddrPacket *)data;
        SP_LOG("Received DWC_MATCH_CMD_TELL_ADDR\n");
        break;
    case DWC_MATCH_CMD_NEW_PID_AID:
        // DWCNewPIDAidPacket *newPidAidPacket = (DWCNewPIDAidPacket *)data;
        SP_LOG("Received DWC_MATCH_CMD_NEW_PID_AID\n");
        break;
    default:
        SP_LOG("Unknown command received: %d\n", command);
    }

    BOOL result = REPLACED(
            DWCi_ProcessRecvMatchCommand)(command, profileId, publicIp, publicPort, data, dataLen);
    return result;
}

s32 DWCi_SendMatchCommand(DWCMatchCommandType command, s32 profileId, u32 publicIp, u16 publicPort,
        void *commandData, s32 dataLen) {
    switch (command) {
    case DWC_MATCH_CMD_RESV:
        // copy commandData in a DWCResvCommand
        // DWCResvPacket *resvPacket = (DWCResvPacket *)commandData;
        SP_LOG("Sent DWC_MATCH_CMD_RESV\n");
        break;
    case DWC_MATCH_CMD_RESV_OK:
        // DWCResvOkPacket *okPacket = (DWCResvOkPacket *)commandData;
        SP_LOG("Sent DWC_MATCH_CMD_RESV_OK\n");
        break;
    case DWC_MATCH_CMD_RESV_DENY:
        // DWCResvDenyErrors *err = (DWCResvDenyErrors *)commandData;
        SP_LOG("Sent DWC_MATCH_CMD_RESV_DENY\n");
        break;
    case DWC_MATCH_CMD_RESV_WAIT:
    case DWC_MATCH_CMD_RESV_CANCEL:
        SP_LOG("DWC_MATCH_CMD_RESV_WAIT or DWC_MATCH_CMD_RESV_CANCEL received\n");
        break;
    case DWC_MATCH_CMD_TELL_ADDR:
        // DWCTellAddrPacket *tellAddrPacket = (DWCTellAddrPacket *)commandData;
        SP_LOG("Sent DWC_MATCH_CMD_TELL_ADDR\n");
        break;
    case DWC_MATCH_CMD_NEW_PID_AID:
        // DWCNewPIDAidPacket *newPidAidPacket = (DWCNewPIDAidPacket *)commandData;
        SP_LOG("Sent DWC_MATCH_CMD_NEW_PID_AID\n");
        break;
    default:
        SP_LOG("Unknown command received: %d\n", command);
    }

    s32 result = REPLACED(
            DWCi_SendMatchCommand)(command, profileId, publicIp, publicPort, commandData, dataLen);
    return result;
}

s32 DWCi_SendResvCommand(s32 profileId, s32 delay) {
    s32 result = REPLACED(DWCi_SendResvCommand)(profileId, delay);
    return result;
}