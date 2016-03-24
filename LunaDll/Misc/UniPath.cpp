#include "UniPath.h"
#include <windows.h>

UniPath::UniPath()
{}

UniPath::UniPath(std::string path)
{
    setPath(path);
}

UniPath::UniPath(std::wstring path)
{
    setPath(path);
}

void UniPath::setPath(std::string path)
{
    m_utf8_path = path;
    wchar_t *newpath = new wchar_t[path.size()+1];
    int new_Len = MultiByteToWideChar(CP_UTF8, 0, (char*)path.c_str(), path.size(), newpath, path.size());
    newpath[new_Len] = L'\0';
    m_wchar_path.clear();
    m_wchar_path.append(newpath);
    delete[] newpath;
}

void UniPath::setPath(std::wstring path)
{
    m_wchar_path = path;
    char *newpath = new char[(path.size()*2)+1];
    int new_Len = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)path.c_str(), path.size(), newpath, path.size(), 0, 0);
    newpath[new_Len] = '\0';
    m_utf8_path.clear();
    m_utf8_path.append(newpath);
    delete[] newpath;
}

std::string UniPath::path()
{
    return m_utf8_path;
}

std::wstring UniPath::pathW()
{
    return m_wchar_path;
}
