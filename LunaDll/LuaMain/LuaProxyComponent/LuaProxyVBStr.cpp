#include "../LuaProxy.h"
#include "../../GlobalFuncs.h"


LuaProxy::VBStr::VBStr(wchar_t *ptr)
{
	m_wcharptr = ptr;
}


LuaProxy::VBStr::VBStr(long ptr)
{
	m_wcharptr = (wchar_t*)ptr;
}


std::string LuaProxy::VBStr::str() const
{
	if(!isValid())
		return std::string("");
	return utf8_encode(std::wstring(m_wcharptr, (*(((short*)m_wcharptr) - 2) / 2)));
}

void LuaProxy::VBStr::setStr(const std::string &str)
{
	if(!isValid())
		return;
	size_t len = wcslen(m_wcharptr);
	std::wstring trimmedStr = utf8_decode(str.substr(0, len));
	const wchar_t* newWStr = trimmedStr.c_str();
	wcscpy(m_wcharptr, newWStr);
}

int LuaProxy::VBStr::length() const
{
	if(!isValid())
		return 0;
	return (*(((short*)m_wcharptr) - 2)/2);
}

void LuaProxy::VBStr::setLength(int len)
{
	if(!isValid())
		return;
	*(((short*)m_wcharptr) - 2) = len*2;
}

void LuaProxy::VBStr::clear()
{
	if(!isValid())
		return;
	*(((short*)m_wcharptr) - 2) = (int)0;
	*(short*)m_wcharptr = (short)0;
}


bool LuaProxy::VBStr::isValid() const
{
	return m_wcharptr != NULL;
}

std::ostream& LuaProxy::operator<<(std::ostream& os, const VBStr& wStr)
{
	if(wStr.isValid())
		os << wStr.str();
	return os;
}

std::string LuaProxy::VBStr::luaConcatToString(const luabind::object &value, lua_State *L)
{
	// Get std::string from VBStr object
	boost::optional<LuaProxy::VBStr const> opt_obj = luabind::object_cast_nothrow<LuaProxy::VBStr const>(value);
	if (opt_obj != boost::none)
	{
		return *((VB6StrPtr*)&(*opt_obj).m_wcharptr);
	}

	// Otherwise use default lua_tostring
	value.push(L);
	if (lua_isstring(L, -1) == 0 && lua_isnumber(L, -1) == 0)
	{
		luaL_error(L, "Cannot cast to string");
	}
	std::string str = std::string(lua_tolstring(L, -1, NULL));
	lua_remove(L, -1);
	return str;
}

std::string LuaProxy::VBStr::luaConcat(const luabind::object &arg1, const luabind::object &arg2, lua_State *L)
{
	std::string str1 = luaConcatToString(arg1, L);
	std::string str2 = luaConcatToString(arg2, L);

	return str1 + str2;
}