#ifndef UNIPATH_H
#define UNIPATH_H

#include <string>

/*!
 * \brief Provides management of paths compatible with both UTF-8 and UTF-16 charsets
 */
class UniPath
{
public:
    UniPath();
    UniPath(std::string path);
    UniPath(std::wstring path);
    void setPath(std::string path);
    void setPath(std::wstring path);
    std::string  path();
    std::wstring pathW();
private:
    std::string     m_utf8_path;
    std::wstring    m_wchar_path;
};

#endif // UNIPATH_H
