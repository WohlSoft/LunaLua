#include <algorithm>
#include <string> 
#include "ResourceFileMapper.h"
#include <Windows.h>

ResourceFileInfo GetResourceFileInfo(const std::wstring& filePath)
{
    std::wstring fileExt = L"";
    std::wstring::size_type pathIdx1 = filePath.rfind(L'/');
    std::wstring::size_type pathIdx2 = filePath.rfind(L'\\');
    std::wstring::size_type extIdx = filePath.rfind(L'.');
    if (
        (extIdx != std::wstring::npos) &&
        ((pathIdx1 == std::wstring::npos) || (extIdx > pathIdx1)) &&
        ((pathIdx2 == std::wstring::npos) || (extIdx > pathIdx2))
       )
    {
        fileExt = filePath.substr(extIdx + 1);
    }

    return GetResourceFileInfo(filePath, fileExt);
}

ResourceFileInfo GetResourceFileInfo(const std::wstring& searchPath, const std::wstring& baseName, const std::wstring& fileExt)
{
    std::wstring filePath = searchPath + L"/" + baseName + L"." + fileExt;

    return GetResourceFileInfo(filePath, fileExt);
}

ResourceFileInfo GetResourceFileInfo(const std::wstring& filePath, const std::wstring& fileExt)
{
    ResourceFileInfo entry;

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

    // Lowercase extension
    std::transform(entry.extension.begin(), entry.extension.end(), entry.extension.begin(), towlower);

    return entry;
}

void ListResourceFilesFromDir(const std::wstring& searchPath, ResourceFileMap& outData)
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
        
        ResourceFileInfo entry;
        entry.done = true;
        entry.path = filePath;
        entry.extension = fileExt;
        entry.size = ((uint64_t)fileData.nFileSizeLow) | (((uint64_t)fileData.nFileSizeHigh) << 32);
        entry.timestamp = ((uint64_t)fileData.ftLastWriteTime.dwLowDateTime) | (((uint64_t)fileData.ftLastWriteTime.dwHighDateTime) << 32);

        // Lowercase extension
        std::transform(entry.extension.begin(), entry.extension.end(), entry.extension.begin(), towlower);

        outData[fileName] = entry;
    } while (FindNextFileW(dir, &fileData));

    FindClose(dir);
}