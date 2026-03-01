#include "MatchRequestHeader.h"

void createMatchRequestHeader(MatchRequestHeader *out, MatchRequestType type, u64 searchId) {
    out->magic = 0x4D524551; // "MREQ"
    out->type = type;
    out->searchId = searchId;
}
