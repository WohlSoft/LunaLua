#include <cerrno>
#include <fcntl.h>
#include <random>
#include <stdio.h>
#include <string>
#include <windows.h>
#include <WinBase.h>
#include <winternl.h>
#include <ntstatus.h>
#include <io.h>
#include <unordered_set>
#include "LunaPathValidator.h"
#include "../Globals.h"
#include "../Misc/LoadScreen.h"
#include "../Misc/FileUtils.h"

#include "../Misc/Syscalls.h"

// I know this is incredibly cursed, but this is the only way I can think of to safely list the files of a directory
// Huge props to https://blog.s-schoener.com/2024-06-24-find-files-internals/ for these definitions
typedef struct _FILE_DIRECTORY_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG FileAttributes;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_DIRECTORY_INFORMATION;

// Taken from https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_rename_information
typedef struct _FILE_RENAME_INFORMATION {
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10_RS1)
    union {
        BOOLEAN ReplaceIfExists;  // FileRenameInformation
        ULONG Flags;              // FileRenameInformationEx
    } DUMMYUNIONNAME;
#else
    BOOLEAN ReplaceIfExists;
#endif
    HANDLE RootDirectory;
    ULONG FileNameLength;
    WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION;

// Construct directory handle from path
static RAIIHandle getDirectoryHandle(wchar_t const* path, DWORD permissions, DWORD& errorCode) {
    // Create directory handle
    RAIIHandle directoryHandle = CreateFileW(
        path, // Folder path
        permissions,
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, // We're allowing other processes to do anything to the folder
        nullptr, // This handle can't be inherited by child processes
        OPEN_EXISTING, // We don't want to create a new folder
        FILE_FLAG_BACKUP_SEMANTICS, // Neccesary to create a directory handle
        nullptr // Useless for opening existing folders 
    );

    // Check if the directory handle was successfully created
    if (!directoryHandle.isValid()) {
        errorCode =  GetLastError();
    }

    return directoryHandle;
}

// Get the path of a file or directory handle after symlink resolution
static DWORD getHandleFinalPath(HANDLE handle, std::wstring& finalPath, bool addTrailingBackslash = false) {
    // Get final path length
    DWORD pathLength = GetFinalPathNameByHandleW(handle, nullptr, 0, FILE_NAME_NORMALIZED);

    if (pathLength == 0) {
        return GetLastError();
    }

    // Allocate string buffer
    finalPath.resize(pathLength - 1);

    // Get final path
    if (GetFinalPathNameByHandleW(handle, &finalPath[0], pathLength, FILE_NAME_NORMALIZED) == 0) {
        return GetLastError();
    }

    // Remove "\\?\" prefix if needed
    if (finalPath.rfind(L"\\\\?\\", 0) == 0) {
        finalPath = finalPath.substr(4);
    }

    // Add a trailing backslash if needed
    if (addTrailingBackslash) {
        if ((finalPath.size() > 0) && (finalPath[finalPath.size() - 1] != L'\\')) {
            finalPath += L"\\";
        }
    }

    return ERROR_SUCCESS;
}

static DWORD getDirectoryFinalPath(wchar_t const* path, std::wstring& finalPath) {
    DWORD errorCode;

    RAIIHandle directoryHandle = getDirectoryHandle(path, 0, errorCode);

    if (!directoryHandle.isValid()) {
        return errorCode;
    }

    return getHandleFinalPath(directoryHandle.borrow(), finalPath, true);
}

// removeFilePathW doesn't correctly handle files at the root of a drive
void computeParentFolder(std::wstring &path)
{
    for (int i = path.size(); i >= 2; i--) {
        if ((path[i] == L'\\') || (path[i] == L'/'))
        {
            if (path[i - 1] == L':') {
                path.resize(i + 1);
            } else {
                path.resize(i);
            }
            
            break;
        }
    }
}

// Instances
LunaPathValidator gLunaPathValidator;
LunaPathValidator gLunaPathValidatorLoadscreen;

// 
static std::unordered_set<std::wstring> naughtyExtensionMap(
    { L"application", L"bat", L"cmd", L"com", L"cpl", L"dll", L"exe", L"gadget", L"hta", L"inf", L"jar", L"js", L"jse", L"lnk", L"lua", L"msc", L"msi", L"msp", L"pif", L"ps1", L"ps1xml", L"ps2", L"ps2xml", L"psc1", L"psc2", L"py", L"pyw", L"reg", L"scf", L"scr", L"vb", L"vbe", L"vbs", L"ws", L"wsf", L"wsh" }
);


LunaPathValidator::LunaPathValidator() :
    mFinalEnginePath(), mFinalEpisodePath()
{
}

LunaPathValidator::~LunaPathValidator()
{
}

void LunaPathValidator::SetPaths()
{
    DWORD errorCode;
    std::wstring mMatchingEnginePath;
    std::wstring mMatchingEpisodePath;

    std::string mEnginePath = NormalizedPath<std::string>(gAppPathUTF8);
    if ((mEnginePath.size() > 0) && (mEnginePath[mEnginePath.size() - 1] != '\\'))
    {
        mEnginePath += "\\";
    }

    mMatchingEnginePath = NormalizedPath<std::wstring>(gAppPathWCHAR);
    if ((mMatchingEnginePath.size() > 0) && (mMatchingEnginePath[mMatchingEnginePath.size() - 1] != L'\\'))
    {
        mMatchingEnginePath += L"\\";
    }
    std::transform(mMatchingEnginePath.begin(), mMatchingEnginePath.end(), mMatchingEnginePath.begin(), towlower);

    // Get the final path of the engine folder
    if (mMatchingEnginePath.size() > 0) {
        errorCode = getDirectoryFinalPath(mMatchingEnginePath.c_str(), mFinalEnginePath);
        if (errorCode != ERROR_SUCCESS) {
            mFinalEnginePath.resize(0);
        }
    }
    // No need to add a trailing backslash, getDirectoryFinalPath adds it automatically
    std::transform(mFinalEnginePath.begin(), mFinalEnginePath.end(), mFinalEnginePath.begin(), towlower);

    mMatchingEpisodePath = NormalizedPath<std::wstring>(GM_FULLDIR);
    if ((mMatchingEpisodePath.size() > 0) && (mMatchingEpisodePath[mMatchingEpisodePath.size() - 1] != L'\\'))
    {
        mMatchingEpisodePath += L"\\";
    }
    std::transform(mMatchingEpisodePath.begin(), mMatchingEpisodePath.end(), mMatchingEpisodePath.begin(), towlower);

    // Get the final path of the episode folder
    if (mMatchingEpisodePath.size() > 0) {
        errorCode = getDirectoryFinalPath(mMatchingEpisodePath.c_str(), mFinalEpisodePath);
        if (errorCode != ERROR_SUCCESS) {
            mFinalEpisodePath.resize(0);
        }
    }
    // No need to add a trailing backslash, getDirectoryFinalPath adds it automatically
    std::transform(mFinalEpisodePath.begin(), mFinalEpisodePath.end(), mFinalEpisodePath.begin(), towlower);
}

DWORD LunaPathValidator::CheckPath(std::wstring const& pathArg, bool requestWrite) {
    // Make path lowercase
    std::wstring path = pathArg;
    std::transform(path.begin(), path.end(), path.begin(), towlower);

    if ((mFinalEpisodePath.size() == 0) || (mFinalEpisodePath != path.substr(0, mFinalEpisodePath.size()))) {
        // If the episode path doesn't match
        
        if ((mFinalEnginePath.size() > 0) && (mFinalEnginePath == path.substr(0, mFinalEnginePath.size()))) {
            // If engine path matches, check for write protection
            bool canWrite = ((path.substr(mFinalEnginePath.size(), 5) == L"logs\\") ||
                            (path.substr(mFinalEnginePath.size(), std::wstring::npos) == L"worlds\\mario challenge\\data.json") ||
                            (path.substr(mFinalEnginePath.size(), std::wstring::npos) == L"worlds\\mario challenge\\"));
            
            if (requestWrite && !canWrite) {
                return ERROR_WRITE_PROTECT;
            }
        } else {
            // Otherwise, refuse file access
            return ERROR_ACCESS_DENIED;
        }
    }
    if (requestWrite) {
        // Check file extension if write access is requested
        std::wstring fileExt = L"";
        std::wstring::size_type pathIdx = path.rfind(L'\\');
        std::wstring::size_type extIdx = path.rfind(L'.');
        if ((extIdx != std::wstring::npos) && ((pathIdx == std::wstring::npos) || (extIdx > pathIdx))) {
            std::wstring fileExt = path.substr(extIdx + 1);
            if (naughtyExtensionMap.find(fileExt) != naughtyExtensionMap.cend()) {
                return ERROR_WRITE_PROTECT;
            }
        }
    }

    return ERROR_SUCCESS;
}

std::wstring LunaPathValidator::NormalizePath(std::wstring const& path) {
    std::wstring wNormalPath;
    
    // Normalize path and make it absolute if necessary
    if (path.size() >= 3 &&
        (
        ((path[0] >= L'A') && (path[0] <= L'Z')) ||
        ((path[0] >= L'a') && (path[0] <= L'z'))
        ) &&
        (path[1] == L':') &&
        ((path[2] == L'/') || (path[2] == L'\\'))
        )
    {
        // It's an absolute path already
        wNormalPath = NormalizedPath<std::wstring>(path);
    }
    else
    {
        // Not absolute path
        wNormalPath = NormalizedPath<std::wstring>(mFinalEnginePath + path);
    }

    // Get wchar_t version of path for checking what it starts with, so that we use towlower to better handle unicode case insensitivity 
    std::transform(wNormalPath.begin(), wNormalPath.end(), wNormalPath.begin(), towlower);

    return wNormalPath;
}

std::FILE* LunaPathValidator::OpenFile(const char* path, const char* mode) {
    if (!path) {
        mLastError.type = ErrorType::NULL_PATH;
        return nullptr;
    }
    
    return OpenFile(std::string(path), mode);
}

std::FILE* LunaPathValidator::OpenFile(const wchar_t* path, const char* mode) {
    if (!path) {
        mLastError.type = ErrorType::NULL_PATH;
        return nullptr;
    }
    
    return OpenFile(std::wstring(path), mode);
}

std::FILE* LunaPathValidator::OpenFile(std::string const& path, const char* mode) {
    return OpenFile(Str2WStr(path), mode);
}

std::FILE* LunaPathValidator::OpenFile(std::wstring const& path, const char* mode) {
    if (!mode) {
        mode = "r";
    }

    FileUtils::FileOpeningMode modeInfo;

    // Parse file opening mode
    if (!FileUtils::ParseFileOpeningMode(mode, modeInfo)) {
        mLastError.type = ErrorType::MODE_PARSING_ERROR;
        mLastError.pathOrMode = mode;
        return nullptr;
    }

    // Normalize path
    std::wstring wNormalPath = NormalizePath(path);
    std::wstring wLongPath = L"\\\\?\\";
    wLongPath += wNormalPath;

    // The file we're trying to open
    RAIIHandle fileHandle = CreateFileW(
        wLongPath.c_str(), // File path
        modeInfo.requestWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ, // Open in readonly mode unless we request write permissions
        FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, // We're allowing other processes to do anything to the file
        nullptr, // This handle can't be inherited by child processes
        OPEN_EXISTING, // We're trying to open an existing file without truncating it
        FILE_ATTRIBUTE_NORMAL, // No extra attributes
        nullptr // Useless for opening existing files/folders
    );

    if (!fileHandle.isValid()) {
        // We couldn't open the file, get the last error to understand why
        mLastError.errorCode = GetLastError();

        // If the file must exist or the reason is not ERROR_FILE_NOT_FOUND, return invalid handle
        if (modeInfo.fileMustExist || mLastError.errorCode != ERROR_FILE_NOT_FOUND) {
            mLastError.type = ErrorType::FILE_OPENING_ERROR;
            mLastError.pathOrMode = WStr2Str(wNormalPath);
            return nullptr;
        }

        // We're trying to create a new file

        // Get the path of the parent folder
        std::wstring parentFolder = wNormalPath;
        computeParentFolder(parentFolder);
        std::wstring longParentFolder = L"\\\\?\\";
        longParentFolder += parentFolder;

        // Get the name of the file to create
        const wchar_t* filename = &wNormalPath[wNormalPath.rfind(L"\\") + 1];

        // Get a handle of the parent folder
        RAIIHandle directoryHandle = getDirectoryHandle(longParentFolder.c_str(), FILE_ADD_FILE, mLastError.errorCode);

        // Error if we can't get the handle
        if (!directoryHandle.isValid()) {
            mLastError.type = ErrorType::DIR_OPENING_ERROR;
            mLastError.pathOrMode = WStr2Str(parentFolder);
            return nullptr;
        }

        // Try to get the final path of the parent folder
        std::wstring finalDirectoryPath;
        mLastError.errorCode = getHandleFinalPath(directoryHandle.borrow(), finalDirectoryPath, true);
        if (mLastError.errorCode != ERROR_SUCCESS) {
            mLastError.type = ErrorType::GET_DIR_FINAL_PATH_ERROR;
            mLastError.pathOrMode = WStr2Str(parentFolder);
            return nullptr;
        }

        // Get path of file to create
        std::wstring newFilePath = NormalizedPath<std::wstring>(finalDirectoryPath + filename);

        // Check if we authorize file creation
        mLastError.errorCode = CheckPath(newFilePath, modeInfo.requestWrite);
        if (mLastError.errorCode != ERROR_SUCCESS) {
            mLastError.type = ErrorType::UNAUTHORIZED_FILE_CREATION;
            mLastError.pathOrMode = WStr2Str(newFilePath);
            return nullptr;
        }

        // Initialize unicode string containing the filename
        UNICODE_STRING filenameUnicode;
        ntdll::RtlInitUnicodeString(&filenameUnicode, filename);

        // Initialize file creation attributes
        OBJECT_ATTRIBUTES attributes;
        InitializeObjectAttributes(
            &attributes,
            &filenameUnicode, // The name of the file to create
            OBJ_CASE_INSENSITIVE, // We ignore case for the file existence check
            directoryHandle.borrow(), // The handle of the directory where the file is to be created
            nullptr
        );

        // IO status block object written by NtCreateFile
        IO_STATUS_BLOCK ioStatusBlock;

        // Create the file
        NTSTATUS status = ntdll::NtCreateFile(
            &fileHandle.getHandleRef(), // Where to write the handle
            SYNCHRONIZE | FILE_READ_ATTRIBUTES | (modeInfo.requestWrite ? (FILE_GENERIC_READ | FILE_GENERIC_WRITE) : FILE_GENERIC_READ), // Open in readonly mode unless we request write permissions
            &attributes,
            &ioStatusBlock,
            nullptr, // We don't care about setting an initial allocation size
            FILE_ATTRIBUTE_NORMAL, // We're creating a normal file
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // We're allowing other processes to do anything to the file
            FILE_CREATE, // Error if the file already exists
            FILE_NON_DIRECTORY_FILE| FILE_SYNCHRONOUS_IO_NONALERT, // We're not creating a directory, we open the file in synchronous IO mode
            nullptr, // I don't even know what's an EA buffer
            0
        );

        if (status != STATUS_SUCCESS) {
            mLastError.errorCode = ntdll::RtlNtStatusToDosError(status);
            mLastError.type = ErrorType::FILE_CREATION_ERROR;
            mLastError.pathOrMode = WStr2Str(newFilePath);
            return nullptr;
        }

        UpdateLastPath(newFilePath);
    } else {
        // Get final path of opened file
        std::wstring finalFilePath;
        mLastError.errorCode = getHandleFinalPath(fileHandle.borrow(), finalFilePath);
        if (mLastError.errorCode != ERROR_SUCCESS) {
            mLastError.type = ErrorType::GET_FILE_FINAL_PATH_ERROR;
            mLastError.pathOrMode = WStr2Str(wNormalPath);
            return nullptr;
        }

        // Check if we authorize file access
        mLastError.errorCode = CheckPath(finalFilePath, modeInfo.requestWrite);
        if (mLastError.errorCode != ERROR_SUCCESS) {
            mLastError.type = ErrorType::UNAUTHORIZED_FILE_ACCESS;
            mLastError.pathOrMode = WStr2Str(finalFilePath);
            return nullptr;
        }

        UpdateLastPath(finalFilePath);
    }

    // Convert file handle to file descriptor
    HANDLE rawFileHandle = fileHandle.takeOwnership();
    int fd = _open_osfhandle((std::intptr_t) rawFileHandle, modeInfo.flags);
    if (fd == -1) {
        CloseHandle(rawFileHandle);
        mLastError.type = ErrorType::DESCRIPTOR_CREATION_ERROR;
        return nullptr;
    }
    
    // Convert file descriptor to C file object
    FILE* fileObject = _fdopen(fd, mode);
    if (!fileObject) {
        mLastError.type = ErrorType::FILE_OBJECT_CREATION_ERROR;
        mLastError.errorCode = _doserrno;
        mLastError.cErrorCode = errno;
        _close(fd);
        return nullptr;
    }

    // Truncate file if needed since _open_osfhandle and _fdopen don't do it automatically
    if (modeInfo.flags & _O_TRUNC) {
        if (_chsize(fd, 0) != 0) {
            mLastError.type = ErrorType::TRUNCATE_ERROR;
            mLastError.errorCode = _doserrno;
            mLastError.cErrorCode = errno;
            std::fclose(fileObject);
            return nullptr;
        }
    }

    return fileObject;
}

HANDLE LunaPathValidator::CreateTempFile(HANDLE parentFolder) {
    // RNG for generating temp file names
    static thread_local std::mt19937 rng(GetTickCount());

    HANDLE tmpHwnd = INVALID_HANDLE_VALUE;
    for (uint32_t i=0; (i<=0xFFFF) && (tmpHwnd == INVALID_HANDLE_VALUE); i++)
    {
        static const wchar_t* digits = L"0123456789ABCDEFGHIJKLMNOPQRSTUV";
        std::wstring tmpPath = L".";
        uint32_t randomNumber = rng();
        for (int j = 0; j < 16; j += 5)
        {
            tmpPath += digits[(randomNumber >> j) & 0xF];
        }
        tmpPath += L".TMP";

        // Initialize unicode string containing the filename
        UNICODE_STRING filenameUnicode;
        ntdll::RtlInitUnicodeString(&filenameUnicode, tmpPath.c_str());

        // Initialize file creation attributes
        OBJECT_ATTRIBUTES attributes;
        InitializeObjectAttributes(
            &attributes,
            &filenameUnicode, // The name of the file to create
            OBJ_CASE_INSENSITIVE, // We ignore case for the file existence check
            parentFolder, // The handle of the directory where the file is to be created
            nullptr
        );

        // IO status block object written by NtCreateFile
        IO_STATUS_BLOCK ioStatusBlock;

        // Create the file
        NTSTATUS status = ntdll::NtCreateFile(
            &tmpHwnd, // Where to write the handle
            FILE_GENERIC_WRITE | DELETE | SYNCHRONIZE | FILE_READ_ATTRIBUTES, // We can write to the file or delete it
            &attributes,
            &ioStatusBlock,
            nullptr, // We don't care about setting an initial allocation size
            FILE_ATTRIBUTE_NORMAL, // We're creating a normal file
            0, // We're not allowing other processes to do anything to the file
            FILE_CREATE, // Error if the file already exists
            FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT, // We're not creating a directory, we open the file in synchronous IO mode
            nullptr, // I don't even know what's an EA buffer
            0
        );

        if (tmpHwnd == INVALID_HANDLE_VALUE)
        {
            // No success
            mLastError.errorCode = GetLastError();
            if (mLastError.errorCode == ERROR_FILE_EXISTS)
            {
                // File exists? Retry
                continue;
            }
            else
            {
                // Other failure, abort
                return INVALID_HANDLE_VALUE;
            }
        }
    }

    if (tmpHwnd == INVALID_HANDLE_VALUE)
    {
        // Something very wrong... even 0xFFFF retries got "ERROR_FILE_EXISTS"
        return INVALID_HANDLE_VALUE;
    }
    return tmpHwnd;
}

// Mark a file handle for deletion, ignore errors
void MarkForDeletion(HANDLE handle) {
    FILE_DISPOSITION_INFO deletionInfo;
    deletionInfo.DeleteFileW = true;

    SetFileInformationByHandle(
        handle,
        FileDispositionInfo,
        &deletionInfo,
        sizeof(deletionInfo)
    );
}

bool LunaPathValidator::WriteFileAtomic(const char* path, const void* data, ptrdiff_t dataLen) {
    if (!path) {
        mLastError.type = ErrorType::NULL_PATH;
        return false;
    }

    return WriteFileAtomic(std::string(path), data, dataLen);
}

bool LunaPathValidator::WriteFileAtomic(const wchar_t* path, const void* data, ptrdiff_t dataLen) {
    if (!path) {
        mLastError.type = ErrorType::NULL_PATH;
        return false;
    }

    return WriteFileAtomic(std::wstring(path), data, dataLen);
}

bool LunaPathValidator::WriteFileAtomic(std::string const& path, const void* data, ptrdiff_t dataLen) {
    return WriteFileAtomic(Str2WStr(path), data, dataLen);
}

bool LunaPathValidator::WriteFileAtomic(std::wstring const& path, const void* data, ptrdiff_t dataLen) {
    // Normalize path
    std::wstring wNormalPath = NormalizePath(path);

    // Get the path of the parent folder
    std::wstring parentFolder = wNormalPath;
    computeParentFolder(parentFolder);
    std::wstring longParentFolder = L"\\\\?\\";
    longParentFolder += parentFolder;

    // Get the name of the file to create
    const wchar_t* filename = &wNormalPath[wNormalPath.rfind(L"\\") + 1];
    std::size_t filenameLength = std::wcslen(filename);

    // Get a handle of the parent folder
    RAIIHandle directoryHandle = getDirectoryHandle(longParentFolder.c_str(), FILE_TRAVERSE | FILE_READ_ATTRIBUTES, mLastError.errorCode);

    // Error if we can't get the handle
    if (!directoryHandle.isValid()) {
        mLastError.type = ErrorType::DIR_OPENING_ERROR;
        mLastError.pathOrMode = WStr2Str(parentFolder);
        return false;
    }

    // Try to get the final path of the parent folder
    std::wstring finalDirectoryPath;
    mLastError.errorCode = getHandleFinalPath(directoryHandle.borrow(), finalDirectoryPath, true);
    if (mLastError.errorCode != ERROR_SUCCESS) {
        mLastError.type = ErrorType::GET_DIR_FINAL_PATH_ERROR;
        mLastError.pathOrMode = WStr2Str(parentFolder);
        return false;
    }
    
    // Check if we authorize writing to the target file
    // We don't care about checking for symlinks since SetFileInformationByHandle will overwrite them anyways
    std::wstring actualFilePath = NormalizedPath<std::wstring>(finalDirectoryPath + filename);
    mLastError.errorCode = CheckPath(actualFilePath, true);
    if (mLastError.errorCode != ERROR_SUCCESS) {
        mLastError.type = ErrorType::UNAUTHORIZED_FILE_ACCESS;
        mLastError.pathOrMode = WStr2Str(actualFilePath);
        return false;
    }

    // Check if we authorize temp file creation
    mLastError.errorCode = CheckPath(finalDirectoryPath, true);
    if (mLastError.errorCode != ERROR_SUCCESS) {
        mLastError.type = ErrorType::UNAUTHORIZED_TEMP_FILE_CREATION;
        mLastError.pathOrMode = WStr2Str(parentFolder);
        return false;
    }

    // Create temp file
    RAIIHandle tempFile = CreateTempFile(directoryHandle.borrow());

    if (!tempFile.isValid()) {
        // mLastError.errorCode already set by CreateTempFile
        mLastError.type = ErrorType::TEMP_FILE_CREATION_ERROR;
        mLastError.pathOrMode = WStr2Str(parentFolder);
        return false;
    }

    // Write data to temp file
    DWORD bytesWritten = 0;
    if (WriteFile(tempFile.borrow(), data, dataLen, &bytesWritten, NULL) == 0) {
        // Write failed
        mLastError.type = ErrorType::TEMP_FILE_WRITE_ERROR;
        mLastError.errorCode = GetLastError();

        // Mark temp file for deletion
        MarkForDeletion(tempFile.borrow());
        return false;
    }

    // Not enough bytes written
    if (bytesWritten != dataLen) {
        mLastError.type = ErrorType::TEMP_FILE_INCOMPLETE_WRITE;

        // Mark temp file for deletion
        MarkForDeletion(tempFile.borrow());
        return false;
    }

    // IO status block object written by NtQueryDirectoryFile
    IO_STATUS_BLOCK ioStatusBlock;

    // Actually replace the target file
    std::size_t renameInfoSize = sizeof(FILE_RENAME_INFORMATION) + sizeof(wchar_t) * filenameLength;
    std::unique_ptr<FILE_RENAME_INFORMATION, decltype(std::free)*> renameInfo((PFILE_RENAME_INFORMATION) std::malloc(renameInfoSize), std::free);
    renameInfo->ReplaceIfExists = true; // Replace the target file if it exists
    renameInfo->RootDirectory = directoryHandle.borrow(); // Directory where to move the file
    renameInfo->FileNameLength = filenameLength * sizeof(wchar_t);
    std::wcscpy(renameInfo->FileName, filename);
    NTSTATUS status = ntdll::NtSetInformationFile(
        tempFile.borrow(),
        &ioStatusBlock,
        renameInfo.get(),
        renameInfoSize,
        (FILE_INFORMATION_CLASS) 10 // FileRenameInformation
    );

    if (status != STATUS_SUCCESS) {
        // Unsuccessful replace
        mLastError.type = ErrorType::FILE_REPLACE_ERROR;
        mLastError.errorCode = ntdll::RtlNtStatusToDosError(status);
        mLastError.pathOrMode = WStr2Str(actualFilePath);

        // Mark temp file for deletion
        MarkForDeletion(tempFile.borrow());
        return false;
    }

    UpdateLastPath(actualFilePath);

    // Everything worked as intended!
    return true;
}

bool LunaPathValidator::ListOfDir(const char* path, DWORD attributes, std::vector<std::string>& outputList) {
    if (!path) {
        mLastError.type = ErrorType::NULL_PATH;
        return false;
    }

    return ListOfDir(std::string(path), attributes, outputList);
}

bool LunaPathValidator::ListOfDir(const wchar_t* path, DWORD attributes, std::vector<std::string>& outputList) {
    if (!path) {
        mLastError.type = ErrorType::NULL_PATH;
        return false;
    }

    return ListOfDir(std::wstring(path), attributes, outputList);
}

bool LunaPathValidator::ListOfDir(std::string const& path, DWORD attributes, std::vector<std::string>& outputList) {
    return ListOfDir(Str2WStr(path), attributes, outputList);
}

bool LunaPathValidator::ListOfDir(std::wstring const& path, DWORD attributes, std::vector<std::string>& outputList) {
    // Normalize path
    std::wstring wNormalPath = NormalizePath(path);
    std::wstring wLongPath = L"\\\\?\\";
    wLongPath += wNormalPath;
    
    // Get directory handle
    RAIIHandle directory = getDirectoryHandle(wLongPath.c_str(), FILE_LIST_DIRECTORY, mLastError.errorCode);
    if (!directory.isValid()) {
        mLastError.type = ErrorType::DIR_OPENING_ERROR;
        mLastError.pathOrMode = WStr2Str(wNormalPath);
        return false;
    }

    // Get directory final path
    std::wstring finalDirectoryPath;
    mLastError.errorCode = getHandleFinalPath(directory.borrow(), finalDirectoryPath, true);
    if (mLastError.errorCode != ERROR_SUCCESS) {
        mLastError.type = ErrorType::GET_DIR_FINAL_PATH_ERROR;
        mLastError.pathOrMode = WStr2Str(wNormalPath);
        return false;
    }

    // Check if we're allowed to get the contents of the directory
    mLastError.errorCode = CheckPath(finalDirectoryPath, false);
    if (mLastError.errorCode != ERROR_SUCCESS) {
        mLastError.type = ErrorType::UNAUTHORIZED_FOLDER_LIST;
        mLastError.pathOrMode = WStr2Str(finalDirectoryPath);
        return false;
    }

    // IO status block object written by NtQueryDirectoryFile
    IO_STATUS_BLOCK ioStatusBlock;

    // File data written to by NtQueryDirectoryFile
    constexpr std::size_t fileDataSize = 1024 * 64;
    std::unique_ptr<FILE_DIRECTORY_INFORMATION, decltype(std::free)*> fileData((FILE_DIRECTORY_INFORMATION*) malloc(fileDataSize), std::free);

    // Get first entry in directory
    NTSTATUS status = ntdll::NtQueryDirectoryFile(
        directory.borrow(), 0, nullptr, nullptr,
        &ioStatusBlock, fileData.get(), fileDataSize,
        FileDirectoryInformation,
        false, // Return as many entries as possible
        nullptr,
        true // Restart scan
    );

    while (status != STATUS_NO_MORE_FILES) {
        // Error if we encounter an error
        if (status != STATUS_SUCCESS) {
            mLastError.errorCode = ntdll::RtlNtStatusToDosError(status);
            mLastError.type = ErrorType::FOLDER_LIST_ERROR;
            mLastError.pathOrMode = WStr2Str(finalDirectoryPath);
            return false;
        }

        // Iterate all found files for current iteration
        FILE_DIRECTORY_INFORMATION* currentFile = fileData.get();
        while (true) {
            // Check if the file has the correct attributes
            if ((currentFile->FileAttributes & attributes) != 0) {
                // Get filename
                std::size_t filenameSize = currentFile->FileNameLength / sizeof(wchar_t);
                std::wstring filename(currentFile->FileName, filenameSize);

                // Add filename to return list if it's not . or .. and it has the correct attributes
                if (filename != L"." && filename != L"..") {
                    outputList.push_back(WStr2Str(filename));
                }
            }

            if (currentFile->NextEntryOffset == 0) {
                break;
            }

            currentFile = (FILE_DIRECTORY_INFORMATION*) (((std::uintptr_t) currentFile) + currentFile->NextEntryOffset);
        }

        // Get next entry in directory
        status = ntdll::NtQueryDirectoryFile(
            directory.borrow(), 0, nullptr, nullptr,
            &ioStatusBlock, fileData.get(), fileDataSize,
            FileDirectoryInformation,
            false, // Return as many entries as possible
            nullptr,
            false // Don't restart scan
        );
    }

    // Success!
    mLastSuccessfulPath = finalDirectoryPath;
    return true;
}

void LunaPathValidator::UpdateLastPath(std::wstring const& path) {
    mLastSuccessfulPath = path;
    std::transform(mLastSuccessfulPath.begin(), mLastSuccessfulPath.end(), mLastSuccessfulPath.begin(), towlower);
}

std::wstring const& LunaPathValidator::LastPath() {
    return mLastSuccessfulPath;
}

LunaPathValidator::Error const& LunaPathValidator::LastError() {
    return mLastError;
}

std::string LunaPathValidator::ErrorMessage() {
    std::string errorMessage;
    switch (mLastError.type) {
        case ErrorType::NULL_PATH:
            errorMessage += "Path is null";
            break;

        case ErrorType::MODE_PARSING_ERROR:
            errorMessage += mLastError.pathOrMode;
            errorMessage += " is not a valid file opening mode";
            break;
        case ErrorType::FILE_OPENING_ERROR:
            errorMessage += "Couldn't open handle for file ";
            errorMessage += mLastError.pathOrMode;
            break;

        case ErrorType::GET_FILE_FINAL_PATH_ERROR:
            errorMessage += "Couldn't get final path of file ";
            errorMessage += mLastError.pathOrMode;
            break; 

        case ErrorType::UNAUTHORIZED_FILE_ACCESS:
            errorMessage += "Access to ";
            errorMessage += mLastError.pathOrMode;
            errorMessage += " is unauthorized";
            break; 

        case ErrorType::DIR_OPENING_ERROR:
            errorMessage += "Couldn't open handle for directory ";
            errorMessage += mLastError.pathOrMode;
            break;

        case ErrorType::GET_DIR_FINAL_PATH_ERROR:
            errorMessage += "Couldn't get final path of directory ";
            errorMessage += mLastError.pathOrMode;
            break;

        case ErrorType::UNAUTHORIZED_FILE_CREATION:
            errorMessage += "Creating file ";
            errorMessage += mLastError.pathOrMode;
            errorMessage += " is unauthorized";
            break;

        case ErrorType::FILE_CREATION_ERROR:
            errorMessage += "Couldn't create file ";
            errorMessage += mLastError.pathOrMode;
            break;

        case ErrorType::DESCRIPTOR_CREATION_ERROR:
            errorMessage += "Couldn't convert handle into file descriptor";
            break;

        case ErrorType::FILE_OBJECT_CREATION_ERROR:
            errorMessage += "Couldn't convert file descriptor into C file object";
            break;

        case ErrorType::UNAUTHORIZED_TEMP_FILE_CREATION:
            errorMessage += "Creating temporary file in directory ";
            errorMessage += mLastError.pathOrMode;
            errorMessage += " is unauthorized";
            break;

        case ErrorType::TEMP_FILE_CREATION_ERROR:
            errorMessage += "Couldn't create temporary file in directory ";
            errorMessage += mLastError.pathOrMode;
            break;

        case ErrorType::TEMP_FILE_WRITE_ERROR:
            errorMessage += "Couldn't write data to temporary file";
            break;

        case ErrorType::TEMP_FILE_INCOMPLETE_WRITE:
            errorMessage += "Write operation to temporary file was incomplete";
            break;
        
        case ErrorType::FILE_REPLACE_ERROR:
            errorMessage += "Couldn't replace ";
            errorMessage += mLastError.pathOrMode;
            errorMessage += " by temporary file";
            break;

        case ErrorType::UNAUTHORIZED_FOLDER_LIST:
            errorMessage += "Listing the contents of directory ";
            errorMessage += mLastError.pathOrMode;
            errorMessage += " is unauthorized";
            break;

        case ErrorType::FOLDER_LIST_ERROR:
            errorMessage += "Error while listing the contents of directory ";
            errorMessage += mLastError.pathOrMode;
            break;

        case ErrorType::TRUNCATE_ERROR:
            errorMessage += "Couldn't truncate the file opened with 'w'";
            break;

        };

    if (
        mLastError.type != ErrorType::MODE_PARSING_ERROR && 
        mLastError.type != ErrorType::DESCRIPTOR_CREATION_ERROR && 
        mLastError.type != ErrorType::NULL_PATH && 
        mLastError.type != ErrorType::TEMP_FILE_INCOMPLETE_WRITE
    ) {
        // mLastError.errorCode is a win32 error code
        wchar_t* win32ErrorString;

        if (FormatMessageW(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
            nullptr,
            mLastError.errorCode,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
            (wchar_t*) &win32ErrorString,
            0,
            nullptr
        ) > 0) {
            errorMessage += " (";
            errorMessage += WStr2Str(win32ErrorString);
            errorMessage += ')';

            LocalFree(win32ErrorString);
        } else {
            errorMessage += " (Win32 error code ";
            errorMessage += std::to_string(mLastError.errorCode);
            errorMessage += ')';
        }
    }

    return errorMessage;
}

// Taken from https://gitlab.winehq.org/wine/wine/-/blob/53d513e626205d5506b7e959bf73b22fd1c17908/dlls/msvcrt/errno.c
static int DosErrnoToCErrno(DWORD dosErrno) {
    switch (dosErrno) {
    case ERROR_ACCESS_DENIED:
    case ERROR_NETWORK_ACCESS_DENIED:
    case ERROR_CANNOT_MAKE:
    case ERROR_SEEK_ON_DEVICE:
    case ERROR_LOCK_FAILED:
    case ERROR_FAIL_I24:
    case ERROR_CURRENT_DIRECTORY:
    case ERROR_DRIVE_LOCKED:
    case ERROR_NOT_LOCKED:
    case ERROR_INVALID_ACCESS:
    case ERROR_SHARING_VIOLATION:
    case ERROR_LOCK_VIOLATION:
        return EACCES;

    case ERROR_FILE_NOT_FOUND:
    case ERROR_NO_MORE_FILES:
    case ERROR_BAD_PATHNAME:
    case ERROR_BAD_NETPATH:
    case ERROR_INVALID_DRIVE:
    case ERROR_BAD_NET_NAME:
    case ERROR_FILENAME_EXCED_RANGE:
    case ERROR_PATH_NOT_FOUND:
        return ENOENT;

    case ERROR_IO_DEVICE:
        return EIO;

    case ERROR_BAD_FORMAT:
        return ENOEXEC;

    case ERROR_INVALID_HANDLE:
        return EBADF;

    case ERROR_OUTOFMEMORY:
    case ERROR_INVALID_BLOCK:
    case ERROR_NOT_ENOUGH_QUOTA:
    case ERROR_ARENA_TRASHED:
        return ENOMEM;

    case ERROR_BUSY:
        return EBUSY;

    case ERROR_ALREADY_EXISTS:
    case ERROR_FILE_EXISTS:
        return EEXIST;

    case ERROR_BAD_DEVICE:
        return ENODEV;

    case ERROR_TOO_MANY_OPEN_FILES:
        return EMFILE;

    case ERROR_DISK_FULL:
        return ENOSPC;

    case ERROR_BROKEN_PIPE:
        return EPIPE;

    case ERROR_POSSIBLE_DEADLOCK:
        return EDEADLK;

    case ERROR_DIR_NOT_EMPTY:
        return ENOTEMPTY;

    case ERROR_BAD_ENVIRONMENT:
        return E2BIG;

    case ERROR_WAIT_NO_CHILDREN:
    case ERROR_CHILD_NOT_COMPLETE:
        return ECHILD;

    case ERROR_NO_PROC_SLOTS:
    case ERROR_MAX_THRDS_REACHED:
    case ERROR_NESTING_NOT_ALLOWED:
        return EAGAIN;

    default:
        return EINVAL;
    }
}

int LunaPathValidator::LastErrno() {
    switch (mLastError.type) {
    case ErrorType::TRUNCATE_ERROR:
    case ErrorType::FILE_OBJECT_CREATION_ERROR:
        return mLastError.cErrorCode;
    
    case ErrorType::MODE_PARSING_ERROR:
    case ErrorType::NULL_PATH:
        return EINVAL;

    case ErrorType::DESCRIPTOR_CREATION_ERROR:
        return EMFILE;
        
    case ErrorType::TEMP_FILE_INCOMPLETE_WRITE:
        return EIO;

    default:
        return DosErrnoToCErrno(mLastError.errorCode);
    }
}

LunaPathValidator& LunaPathValidator::GetForThread()
{
    if (LunaLoadScreenIsCurrentThread())
    {
        return gLunaPathValidatorLoadscreen;
    }
    return gLunaPathValidator;
}