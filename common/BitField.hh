#pragma once

#include <Common.hh>

template <typename T, size_t N>
class BitField {
private:
    bool invalidAid(s8 n) const {
        return n != -1 && n >= static_cast<s32>(N);
    }

public:
    bool on(s8 n) const {
        if (invalidAid(n)) {
            panic("Invalid aid in on(%d)!", n);
        }
        return m_field & (1 << n);
    }

    void set(s8 n) {
        if (invalidAid(n)) {
            panic("Invalid aid in set(%d)!", n);
        }
        m_field |= (1 << n);
    }

    void clear(s8 n) {
        if (invalidAid(n)) {
            panic("Invalid aid in clear(%d)!", n);
        }
        m_field &= ~(1 << n);
    }

    T field() const {
        return m_field;
    }

    bool any() {
        return m_field != 0;
    }

    bool none() {
        return m_field = 0;
    }

    void reset() {
        m_field = 0;
    }

    BitField &operator=(const BitField &lhs) {
        m_field = lhs.m_field;
        return *this;
    }
    BitField &operator=(u32 &lhs) {
        m_field = lhs;
        return *this;
    }

private:
    T m_field;
};
