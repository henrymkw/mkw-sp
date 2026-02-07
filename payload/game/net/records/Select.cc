#include "Select.hh"

namespace Net {

SelectHandler *SelectHandler::CreateInstance() {
    s_instance = REPLACED(CreateInstance)();
    return s_instance;
}

} // namespace Net