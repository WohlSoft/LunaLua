#include <string>
#include <cwctype>
#include <cctype>
#include <locale>
#include <random>
#include "Win32PathUtils.h"
#include "../GlobalFuncs.h"
#include "../Globals.h"

bool isAbsolutePath(const std::wstring& path)
{
    return std::iswalpha(path[0]) && path[1] == L':' && ((path[2] == L'\\') || (path[2] == L'/'));
}

bool isAbsolutePath(const std::string& path)
{
    return std::isalpha(path[0], std::locale("C")) && path[1] == L':' && ((path[2] == '\\') || (path[2] == '/'));
}

void removeFilePathW(std::wstring &path)
{
    for (int i = path.size(); i > 3; i--) {
        if ((path[i] == L'\\') || (path[i] == L'/'))
        {
            path.resize(i);
            break;
        }
    }
}

void removeFilePathW(wchar_t*path, int length)
{
    for (int i = length; i > 3; i--) {
        if ((path[i] == L'\\') || (path[i] == L'/'))
        {
            path[i] = 0;
            break;
        }
    }
}

void removeFilePathA(std::string &path)
{
    for (int i = path.size(); i > 3; i--) {
        if ((path[i] == '\\') || (path[i] == '/'))
        {
            path.resize(i);
            break;
        }
    }
}

void removeFilePathA(char*path, int length)
{
    for (int i = length; i > 3; i--) {
        if ((path[i] == '\\') || (path[i] == '/'))
        {
            path[i] = 0;
            break;
        }
    }
}

std::wstring GetWin32LongPath(const char* path)
{
    return GetWin32LongPath(Str2WStr(path));
}

std::wstring GetWin32LongPath(const std::string& path)
{
    return GetWin32LongPath(Str2WStr(path));
}

std::wstring GetWin32LongPath(const std::wstring& path)
{
    // Make sure it's absolute
    std::wstring newPath;
    if (!isAbsolutePath(path))
    {
        newPath = gAppPathWCHAR;
        if ((newPath.size() > 0) && (newPath[newPath.size() - 1] != L'\\'))
        {
            newPath += L"\\";
        }
        newPath += path;
    }
    else
    {
        newPath = path;
    }

    // Normalize path, resolving .. and such
    newPath = normalizeToBackslashAndResolvePath(newPath);

    // If not starting with "\\?\", add it
    if (newPath.rfind(L"\\\\?\\", 0) != 0)
    {
        newPath = L"\\\\?\\" + newPath;
    }

    return newPath;
}

static uint32_t getRandomU32()
{
    static thread_local std::mt19937 rng(GetTickCount());
    return rng();
}

// Function to write data to a file, making repalcement as close to atomic as Windows seems to allow
bool writeFileAtomic(const std::string& path, const void* data, ptrdiff_t dataLen)
{
    return writeFileAtomic(Str2WStr(path), data, dataLen);
}

// Function to write data to a file, making repalcement as close to atomic as Windows seems to allow
bool writeFileAtomic(const std::wstring& path, const void* data, ptrdiff_t dataLen)
{
    std::wstring pathW = GetWin32LongPath(path);
    if ((pathW.size() <= 0) || (pathW[pathW.size() - 1] == L'\\'))
    {
        // Can't end with backslash
        return false;
    }

    // Make temporary file path
    std::wstring tmpPath;
    HANDLE tmpHwnd = INVALID_HANDLE_VALUE;
    for (uint32_t i=0; (i<=0xFFFF) && (tmpHwnd == INVALID_HANDLE_VALUE); i++)
    {
        static const wchar_t* digits = L"0123456789ABCDEFGHIJKLMNOPQRSTUV";
        tmpPath = pathW + L".";
        uint32_t rng = getRandomU32();
        for (int j = 0; j < 16; j += 5)
        {
            tmpPath += digits[(rng >> j) & 0xF];
        }
        tmpPath += L".TMP";
        tmpHwnd = CreateFileW(tmpPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, NULL, NULL);
        if (tmpHwnd == INVALID_HANDLE_VALUE)
        {
            // No success
            if (GetLastError() == ERROR_FILE_EXISTS)
            {
                // File exists? Retry
                continue;
            }
            else
            {
                // Other failure, abort
                return false;
            }
        }
    }
    if (tmpHwnd == INVALID_HANDLE_VALUE)
    {
        // Something very wrong... even 0xFFFF retries got "ERROR_FILE_EXISTS"
        return false;
    }

    // DEBUG: wprintf(L"Opened tmp file: %s\n", tmpPath.c_str());

    DWORD bytesWritten = 0;
    if ((WriteFile(tmpHwnd, data, dataLen, &bytesWritten, NULL) == 0) || (bytesWritten != dataLen))
    {
        // Write failed
        CloseHandle(tmpHwnd);
        DeleteFileW(tmpPath.c_str());
        return false;
    }

    // DEBUG: wprintf(L"Wrote %u bytes\n", bytesWritten);

    // Close temporary file
    CloseHandle(tmpHwnd);

    // Try to replace target file if possible
    if (ReplaceFileW(pathW.c_str(), tmpPath.c_str(), NULL, REPLACEFILE_IGNORE_MERGE_ERRORS, NULL, NULL) != 0)
    {
        // Success! We're done!
        // DEBUG: wprintf(L"Replaced file %s\n", pathW.c_str());
        return true;
    }

    // Otherwise, let's try to move the file
    if (MoveFileEx(tmpPath.c_str(), pathW.c_str(), MOVEFILE_REPLACE_EXISTING) != 0)
    {
        // DEBUG: wprintf(L"Moved file %s\n", pathW.c_str());
        return true;
    }

    // DEBUG: wprintf(L"Failed to write to %s\n", pathW.c_str());
    DeleteFileW(tmpPath.c_str());
    return false;
}

bool readFileToStr(const std::string& path, std::string& out)
{
    return readFileToStr(Str2WStr(path), out);
}

bool readFileToStr(const std::wstring& path, std::string& out)
{
    // Get long path
    std::wstring pathW = GetWin32LongPath(path);

    HANDLE hwnd = CreateFileW(pathW.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
    if (hwnd == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    // Get size
    DWORD sizeHigh = 0;
    DWORD sizeLow = GetFileSize(hwnd, &sizeHigh);
    
    // Proceed reading if size
    if (sizeLow > 0)
    {
        std::vector<char> data;
        data.resize(sizeLow);
        DWORD bytesRead = 0;
        if ((ReadFile(hwnd, &data[0], sizeLow, &bytesRead, NULL) == 0) || (bytesRead != sizeLow))
        {
            // Read not successful
            CloseHandle(hwnd);
            return false;
        }
        CloseHandle(hwnd);
        // Copy to string
        out = std::string(&data[0], sizeLow);
    }
    else
    {
        CloseHandle(hwnd);
        out.clear();
    }

    return true;
}
