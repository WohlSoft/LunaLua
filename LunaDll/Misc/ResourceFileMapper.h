#ifndef ResourceFileMapper_hhhh
#define ResourceFileMapper_hhhh


#include <cstdint>
#include <string>

class ResourceFileInfo {
public:
    std::wstring path;
    uint64_t     timestamp;
    uint64_t     size;

    ResourceFileInfo() :
        path(), timestamp(0), size(0)
    {}
};

static inline bool operator==(const ResourceFileInfo& lhs, const ResourceFileInfo& rhs)
{
    return (
        (lhs.size == rhs.size) &&
        (lhs.timestamp == rhs.timestamp) &&
        (lhs.path == rhs.path)
    );
}

static inline bool operator!=(const ResourceFileInfo& lhs, const ResourceFileInfo& rhs)
{
    return !(lhs == rhs);
}

// Fills an array of ResourceFileInfo based on files found matching the specified pattern.
// Skips anything already filled out...
void FillResourceFileInfo(const wchar_t* pathHead, const wchar_t* pathTail, uint16_t firstIdx, uint16_t lastIdx, ResourceFileInfo* outData);

#endif
