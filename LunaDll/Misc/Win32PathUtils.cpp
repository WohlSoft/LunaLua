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
