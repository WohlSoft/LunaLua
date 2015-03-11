#include <string>
#include <cstdlib>
#include "VB6StrPtr.h"
#include "..\Defines.h"
#include "..\GlobalFuncs.h"

#define _vbaStrCopy ((void(__fastcall *)(VB6StrPtr*, const wchar_t*))IMP_vbaStrCopy)
#define _vbaFreeStr ((void(__fastcall *)(VB6StrPtr*))IMP_vbaFreeStr)

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
	*((unsigned int*)&tmpVBString[0]) = len * 2;
	memcpy(&tmpVBString[4], other.c_str(), sizeof(wchar_t)*(len + 1));

	// Copy the temporary string using _vbaStrCopy
	_vbaStrCopy(this, (wchar_t*)&tmpVBString[4]);

	// Free the temporary string
	free(tmpVBString);
}

// Assignment operator from std::string
void VB6StrPtr::operator=(const std::string &other) {
	*this = utf8_decode(other);
}

// Cast operator to std::wstring
VB6StrPtr::operator std::wstring() const {
	if (ptr == NULL)
		return std::wstring(L"");

	// Get a pointer to the length prefix
	unsigned int* len = (unsigned int*)&((unsigned char*)ptr)[-4];

	// Make a wstring from the string data, making sure to truncate based on the length prefix just in case
	return std::wstring(ptr, *len/2);
}

// Cast operator to std::string
VB6StrPtr::operator std::string() const {
	if (ptr == NULL)
		return std::string("");

	return std::string(utf8_encode(*this));
}

// Cast operator to bool, true if non-null
VB6StrPtr::operator bool() const {
	return ptr != NULL;
}
