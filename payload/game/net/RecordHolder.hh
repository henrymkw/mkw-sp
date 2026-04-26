#pragma once

#include <Common.hh>

#include <cstring>

namespace Net {

template <typename T>
class RecordHolder {
public:
    void reset() {
        memset(m_record, 0, m_bufferSize);
        m_recordSize = 0;
    }

    void copy(void *src, u32 len) {
        memcpy(m_record, src, len);
        m_recordSize = len;
    }

    void append(void *src, u32 len) {
        memcpy(reinterpret_cast<u8 *>(m_record) + m_recordSize, src, len);
        m_recordSize += len;
    }

    T *record() {
        return m_record;
    }

    u32 recordSize() const {
        return m_recordSize;
    }

private:
    T *m_record;
    u32 m_bufferSize;
    u32 m_recordSize;
};

} // namespace Net
