#include "../LuaProxy.h"
#include "../../World/WorldLevel.h"
#include "../../Misc/MiscFuncs.h"


LuaProxy::LevelObject::LevelObject(int index) : m_index(index)
{}

void LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, luabind::object value)
{
	int iftype = (int)ftype;
	if(iftype >= 1 && iftype <= 5){
		WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
		void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
		MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
	}
}

luabind::object LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, lua_State* L)
{
	int iftype = (int)ftype;
	double val = 0;
	WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
	void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
		if(iftype >= 1 && iftype <= 6){
		val = GetMem((int)ptr, (FIELDTYPE)ftype);
	}
	switch (ftype) {
	case LFT_BYTE:
		return luabind::object(L, (byte)val);
	case LFT_WORD:
		return luabind::object(L, (short)val);
	case LFT_DWORD:
		return luabind::object(L, (int)val);
	case LFT_FLOAT:
		return luabind::object(L, (float)val);
	case LFT_DFLOAT:
		return luabind::object(L, (double)val);
	case LFT_STRING:
		return luabind::object(L, VBStr((wchar_t*)(int)val));
	default:
		return luabind::object();
	}
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
	return VBStr(SMBXLevel::get(m_index)->levelTitle);
}
