#include <algorithm>
#include <string> 
#include "ResourceFileMapper.h"
#include "../GlobalFuncs.h"
#include <Windows.h>

static void ListResourceFilesFromDir(const std::wstring& searchPath, ResourceFileMap& outData)
{
    std::wstring searchPattern = GetWin32LongPath(searchPath) + L"\\*";

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
        std::wstring fileExt = (sepIdx != std::wstring::npos) ? fileName.substr(sepIdx + 1) : L"";
        
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
    for (auto& it : mSearchPaths)
    {
        FindCloseChangeNotification(it.second.changeHandle);
        it.second.changeHandle = INVALID_HANDLE_VALUE;
    }
}

void CachedFileMetadata::purge()
{
    // Nothing to do here now, we listen for filesystem metadata updates
}

const ResourceFileInfo CachedFileMetadata::getResourceFileInfo(const NormalizedPath<std::wstring>& filePath)
{
    std::wstring path = L"";
    std::wstring fileName = L"";
    std::wstring::size_type pathIdx = filePath.str().rfind(L'\\');
    if (pathIdx != std::wstring::npos)
    {
        path = filePath.str().substr(0, pathIdx);
        fileName = filePath.str().substr(pathIdx + 1);
    }
    else
    {
        fileName = filePath;
    }

    return getResourceFileInfo(path, fileName);
}

const ResourceFileInfo CachedFileMetadata::getResourceFileInfo(const std::wstring& path, const std::wstring& file)
{
    std::wstring lpath = path;
    std::wstring lfile = file;
    std::transform(lpath.begin(), lpath.end(), lpath.begin(), ::towlower);
    std::transform(lfile.begin(), lfile.end(), lfile.begin(), ::towlower);
    std::unique_lock<std::mutex> lck(mMutex);

    auto&& it = mSearchPaths.find(lpath);

    if (it == mSearchPaths.end())
    {
        // Don't have this search path searched yet, so search it
        // Get change notification handle
        HANDLE changeHandle = FindFirstChangeNotificationW(path.c_str(), FALSE, (
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE
            ));
        if (changeHandle == INVALID_HANDLE_VALUE)
        {
            // Couldn't get change handle
            return emptyFileInfo;
        }

        auto emplaceRet = mSearchPaths.emplace(lpath, changeHandle);
        if (!emplaceRet.second)
        {
            // Couldn't insert
            FindCloseChangeNotification(changeHandle);
            return emptyFileInfo;
        }
        it = emplaceRet.first;
        ListResourceFilesFromDir(path, it->second.map);
    }
    else
    {
        // Check if there's an update in the directory?
        if (WaitForSingleObject(it->second.changeHandle, 0) == WAIT_OBJECT_0)
        {
            FindNextChangeNotification(it->second.changeHandle);
            it->second.map.clear();
            ListResourceFilesFromDir(path, it->second.map);
        }
    }

    const ResourceFileMap& fileMap = it->second.map;
    auto&& fileIt = fileMap.find(lfile);

    if (fileIt == fileMap.end())
    {
        // File not found
        return emptyFileInfo;
    }

    return fileIt->second;
}

ResourceFileMap CachedFileMetadata::listResourceFilesFromDir(const NormalizedPath<std::wstring>& _path)
{
    std::wstring path = _path;
    while ((path.length() > 0) && (path[path.length() - 1] == L'\\'))
    {
        path.resize(path.length() - 1);
    }
    std::wstring lpath = path;
    std::transform(lpath.begin(), lpath.end(), lpath.begin(), ::towlower);
    std::unique_lock<std::mutex> lck(mMutex);

    auto&& it = mSearchPaths.find(lpath);

    if (it == mSearchPaths.end())
    {
        // Don't have this search path searched yet, so search it
        // Get change notification handle
        HANDLE changeHandle = FindFirstChangeNotificationW(path.c_str(), FALSE, (
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE
            ));
        if (changeHandle == INVALID_HANDLE_VALUE)
        {
            // Couldn't get change handle
            return std::move(ResourceFileMap());;
        }

        auto emplaceRet = mSearchPaths.emplace(lpath, changeHandle);
        if (!emplaceRet.second)
        {
            // Couldn't insert
            FindCloseChangeNotification(changeHandle);
            return std::move(ResourceFileMap());
        }
        it = emplaceRet.first;
        ListResourceFilesFromDir(path, it->second.map);
    }
    else
    {
        // Check if there's an update in the directory?
        if (WaitForSingleObject(it->second.changeHandle, 0) == WAIT_OBJECT_0)
        {
            FindNextChangeNotification(it->second.changeHandle);
            it->second.map.clear();
            ListResourceFilesFromDir(path, it->second.map);
        }
    }

    return it->second.map;
}

bool CachedFileMetadata::checkUpdateFile(const NormalizedPath<std::wstring>& filePath, ResourceFileInfo& fileInfo)
{
    const ResourceFileInfo newInfo = getResourceFileInfo(filePath);

    if (newInfo != fileInfo)
    {
        fileInfo = newInfo;
        return true;
    }

    return false;
}

bool CachedFileMetadata::exists(const NormalizedPath<std::wstring>& filePath)
{
    const ResourceFileInfo newInfo = getResourceFileInfo(filePath);
    return newInfo.done;
}

//=============================================================================

