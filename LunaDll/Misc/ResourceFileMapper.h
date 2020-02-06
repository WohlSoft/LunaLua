#ifndef ResourceFileMapper_hhhh
#define ResourceFileMapper_hhhh


#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>

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

ResourceFileInfo GetResourceFileInfo(const std::wstring& filePath);
ResourceFileInfo GetResourceFileInfo(const std::wstring& searchPath, const std::wstring& baseName, const std::wstring& fileExt);
ResourceFileInfo GetResourceFileInfo(const std::wstring& filePath, const std::wstring& fileExt);
void ListResourceFilesFromDir(const std::wstring& searchPath, ResourceFileMap& outData);

// TODO: Allow use of std::string instead

#endif
