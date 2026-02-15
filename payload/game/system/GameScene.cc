#include "GameScene.hh"

u32 g_frameCount = 0;

namespace System {

void GameScene::calc() {
    g_frameCount++;
    REPLACED(calc)();
}

} // namespace System