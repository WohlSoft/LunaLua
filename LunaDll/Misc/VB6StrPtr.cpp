#include <string>
#include <cstdlib>
#include "VB6StrPtr.h"
#include "..\Defines.h"
#include "..\GlobalFuncs.h"

// Empty constructor
VB6StrPtr::VB6StrPtr() : ptr(NULL) {}

// Constructor from another VB6StrPtr
VB6StrPtr::VB6StrPtr(const VB6StrPtr &other) : ptr(NULL) {
    *this = other;
}

// Constructor from a std::wstring
VB6StrPtr::VB6StrPtr(const std::wstring &other) : ptr(NULL) {
    *this = other;
}

// Constructor from a std::string
VB6StrPtr::VB6StrPtr(const std::string &other) : ptr(NULL) {
    *this = other;
}

// Constructor from a const wchar_t*
VB6StrPtr::VB6StrPtr(const wchar_t* other) : ptr(NULL) {
    *this = other;
}

// Constructor from a const char*
VB6StrPtr::VB6StrPtr(const char* other) : ptr(NULL) {
    *this = other;
}

// Destructor in case this isn't part of a struct
VB6StrPtr::~VB6StrPtr() {
    if (ptr != NULL) {
        _vbaFreeStr(this);
        ptr = NULL;
    }
}

// Assignment operator from another VB6StrPtr
void VB6StrPtr::operator=(const VB6StrPtr &other) {
    _vbaStrCopy(this, other.ptr);
}

// Assignment operator from std::wstring
void VB6StrPtr::operator=(const std::wstring &other) {
    // Allocate temporary memory of the right length so we can prefix the length header
    std::size_t len = other.length();
    unsigned char* tmpVBString = (unsigned char*)malloc(sizeof(unsigned int) + sizeof(wchar_t)*(len + 1));

    // Copy the string data, prefixing it with the length
#pragma warning(suppress: 6011) //if we run out of memory, then it's lost anyway
    *((unsigned int*)&tmpVBString[0]) = len * 2;
#pragma warning(suppress: 6011)
    memcpy(&tmpVBString[4], other.c_str(), sizeof(wchar_t)*(len + 1));

    // Copy the temporary string using _vbaStrCopy
    _vbaStrCopy(this, (wchar_t*)&tmpVBString[4]);

    // Free the temporary string
    free(tmpVBString);
}

// Assignment operator from std::string, assuming it's utf-8 encoding
void VB6StrPtr::operator=(const std::string &other) {
    *this = Str2WStr(other);
}

// Assignment operator from std::wstring
void VB6StrPtr::operator=(const wchar_t* other) {
    if (other == nullptr)
    {
        if (ptr != nullptr) {
            _vbaFreeStr(this);
            ptr = nullptr;
        }
        return;
    }

    // Allocate temporary memory of the right length so we can prefix the length header
    std::size_t len = wcslen(other);
    unsigned char* tmpVBString = (unsigned char*)malloc(sizeof(unsigned int) + sizeof(wchar_t)*(len + 1));

    // Copy the string data, prefixing it with the length
#pragma warning(suppress: 6011) //if we run out of memory, then it's lost anyway
    *((unsigned int*)&tmpVBString[0]) = len * 2;
#pragma warning(suppress: 6011)
    memcpy(&tmpVBString[4], other, sizeof(wchar_t)*(len + 1));

    // Copy the temporary string using _vbaStrCopy
    _vbaStrCopy(this, (wchar_t*)&tmpVBString[4]);

    // Free the temporary string
    free(tmpVBString);
}

// Assignment operator from const char*, assuming it's utf-8 encoding
void VB6StrPtr::operator=(const char* other) {
    if (other == nullptr)
    {
        if (ptr != nullptr) {
            _vbaFreeStr(this);
            ptr = nullptr;
        }
        return;
    }

    *this = Str2WStr(other);
}

// Cast operator to std::wstring
VB6StrPtr::operator std::wstring() const {
    if (ptr == NULL)
        return std::wstring(L"");

    // Make a wstring from the string data, making sure to truncate based on the length prefix just in case
    return std::wstring(ptr, length());
}

// Cast operator to std::string, assuming utf-8 encoding
VB6StrPtr::operator std::string() const {
    if (ptr == NULL)
        return std::string("");

    return std::string(WStr2Str(*this));
}

// Cast operator to bool, true if non-null
VB6StrPtr::operator bool() const {
    return ptr != NULL;
}

// Reads the BSTR length header
unsigned int VB6StrPtr::length() const
{
    if (ptr == NULL)
        return 0;

    return *((unsigned int*)&((unsigned char*)ptr)[-4]) / 2;
}

void VB6StrPtr::assignNoDestruct(const VB6StrPtr & other)
{
    BSTR tmp = 0;
    _vbaStrCopy((VB6StrPtr*)&tmp, other.ptr);
    this->ptr = tmp;
}

void VB6StrPtr::assignNoDestruct(const std::wstring & other)
{
    // Allocate temporary memory of the right length so we can prefix the length header
    std::size_t len = other.length();
    unsigned char* tmpVBString = (unsigned char*)malloc(sizeof(unsigned int) + sizeof(wchar_t)*(len + 1));

    // Copy the string data, prefixing it with the length
#pragma warning(suppress: 6011) //if we run out of memory, then it's lost anyway
    *((unsigned int*)&tmpVBString[0]) = len * 2;
#pragma warning(suppress: 6011)
    memcpy(&tmpVBString[4], other.c_str(), sizeof(wchar_t)*(len + 1));

    // Copy the temporary string using _vbaStrCopy
    BSTR tmp = 0;
    _vbaStrCopy((VB6StrPtr*)&tmp, (wchar_t*)&tmpVBString[4]);
    this->ptr = tmp;

    // Free the temporary string
    free(tmpVBString);
}

void VB6StrPtr::assignNoDestruct(const std::string & other)
{
    this->assignNoDestruct(Str2WStr(other));
}

// Equality operator for VBStrPtr
bool VB6StrPtr::operator==(const VB6StrPtr &other) const
{
    // To my understanding the VB6 strings are generally null-terminated, but
    // there is a length field I trust more, so let's be extra safe and ensure
    // we don't read beyond the length specified.
    unsigned int len1 = length();
    unsigned int len2 = other.length();
    if (len1 != len2) return false;
    if (len1 == 0) return true; // If both zero length, it matches and the pointer might be invalid so have an early check for that
    return std::wcsncmp(ptr, other.ptr, len1) == 0;
}

// Equality operator for str::wstring
bool VB6StrPtr::operator==(const std::wstring &other) const
{
    // To my understanding the VB6 strings are generally null-terminated, but
    // there is a length field I trust more, so let's be extra safe and ensure
    // we don't read beyond the length specified.
    unsigned int len1 = length();
    unsigned int len2 = other.length();
    if (len1 != len2) return false;
    if (len1 == 0) return true; // If both zero length, it matches and the pointer might be invalid so have an early check for that
    return std::wcsncmp(ptr, other.c_str(), len1) == 0;
}

// Equality operator for str::string, assumes it's utf-8 encoded
bool VB6StrPtr::operator==(const std::string &other) const
{
    return (*this == Str2WStr(other));
}

// Equality operator for const wchar_t*
bool VB6StrPtr::operator==(const wchar_t* other) const
{
    bool otherIsEmpty = (other == nullptr) || (other[0] == L'\0');
    if (length() == 0) return otherIsEmpty; // Shortcut
    if (otherIsEmpty) return false;
    return std::wcscmp(ptr, other) == 0;
}

// Equality operator for const char*, assumes it's utf-8 encoded
bool VB6StrPtr::operator==(const char* other) const
{
    return (*this == Str2WStr(other));
}
