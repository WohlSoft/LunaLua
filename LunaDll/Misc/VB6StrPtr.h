#ifndef _VB6StrPtr_H_
#define _VB6StrPtr_H_

#include <string>
#include "../LuaMain/LunaGenerator/LunaGenHelperUtils.h"

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
	void operator=(const VB6StrPtr &other);
	void operator=(const std::wstring &other);
	void operator=(const std::string &other);
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
};
#pragma pack(pop)

namespace LunaGen {
    template<>
    struct vb6_converter<std::string> {
        typedef VB6StrPtr from_type; // vb6
        typedef std::string to_type; // C++

        static to_type ConvertForGetter(const from_type& data) {
            return data;
        }

        static from_type ConvertForSetter(const to_type& data) {
            return data;
        }
    };
}


#endif