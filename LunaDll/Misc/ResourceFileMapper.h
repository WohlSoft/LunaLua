#ifndef ResourceFileMapper_hhhh
#define ResourceFileMapper_hhhh


#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

class ResourceFileInfo {
public:
    bool         done;
    std::wstring path;
    std::wstring extension;
    uint64_t     timestamp;
    uint64_t     size;

    ResourceFileInfo() :
        done(false), path(), extension(), timestamp(0), size(0)
    {}
};

static inline bool operator==(const ResourceFileInfo& lhs, const ResourceFileInfo& rhs)
{
    return (
        (lhs.size == rhs.size) &&
        (lhs.timestamp == rhs.timestamp) &&
        (lhs.done == rhs.done) &&
        (lhs.path == rhs.path)
    );
}

static inline bool operator!=(const ResourceFileInfo& lhs, const ResourceFileInfo& rhs)
{
    return !(lhs == rhs);
}

typedef std::unordered_map<std::wstring, ResourceFileInfo> ResourceFileMap;

class CachedFileMetadata
{
private:
    std::mutex mMutex;
    std::unordered_map<std::wstring, ResourceFileMap> mSearchPaths;
public:
    CachedFileMetadata();
    ~CachedFileMetadata();
    void purge();
    const ResourceFileInfo getResourceFileInfo(const std::wstring& filePath);
    const ResourceFileInfo getResourceFileInfo(const std::wstring& path, const std::wstring& fileName);
    ResourceFileMap listResourceFilesFromDir(const std::wstring& searchPath);
    bool checkUpdateFile(const std::wstring& filePath, ResourceFileInfo& fileInfo);
    bool exists(const std::wstring& filePath);
};

// Global instance
extern CachedFileMetadata gCachedFileMetadata;

template <typename T>
class CachedFileDataWeakPtr
{
public:
    struct Entry
    {
        ResourceFileInfo metadata;
        std::weak_ptr<T> data;
        bool used;
        Entry(const ResourceFileInfo& fileInfo) :
            metadata(fileInfo), data(), used(true)
        {
        }
    };
private:
    std::unordered_map<std::wstring, Entry> mCache;
    std::vector<std::shared_ptr<T>> mCacheHolder;
    std::vector<std::shared_ptr<T>> mWorldCacheHolder;
public:
    CachedFileDataWeakPtr::CachedFileDataWeakPtr() :
        mCache(), mCacheHolder()
    {
    }

    CachedFileDataWeakPtr::~CachedFileDataWeakPtr()
    {
    }

    void CachedFileDataWeakPtr::hold(bool isWorld)
    {
        std::vector<std::shared_ptr<T>>* pCacheHolder = isWorld ? &mWorldCacheHolder : &mCacheHolder;
        pCacheHolder->clear();
        for (auto& cacheEntry = mCache.begin(); cacheEntry != mCache.end();)
        {
            std::shared_ptr<T> cachePtr = cacheEntry->second.data.lock();
            if (cachePtr)
            {
                if (cacheEntry->second.used)
                {
                    pCacheHolder->push_back(std::move(cachePtr));
                }
                cacheEntry->second.used = false;
                cacheEntry++;
            }
            else
            {
                mCache.erase(cacheEntry++);
            }
        }
    }

    void CachedFileDataWeakPtr::release(bool isWorld)
    {
        std::vector<std::shared_ptr<T>>* pCacheHolder = isWorld ? &mWorldCacheHolder : &mCacheHolder;
        pCacheHolder->clear();
        for (auto& cacheEntry = mCache.begin(); cacheEntry != mCache.end();)
        {
            std::shared_ptr<T> cachePtr = cacheEntry->second.data.lock();
            if (!cachePtr)
            {
                mCache.erase(cacheEntry++);
            }
            else
            {
                cacheEntry++;
            }
        }
    }

    Entry* CachedFileDataWeakPtr::get(const std::wstring& filePath)
    {
        std::wstring lpath = filePath;
        std::transform(lpath.begin(), lpath.end(), lpath.begin(), ::towlower);

        auto& it = mCache.find(lpath);

        if (it == mCache.end())
        {
            // Not in cache, see if we can find the file
            const ResourceFileInfo& fileInfo = gCachedFileMetadata.getResourceFileInfo(filePath);

            // No file, can't do this
            if (!fileInfo.done)
            {
                return nullptr;
            }

            // Add an entry
            auto emplaceRet = mCache.emplace(lpath, fileInfo);
            if (!emplaceRet.second)
            {
                return nullptr;
            }
            it = emplaceRet.first;
            return &it->second;
        }
        else
        {
            // Mark as used
            it->second.used = true;

            // In cache, check if it's up to date
            bool needUpdate = gCachedFileMetadata.checkUpdateFile(filePath, it->second.metadata);

            // If there needs to be an update, clear our weak pointer
            if (needUpdate)
            {
                it->second.data.reset();
            }

            // If no file actually exists, don't return the entry
            if (!it->second.metadata.done)
            {
                return nullptr;
            }

            return &it->second;
        }
    }
};

// TODO: Allow use of std::string instead

#endif
