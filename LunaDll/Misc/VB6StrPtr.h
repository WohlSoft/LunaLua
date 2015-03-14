#ifndef _VB6StrPtr_H_
#define _VB6StrPtr_H_

#include <string>

#pragma pack(push, 4)
struct VB6StrPtr {
	wchar_t* ptr;
	
	VB6StrPtr();
	~VB6StrPtr();
	VB6StrPtr(const VB6StrPtr &other);
	VB6StrPtr(const std::wstring &other);
	VB6StrPtr(const std::string &other);
	void operator=(const VB6StrPtr &other);
	void operator=(const std::wstring &other);
	void operator=(const std::string &other);
	operator std::wstring() const;
	operator std::string() const;
	operator bool() const;

	unsigned int length() const;

	bool operator==(const VB6StrPtr &other) const;
	bool operator==(const std::wstring &other) const;
	bool operator==(const std::string &other) const;
};
#pragma pack(pop)

#endif