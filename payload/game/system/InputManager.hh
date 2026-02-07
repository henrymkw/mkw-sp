#pragma once

#include <Common.hh>

extern "C" {
#include <revolution.h>
}

#include "game/util/Registry.hh"

namespace System {

namespace Button {

enum {
    Accel = 1 << 0,
    Brake = 1 << 1,
    Item = 1 << 2,
    Drift = 1 << 3,
    BrakeDrift = 1 << 4, // Added
    LookBackwards = 1 << 5,
};

} // namespace Button

namespace Trick {

enum {
    Off,
    Up,
    Down,
    Left,
    Right,
};

} // namespace Trick

struct RaceInputState {
    RaceInputState();
    virtual ~RaceInputState();
    virtual void dt(s32 type);

    u16 _04 : 10;
    bool lookBackwards : 1;
    bool brakeDrift : 1;
    bool drift : 1;
    bool item : 1;
    bool brake : 1;
    bool accelerate : 1;
    u16 rawButtons;
    Vec2<f32> stick;
    Vec2<u8> rawStick;
    u8 trick;
    u8 rawTrick;
    bool isValid : 1;
};
static_assert(sizeof(RaceInputState) == 0x18);

struct UIInputState {
    u8 _00[0x08 - 0x00];
    Vec2<f32> stick;
    u8 _10[0x34 - 0x10];
};
static_assert(sizeof(UIInputState) == 0x34);

class Pad {
public:
    Pad();
    virtual ~Pad();
    virtual void dt(s32 type);

protected:
    virtual void process(RaceInputState &raceInputState, UIInputState &uiInputState) = 0;

public:
    virtual Registry::Controller getControllerId() const;
    virtual void vf_14();
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();
    virtual void vf_24();
    virtual void vf_28();
    virtual void vf_2c();
    virtual void vf_30();
    virtual void vf_34();
    virtual void vf_38();
    virtual void vf_3c();
    virtual void vf_40();
    virtual void vf_44();

protected:
    void processSimplified(RaceInputState &raceInputState, bool isPressed);

public:
    void calc();

private:
    RaceInputState m_currentRaceInputState;
    UIInputState m_currentUIInputState;
    u8 _50[0x90 - 0x50];
};
static_assert(sizeof(Pad) == 0x90);

class PadProxy {
public:
    PadProxy();
    virtual ~PadProxy();
    virtual void dt(s32 type);
    virtual void calc(bool isPaused);
    virtual void reset();
    virtual void setRaceInputState(const RaceInputState &inputState);
    virtual void vf_18();
    virtual void vf_1c();
    virtual void vf_20();

    const Pad *pad() const;
    const RaceInputState &currentRaceInputState() const;
    void setPad(Pad *pad, Pad *r5);

private:
    Pad *m_pad;
    u8 _08[0x28 - 0x08];
    RaceInputState m_currentRaceInputState;
    RaceInputState m_lastRaceInputState;
    u8 _58[0xd8 - 0x58];
};
static_assert(sizeof(PadProxy) == 0xd8);

} // namespace System
