#include "MatchRequestHeader.h"

#include <string.h>

void createMatchRequestHeader(MatchRequestHeader *out, MatchRequestType type, u64 searchId) {
    memset(out, 0, sizeof(MatchRequestHeader));
    out->magic = 0x4D524551; // "MREQ"
    out->type = type;
    out->searchId = searchId;
}
