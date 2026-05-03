#include "User.hh"

namespace Net {

bool UserHandler::prepared() {
    return m_prepared;
}

u16 UserHandler::myVR() {
    return m_recordToSend.vr;
}

u16 UserHandler::myBR() {
    return m_recordToSend.br;
}

} // namespace Net
