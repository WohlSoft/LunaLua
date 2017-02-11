#include "ResourceFileMapper.h"
#include <Windows.h>

void FillResourceFileInfo(const wchar_t* pathHead, const wchar_t* pathTail, uint16_t maxId, ResourceFileInfo* outData)
{
    // TODO: Check if using FindFirstFile/FindNextFile would yield better performance?

    WIN32_FILE_ATTRIBUTE_DATA fileData;
    for (uint16_t id = 1; id <= maxId; id++) {
        if (outData[id - 1].path.length() > 0) continue;

        std::wstring path = pathHead + std::to_wstring(id) + pathTail;
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fileData) == 0)
        {
            // Failed to get attributes
            continue;
        }

        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Ignore directories
            continue;
        }

        outData[id - 1].path = path;
        outData[id - 1].size = ((uint64_t)fileData.nFileSizeLow) | (((uint64_t)fileData.nFileSizeHigh) << 32);
        outData[id - 1].timestamp = ((uint64_t)fileData.ftLastWriteTime.dwLowDateTime) | (((uint64_t)fileData.ftLastWriteTime.dwHighDateTime) << 32);
    }
}