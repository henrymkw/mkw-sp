#include "ModelPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

BackGroundModelControl &ModelPage::modelControl() {
    return m_modelControl;
}

void ModelPage::SetModel(u32 model) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    assert(modelPage);

    modelPage->m_modelControl.setModel(model);
}

} // namespace UI
