#ifndef _VB6StrPtr_H_
#define _VB6StrPtr_H_

#include <string>

#define _vbaStrCopy ((void(__fastcall *)(VB6StrPtr*, const wchar_t*))IMP_vbaStrCopy)
#define _vbaFreeStr ((void(__fastcall *)(VB6StrPtr*))IMP_vbaFreeStr)

#pragma pack(push, 4)
struct VB6StrPtr {
    wchar_t* ptr;
    
    VB6StrPtr();
    ~VB6StrPtr();
    VB6StrPtr(const VB6StrPtr &other);
    VB6StrPtr(const std::wstring &other);
    VB6StrPtr(const std::string &other);
    VB6StrPtr(const wchar_t* other);
    VB6StrPtr(const char* other);
    void operator=(const VB6StrPtr &other);
    void operator=(const std::wstring &other);
    void operator=(const std::string &other);
    void operator=(const wchar_t* other);
    void operator=(const char* other);
    operator std::wstring() const;
    operator std::string() const;
    operator bool() const;

    unsigned int length() const;
    void assignNoDestruct(const VB6StrPtr& other);
    void assignNoDestruct(const std::string& other);
    void assignNoDestruct(const std::wstring& other);

    bool operator==(const VB6StrPtr &other) const;
    bool operator==(const std::wstring &other) const;
    bool operator==(const std::string &other) const;
    bool operator==(const wchar_t* other) const;
    bool operator==(const char* other) const;
    bool operator!=(const VB6StrPtr &other) const { return !(*this == other); }
    bool operator!=(const std::wstring &other) const { return !(*this == other); }
    bool operator!=(const std::string &other) const { return !(*this == other); }
    bool operator!=(const wchar_t* other) const { return !(*this == other); }
    bool operator!=(const char* other) const { return !(*this == other); }
};
#pragma pack(pop)

#endif