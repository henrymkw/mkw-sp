#pragma once

#include <Common.hh>

namespace EGG {

class SceneManager {
public:
    void changeSiblingScene(int sceneId);

private:
    u8 _00[0x04 - 0x00];

protected:
    void *m_creator;

private:
    u8 _08[0x2c - 0x08];
};
static_assert(sizeof(SceneManager) == 0x2c);

} // namespace EGG
