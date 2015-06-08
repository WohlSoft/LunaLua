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

luabind::object LuaProxy::Warp::getIntersectingEntrance(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBX_Warp::Count(),
        [](unsigned short i){ return LuaProxy::Warp(i); },
        [x1, y1, x2, y2](unsigned short i){
        ::SMBX_Warp *obj = ::SMBX_Warp::Get(i);
        if (obj == NULL) return false;

        double wx1 = obj->entranceX;
        if (x2 <= wx1) return false;
        double wy1 = obj->entranceY;
        if (y2 <= wy1) return false;
        double wx2 = wx1 + 32.0;
        if (wx2 <= x1) return false;
        double wy2 = wy1 + 32.0;
        if (wy2 <= y1) return false;

        return true;
    }, L);
}

luabind::object LuaProxy::Warp::getIntersectingExit(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBX_Warp::Count(),
        [](unsigned short i){ return LuaProxy::Warp(i); },
        [x1, y1, x2, y2](unsigned short i){
        ::SMBX_Warp *obj = ::SMBX_Warp::Get(i);
        if (obj == NULL) return false;

        // Ignore if exit isn't pointing to within this level actually
        if (obj->warpToLevelFileName.length() > 0) return false;

        double wx1 = obj->exitX;
        if (x2 <= wx1) return false;
        double wy1 = obj->exitY;
        if (y2 <= wy1) return false;
        double wx2 = wx1 + 32.0;
        if (wx2 <= x1) return false;
        double wy2 = wy1 + 32.0;
        if (wy2 <= y1) return false;

        return true;
    }, L);
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
