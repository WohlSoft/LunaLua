#include <algorithm>
#include <string> 
#include "ResourceFileMapper.h"
#include <Windows.h>

static void ListResourceFilesFromDir(const std::wstring& searchPath, ResourceFileMap& outData)
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

        std::wstring fileName = fileData.cFileName;
        std::wstring filePath = searchPath + L"/" + fileName;
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
        std::transform(entry.extension.begin(), entry.extension.end(), entry.extension.begin(), ::towlower);

        outData[fileName] = entry;
    } while (FindNextFileW(dir, &fileData));

    FindClose(dir);
}

// Global instance
CachedFileMetadata gCachedFileMetadata;
const ResourceFileInfo emptyFileInfo;

CachedFileMetadata::CachedFileMetadata() :
    mSearchPaths()
{
}

CachedFileMetadata::~CachedFileMetadata()
{
}

void CachedFileMetadata::purge()
{
    std::unique_lock<std::mutex> lck(mMutex);
    mSearchPaths.clear();
}

const ResourceFileInfo CachedFileMetadata::getResourceFileInfo(const std::wstring& filePath)
{
    std::wstring path = L"";
    std::wstring fileName = L"";
    std::wstring::size_type pathIdx = filePath.find_last_of(L"/\\");
    if (pathIdx != std::wstring::npos)
    {
        path = filePath.substr(0, pathIdx);
        fileName = filePath.substr(pathIdx + 1);
    }
    else
    {
        fileName = filePath;
    }

    return getResourceFileInfo(path, fileName);
}

const ResourceFileInfo CachedFileMetadata::getResourceFileInfo(const std::wstring& path, const std::wstring& fileName)
{
    std::wstring lpath = path;
    std::wstring lfile = fileName;
    std::transform(lpath.begin(), lpath.end(), lpath.begin(), ::towlower);
    std::transform(lfile.begin(), lfile.end(), lfile.begin(), ::towlower);
    std::unique_lock<std::mutex> lck(mMutex);

    auto& it = mSearchPaths.find(lpath);

    if (it == mSearchPaths.end())
    {
        // Don't have this search path searched yet, so search it
        auto emplaceRet = mSearchPaths.emplace(lpath, ResourceFileMap());
        if (!emplaceRet.second)
        {
            // Couldn't insert
            return emptyFileInfo;
        }
        it = emplaceRet.first;
        ListResourceFilesFromDir(path, it->second);
    }

    const ResourceFileMap& fileMap = it->second;
    auto& fileIt = fileMap.find(lfile);

    if (fileIt == fileMap.end())
    {
        // File not found
        return emptyFileInfo;
    }

    return fileIt->second;
}

ResourceFileMap CachedFileMetadata::listResourceFilesFromDir(const std::wstring& searchPath)
{
    std::wstring lpath = searchPath;
    std::transform(lpath.begin(), lpath.end(), lpath.begin(), ::towlower);
    std::unique_lock<std::mutex> lck(mMutex);

    auto& it = mSearchPaths.find(lpath);

    if (it == mSearchPaths.end())
    {
        // Don't have this search path searched yet, so search it
        auto emplaceRet = mSearchPaths.emplace(lpath, ResourceFileMap());
        if (!emplaceRet.second)
        {
            // Couldn't insert
            return std::move(ResourceFileMap());
        }
        it = emplaceRet.first;
        ListResourceFilesFromDir(searchPath, it->second);
    }

    return it->second;
}

bool CachedFileMetadata::checkUpdateFile(const std::wstring& filePath, ResourceFileInfo& fileInfo)
{
    const ResourceFileInfo newInfo = getResourceFileInfo(filePath);

    if (newInfo != fileInfo)
    {
        fileInfo = newInfo;
        return true;
    }

    return false;
}

bool CachedFileMetadata::exists(const std::wstring& filePath)
{
    const ResourceFileInfo newInfo = getResourceFileInfo(filePath);
    return newInfo.done;
}

//=============================================================================

