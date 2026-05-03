#pragma once

#include <Common.hh>

#include <egg/core/eggExpHeap.hh>
#include <egg/core/eggTaskThread.hh>

#include "game/system/MultiDvdArchive.hh"

#include "game/util/Registry.hh"

namespace System {

class ResourceManager {
public:
    class CourseCache {
        friend class ResourceManager;

    public:
        enum class State {
            Cleared = 0,
            Loaded = 2,
        };

        // Remove CourseCache to save memory. Credits: Stebler
        REPLACE void init();
        REPLACE void load(u32 courseId);

    private:
        u8 _00[0x10 - 0x00];
        void *m_buffer;
        EGG::ExpHeap *m_heap;
        u32 m_course;
        State m_state;
        MultiDvdArchive *m_archive;
    };
    static_assert(sizeof(CourseCache) == 0x24);

    static ResourceManager *Instance();

    DvdArchive *getMenuArchive(size_t idx);
    REPLACE u16 getMenuArchiveCount() const;

    static const char *GetCourseFilename(Registry::Course course);
    static const char *CourseFilenames[67];

    void process();

    REPLACE MultiDvdArchive *loadCourse(Registry::Course courseId, EGG::Heap *heap,
            bool splitScreen);

    static ResourceManager *s_instance;

private:
    struct JobContext {
        MultiDvdArchive *multiArchive;
        DvdArchive *archive;

        u8 _08[0xc - 0x8];
        char filename[64];

        EGG::Heap *archiveHeap;
        EGG::Heap *fileHeap;
    };

    static void DoLoadTask(void *contextId);

    u8 _000[0x004 - 0x000];
    MultiDvdArchive **m_archives;
    u8 _008[0x338 - 0x008];
    JobContext m_jobContexts[7];
    EGG::TaskThread *m_taskThread;
    CourseCache m_courseCache;
    u8 _5ac[0x60c - 0x5ac];
    bool m_globeLoadingIsBusy;
    u8 _60d[0x614 - 0x60d];
    EGG::Heap *m_globeHeap;
    u8 _618[0x61c - 0x618];
};
static_assert(sizeof(ResourceManager) == 0x61c);

} // namespace System
