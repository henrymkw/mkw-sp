#include <revolution/dwc/DWCMain.h>

// currently just used to include all dwc headers to ease compilation

#include <revolution/dwc/DWCError.h>
#include <revolution/dwc/DWCFriend.h>
#include <revolution/dwc/DWCMatch.h>
#include <revolution/dwc/DWCTransport.h>

GT2Connection DWCi_GetGT2ConnectionByProfileID(s32 profileId) {
    GT2Connection conn = REPLACED(DWCi_GetGT2ConnectionByProfileID)(profileId);
    return conn;
}
