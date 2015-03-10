#include <string>
#include <cstdlib>
#include "VB6StrPtr.h"
#include "..\Defines.h"
#include "..\GlobalFuncs.h"

#define _vbaStrCopy ((void(__stdcall *)(const wchar_t*, VB6StrPtr*))IMP_vbaStrCopy)
#define _vbaFreeStr ((void(__stdcall *)(VB6StrPtr*))IMP_vbaFreeStr)

// Empty constructor
VB6StrPtr::VB6StrPtr() : ptr(NULL)
{
}

// Constructor from another VB6StrPtr
VB6StrPtr::VB6StrPtr(const VB6StrPtr &other) : ptr(NULL)
{
	*this = other;
}

// Constructor from a std::wstring
VB6StrPtr::VB6StrPtr(const std::wstring &other) : ptr(NULL)
{
	*this = other;
}

// Constructor from a std::string
VB6StrPtr::VB6StrPtr(const std::string &other) : ptr(NULL)
{
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
void VB6StrPtr::operator=(const VB6StrPtr &other)
{
	_vbaStrCopy(other.ptr, this);
}

// Assignment operator from std::wstring
void VB6StrPtr::operator=(const std::wstring &other)
{
	// Allocate temporary memory of the right length so we can prefix the length header on
	// as to fake a VB6 string to pass to _vbaStrCopy
	std::size_t len = other.length();
	unsigned char* tmpVBString = (unsigned char*)malloc(sizeof(unsigned int) + sizeof(wchar_t)*(len + 1));

	// Copy the string data, prefixing it with the length
	*((unsigned int*)&tmpVBString[0]) = len * 2;
	memcpy(&tmpVBString[4], other.c_str(), sizeof(wchar_t)*(len + 1));

	// Copy the wstring
	_vbaStrCopy((wchar_t*)&tmpVBString[4], this);

	free(tmpVBString);
}

// Assignment operator from std::string
void VB6StrPtr::operator=(const std::string &other)
{
	*this = utf8_decode(other);
}

// Cast operator to std::wstring
VB6StrPtr::operator std::wstring() {
	return std::wstring(ptr);
}

// Cast operator to std::string
VB6StrPtr::operator std::string() {
	return std::string(utf8_encode(*this));
}

// Cast operator to bool, true if non-null
VB6StrPtr::operator bool() {
	return ptr != NULL;
}
