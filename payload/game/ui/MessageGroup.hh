#pragma once

#include <Common.hh>

namespace UI {

// From Pulsar, added for documentation purposes

enum class BMGEncodingFormat : u8 { Unknown = 0, CP1252 = 1, UTF16 = 2, SJIS = 3, UTF8 = 4 };

struct BMGHeader {
    char magic[8]; // "MESGbmg1"
    u32 fileLength;
    u32 numDataBlocks;
    BMGEncodingFormat encodingFormat; // 0 == unk, 1 = CP1252, 2 = UTF-16, 3 = SJIS, 4 = UTF-8
    u8 _1c[0x20 - 0x11];              // unk
};
static_assert(sizeof(BMGHeader) == 0x20);

struct DataBlockHeader {
    char magic[4];
    u32 size;
};
static_assert(sizeof(DataBlockHeader) == 0x8);

struct MessageEntry {
    u32 DAT1Offset;
    u8 font;
    u8 _5[0x8 - 0x5];
};
static_assert(sizeof(MessageEntry) == 0x8);

struct BMGInfo {
    u16 messageCount;
    u16 entrySize;
    u16 groupId;
    u8 defaultColor;
    u8 reserved;
    MessageEntry entries;
};

struct BMGInfoBlock {
    DataBlockHeader header;
    BMGInfo info;
};

struct BMGData {
    wchar_t stringPool;
};

struct BMGDataBlock {
    DataBlockHeader header;
    BMGData data;
};

struct BMGMessageIds {
    u16 msgCount;
    u8 format;
    u8 info;
    u8 reserved[4];
    u32 messageIds;
};

struct BMGMessageIdsBlock {
    DataBlockHeader header;
    BMGMessageIds messageIds;
};

class MessageGroup {
public:
    MessageGroup();
    void dt(s32 type);
    /**
     * @brief Uses ResourceManager::GetFile() to get a file
     */
    void load(const char *name); // 0x805f8b90


private:
    BMGHeader *m_header;
    BMGInfo *m_info;
    BMGData *m_data;
    u8 _0c[0x10 - 0xc];
    BMGMessageIds *m_messageIds;
};

} // namespace UI
