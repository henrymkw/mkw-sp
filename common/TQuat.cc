#include "TQuat.hh"

Quat::Quat() : TQuatBase{0.0f, 0.0f, 0.0f, 1.0f} {}

Quat::Quat(f32 x, f32 y, f32 z, f32 w) : TQuatBase{x, y, z, w} {}
