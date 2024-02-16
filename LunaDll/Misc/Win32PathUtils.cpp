#include "Win32PathUtils.h"
#include <string>
#include <cwctype>
#include <cctype>
#include <locale>

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
