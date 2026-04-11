#pragma once

#include <Common.hh>

template <size_t N>
class BitField {
public:
    bool on(size_t n) const {
        return m_field & (1 << n);
    }

    void set(size_t n) {
        m_field |= (1 << n);
    }

    void clear(size_t n) {
        m_field &= ~(1 << n);
    }

    u32 field() const {
        return m_field;
    }

private:
    u32 m_field;
};
