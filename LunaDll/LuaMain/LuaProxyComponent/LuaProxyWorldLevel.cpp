#include "../LuaProxy.h"
#include "../../SMBXInternal/WorldLevel.h"
#include "../../Misc/MiscFuncs.h"



unsigned short LuaProxy::LevelObject::count()
{
    return ::WorldLevel::Count();
}

luabind::object LuaProxy::LevelObject::get(lua_State* L)
{
    return LuaHelper::getObjList(::WorldLevel::Count(), [](unsigned short i){ return LuaProxy::LevelObject(i); }, L);
}

luabind::object LuaProxy::LevelObject::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableLevelObjectID;

    try
    {
        lookupTableLevelObjectID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::WorldLevel::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid LevelObject-ID!\nNeed LevelObject-ID between 1-%d\nGot LevelObject-ID: %d", ::WorldLevel::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for LevelObject-ID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        ::WorldLevel::Count(),
        [](unsigned short i){ return LuaProxy::WorldLevel(i); },
        [&lookupTableLevelObjectID](unsigned short i){
        ::WorldLevel *levelObject = ::WorldLevel::Get(i);
        return (levelObject != NULL) &&
            (levelObject->id <= ::WorldLevel::MAX_ID) && lookupTableLevelObjectID.get()[levelObject->id];
    }, L);
}


LuaProxy::LevelObject::LevelObject(int index) : m_index(index)
{}

void LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L)
{
	WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
	void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
	LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, lua_State* L) const
{
	WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
	void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}

bool LuaProxy::LevelObject::isValid() const
{
	if((m_index < 0) || (m_index > (signed)GM_LEVEL_COUNT))
		return false;
	return true;
}


double LuaProxy::LevelObject::x() const
{
	if(!isValid())
		return 0;
	return SMBXLevel::get(m_index)->momentum.x;
}


void LuaProxy::LevelObject::setX(double x)
{
	if(!isValid())
		return;
	SMBXLevel::get(m_index)->momentum.x = x;
}

double LuaProxy::LevelObject::y() const
{
	if(!isValid())
		return 0;
	return SMBXLevel::get(m_index)->momentum.y;
}

void LuaProxy::LevelObject::setY(double y)
{
	if(!isValid())
		return;
    SMBXLevel::get(m_index)->momentum.y = y;
}

LuaProxy::VBStr LuaProxy::LevelObject::levelTitle() const
{
	if(!isValid())
		return VBStr((wchar_t*)0);
	return VBStr(SMBXLevel::get(m_index)->levelTitle.ptr);
}
