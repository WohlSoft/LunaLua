#include "../LuaProxy.h"
#include "../../SMBXInternal/Warp.h"

int LuaProxy::Warp::count()
{
    return SMBX_Warp::Count();
}

luabind::object LuaProxy::Warp::get(lua_State* L)
{
    return LuaHelper::getObjList(::SMBX_Warp::Count(), [](unsigned short i){ return LuaProxy::Warp(i); }, L);
}


LuaProxy::Warp::Warp(int warpIndex)
{
    m_index = warpIndex;
}


void LuaProxy::Warp::mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L)
{
    ::SMBX_Warp* pWarp = SMBX_Warp::Get(m_index);
    void* ptr = ((&(*(byte*)pWarp)) + offset);
    LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::Warp::mem(int offset, L_FIELDTYPE ftype, lua_State* L) const
{
    ::SMBX_Warp* pWarp = SMBX_Warp::Get(m_index);
    void* ptr = ((&(*(byte*)pWarp)) + offset);
    return LuaProxy::mem((int)ptr, ftype, L);
}

double LuaProxy::Warp::entranceX() const
{
    return SMBX_Warp::Get(m_index)->entranceX;
}

void LuaProxy::Warp::setEntranceX(double entranceX)
{
    SMBX_Warp::Get(m_index)->entranceX = entranceX;
}

double LuaProxy::Warp::entranceY() const
{
    return SMBX_Warp::Get(m_index)->entranceY;
}

void LuaProxy::Warp::setEntranceY(double entranceY)
{
    SMBX_Warp::Get(m_index)->entranceY = entranceY;
}

double LuaProxy::Warp::exitX() const
{
    return SMBX_Warp::Get(m_index)->exitX;
}

void LuaProxy::Warp::setExitX(double exitX)
{
    SMBX_Warp::Get(m_index)->exitX = exitX;
}

double LuaProxy::Warp::exitY() const
{
    return SMBX_Warp::Get(m_index)->exitY;
}

void LuaProxy::Warp::setExitY(double exitY)
{
    SMBX_Warp::Get(m_index)->exitY = exitY;
}

std::string LuaProxy::Warp::levelFilename()
{
    return SMBX_Warp::Get(m_index)->warpToLevelFileName;
}

void LuaProxy::Warp::setLevelFilename(const luabind::object &value, lua_State* L)
{
    SMBX_Warp* warp = SMBX_Warp::Get(m_index);
    LuaHelper::assignVB6StrPtr(&warp->warpToLevelFileName, value, L);
}
