#include "MatchRequestHeader.hh"

#include <string.h>
namespace MKWServer {
void createMatchRequestHeader(MatchRequestHeader *out, MatchRequestType type, u64 searchId) {
    memset(out, 0, sizeof(MatchRequestHeader));
    out->magic = MATCH_REQUEST_HEADER_MAGIC;
    out->type = type;
    out->searchId = searchId;
}
} // namespace MKWServer