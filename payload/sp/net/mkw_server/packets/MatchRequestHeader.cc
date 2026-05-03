#include "MatchRequestHeader.hh"

#include "sp/net/mkw_server/MKWServer.hh"

#include <string.h>
namespace MKWServer {
void createMatchRequestHeader(MatchRequestHeader *out, MatchRequestType type) {
    out->magic = MATCH_REQUEST_HEADER_MAGIC;
    out->type = type;
    out->searchId = getSearchId();
}
} // namespace MKWServer
