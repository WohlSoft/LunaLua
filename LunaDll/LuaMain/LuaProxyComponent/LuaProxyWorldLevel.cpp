#include "../LuaProxy.h"
#include "../../World/WorldLevel.h"
#include "../../Misc/MiscFuncs.h"


LuaProxy::LevelObject::LevelObject(int index) : m_index(index)
{}

void LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, luabind::object value, lua_State* L)
{
	WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
	void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
	LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, lua_State* L)
{
	WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
	void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}

bool LuaProxy::LevelObject::isValid()
{
	if((m_index < 0) || (m_index > (signed)GM_LEVEL_COUNT))
		return false;
	return true;
}


double LuaProxy::LevelObject::x()
{
	if(!isValid())
		return 0;
	return SMBXLevel::get(m_index)->XPos;
}


void LuaProxy::LevelObject::setX(double x)
{
	if(!isValid())
		return;
	SMBXLevel::get(m_index)->XPos = x;
}

double LuaProxy::LevelObject::y()
{
	if(!isValid())
		return 0;
	return SMBXLevel::get(m_index)->YPos;
}

void LuaProxy::LevelObject::setY(double y)
{
	if(!isValid())
		return;
	SMBXLevel::get(m_index)->YPos = y;
}

LuaProxy::VBStr LuaProxy::LevelObject::levelTitle()
{
	if(!isValid())
		return VBStr((wchar_t*)0);
	return VBStr(SMBXLevel::get(m_index)->levelTitle.ptr);
}
