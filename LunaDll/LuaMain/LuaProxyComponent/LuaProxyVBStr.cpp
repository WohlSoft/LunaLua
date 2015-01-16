#include "../LuaProxy.h"
#include "../../GlobalFuncs.h"


LuaProxy::VBStr::VBStr(wchar_t *ptr)
{
	m_wcharptr = ptr;
}

std::string LuaProxy::VBStr::str() const
{
	return utf8_encode(std::wstring(m_wcharptr));
}

void LuaProxy::VBStr::setStr(std::string str)
{
	size_t len = wcslen(m_wcharptr);
	std::wstring trimmedStr = utf8_decode(str.substr(0, len));
	const wchar_t* newWStr = trimmedStr.c_str();
	wcscpy(m_wcharptr, newWStr);
}

int LuaProxy::VBStr::length()
{
	if(!m_wcharptr)
		return 0;
	return (*(((short*)m_wcharptr) - 2)/2);
}

void LuaProxy::VBStr::setLength(int len)
{
	if(!m_wcharptr)
		return;
	*(((short*)m_wcharptr) - 2) = len*2;
}

void LuaProxy::VBStr::clear()
{
	*(((short*)m_wcharptr) - 2) = (int)0;
	*(short*)m_wcharptr = (short)0;
}



std::ostream& LuaProxy::operator<<(std::ostream& os, const VBStr& wStr)
{
	os << wStr.str();
	return os;
}
