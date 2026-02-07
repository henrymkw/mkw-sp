#include "ResourceManager.hh"

#include <sp/storage/Storage.hh>

#include <cstdio>
#include <cstring>

namespace System {

ResourceManager *ResourceManager::Instance() {
    return s_instance;
}

DvdArchive *ResourceManager::getMenuArchive(size_t idx) {
    MultiDvdArchive *archive = m_archives[static_cast<size_t>(ResourceType::Menu)];
    u16 loadedCount = 0;
    for (u16 i = 0; i < archive->count(); i++) {
        DvdArchive &candidate = archive->archive(i);
        if (candidate.state() == DvdArchive::State::Mounted) {
            if (loadedCount == idx) {
                return &candidate;
            }
            loadedCount++;
        }
    }
    return nullptr;
}

u16 ResourceManager::getMenuArchiveCount() const {
    MultiDvdArchive *archive = m_archives[static_cast<size_t>(ResourceType::Menu)];
    u16 loadedCount = 0;
    for (u16 i = 0; i < archive->count(); i++) {
        if (archive->archive(i).state() == DvdArchive::State::Mounted) {
            loadedCount++;
        }
    }
    return loadedCount;
}

const char *ResourceManager::GetCourseFilename(Registry::Course course) {
    u32 courseId = static_cast<u32>(course);

    assert(courseId < std::size(CourseFilenames));
    return CourseFilenames[courseId];
}

MultiDvdArchive *ResourceManager::loadCourse(Registry::Course courseId, EGG::Heap *heap,
        bool splitScreen) {
    MultiDvdArchive *archive = m_archives[static_cast<size_t>(ResourceType::Course)];
    if (archive->isLoaded()) {
        return archive;
    }

    archive->init();

    JobContext *jobContext = &m_jobContexts[2];
    jobContext->multiArchive = archive;
    jobContext->archiveHeap = heap;

    auto *filePath = jobContext->filename;
    auto filePathSize = sizeof(jobContext->filename);
    assert(static_cast<u32>(courseId) < std::size(CourseFilenames));
    auto *courseFilename = CourseFilenames[static_cast<u32>(courseId)];

    if (splitScreen) {
        snprintf(filePath, filePathSize, "Race/Course/%s_d", courseFilename);
        if (!archive->exists(filePath)) {
            splitScreen = false;
        }
    }

    if (!splitScreen) {
        snprintf(filePath, filePathSize, "Race/Course/%s", courseFilename);
    }

    m_taskThread->request(DoLoadTask, (void *)2, 0);
    process();

    assert(archive->isLoaded());
    return archive;
}

} // namespace System
