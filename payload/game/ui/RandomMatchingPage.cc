#include "RandomMatchingPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/FriendRoomBackPage.hh"
#include "game/ui/SectionManager.hh"

#include <sp/cs/RoomClient.hh>

namespace UI {

RandomMatchingPage::RandomMatchingPage() : m_handler(*this) {}
RandomMatchingPage::~RandomMatchingPage() = default;

PageId RandomMatchingPage::getReplacement() {
    return PageId::FriendMatching;
}

void RandomMatchingPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(1);

    insertChild(0, &m_title, 0);

    m_title.load(0);
}

void RandomMatchingPage::onActivate() {
    SP_LOG("RandonMatchingPage::onActivate");
    push(PageId::CharacterSelect, Anim::None);
}

void RandomMatchingPage::onRefocus() {
}

void RandomMatchingPage::afterCalc() {
    auto sectionManager = SectionManager::Instance();
    auto section = sectionManager->currentSection();
    if (!section->isPageFocused(this)) {
        return;
    }

    // Check if we found a match, if so, drive the RoomClient.
    auto roomClient = SP::RoomClient::Instance();
    if (roomClient) {
        assert(roomClient->calc(m_handler));
        return;
    }
}

RandomMatchingPage::Handler::Handler(RandomMatchingPage &page) : m_page(page) {}
RandomMatchingPage::Handler::~Handler() = default;

void RandomMatchingPage::Handler::onSelect() {
    m_page.changeSection(SectionId::Voting1PVS, Anim::Next, 0);
}

} // namespace UI
