#include <algorithm>
#include <string> 
#include "ResourceFileMapper.h"
#include <Windows.h>

ResourceFileInfo GetResourceFileInfo(const std::wstring& searchPath, const std::wstring& baseName, const std::wstring& fileExt)
{
    ResourceFileInfo entry;
    std::wstring filePath = searchPath + L"/" + baseName + L"." + fileExt;

    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (GetFileAttributesExW(filePath.c_str(), GetFileExInfoStandard, &fileData) == 0)
    {
        // Failed to get attributes
        return entry;
    }

    if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        // Ignore directories
        return entry;
    }

    entry.done = true;
    entry.path = filePath;
    entry.extension = fileExt;
    entry.size = ((uint64_t)fileData.nFileSizeLow) | (((uint64_t)fileData.nFileSizeHigh) << 32);
    entry.timestamp = ((uint64_t)fileData.ftLastWriteTime.dwLowDateTime) | (((uint64_t)fileData.ftLastWriteTime.dwHighDateTime) << 32);

    return entry;
}

void ListResourceFilesFromDir(const std::wstring& searchPath, std::unordered_map<std::wstring, ResourceFileInfo>& outData)
{
    std::wstring searchPattern = searchPath + L"/*";

    HANDLE dir;
    WIN32_FIND_DATAW fileData;
    if ((dir = FindFirstFileW(searchPattern.c_str(), &fileData)) == INVALID_HANDLE_VALUE)
    {
        return; /* No files found */
    }

    do {
        if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

        std::wstring filePath = searchPath + L"/" + fileData.cFileName;
        std::wstring fileName = fileData.cFileName;
        std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::towlower);

        size_t sepIdx = fileName.find_last_of(L'.');
        if (sepIdx == std::wstring::npos)
        {
            continue;
        }
        std::wstring fileExt = fileName.substr(sepIdx + 1);

        // Limit file types handled (for now?)
        if ((fileExt != L"gif") && (fileExt != L"png"))
        {
            continue;
        }
        
        ResourceFileInfo entry;
        entry.done = true;
        entry.path = filePath;
        entry.extension = fileExt;
        entry.size = ((uint64_t)fileData.nFileSizeLow) | (((uint64_t)fileData.nFileSizeHigh) << 32);
        entry.timestamp = ((uint64_t)fileData.ftLastWriteTime.dwLowDateTime) | (((uint64_t)fileData.ftLastWriteTime.dwHighDateTime) << 32);

        outData[fileName] = entry;
    } while (FindNextFileW(dir, &fileData));

    FindClose(dir);
}