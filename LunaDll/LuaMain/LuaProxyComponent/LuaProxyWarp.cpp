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

LuaProxy::Warp LuaProxy::Warp::spawn(const luabind::object &value, double entranceX, double entranceY, double exitX, double exitY, lua_State* L)
{
	// Currently not use this code. read "LuaProxyGlobalFunctions.cpp".
	// "LuaProxyNPC.cpp" have same problems.

	if (GM_WARP_COUNT >= 5000) {
		luaL_error(L, "Over 5000 Warps, cannot spawn more!");
		return LuaProxy::Warp(-1);
	}

	LuaProxy::Warp theNewWarp(GM_WARP_COUNT);
	
	theNewWarp.setEntranceX(entranceX);
	theNewWarp.setEntranceY(entranceY);
	theNewWarp.setExitX(exitX);
	theNewWarp.setExitY(exitY);

	theNewWarp.mem(0x0E, LuaProxy::L_FIELDTYPE::LFT_WORD, luabind::adl::object(L, -1), L);
	theNewWarp.mem(0x10, LuaProxy::L_FIELDTYPE::LFT_WORD, luabind::adl::object(L, -1), L);
	theNewWarp.mem(0x2A, LuaProxy::L_FIELDTYPE::LFT_WORD, luabind::adl::object(L, 16448), L);
	theNewWarp.mem(0x32, LuaProxy::L_FIELDTYPE::LFT_WORD, luabind::adl::object(L, 16448), L);
	theNewWarp.mem(0x32, LuaProxy::L_FIELDTYPE::LFT_STRING, luabind::adl::object(L, "Default"), L);

	++(GM_WARP_COUNT);

	native_updateWarp();

	return theNewWarp;
}

luabind::object LuaProxy::Warp::getIntersectingEntrance(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBX_Warp::Count(),
        [](unsigned short i){ return LuaProxy::Warp(i); },
        [x1, y1, x2, y2](unsigned short i){
        ::SMBX_Warp *obj = ::SMBX_Warp::Get(i);
        if (obj == NULL) return false;

        double wx1 = obj->entrance.x;
        if (x2 <= wx1) return false;
        double wy1 = obj->entrance.y;
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

        double wx1 = obj->exit.x;
        if (x2 <= wx1) return false;
        double wy1 = obj->exit.y;
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

bool LuaProxy::Warp::isHidden() const
{
    return SMBX_Warp::Get(m_index)->isHidden != 0;
}

void LuaProxy::Warp::setIsHidden(bool isHidden)
{
    SMBX_Warp::Get(m_index)->isHidden = isHidden ? -1 : 0;
}

double LuaProxy::Warp::entranceX() const
{
    return SMBX_Warp::Get(m_index)->entrance.x;
}

void LuaProxy::Warp::setEntranceX(double entranceX)
{
    SMBX_Warp::Get(m_index)->entrance.x = entranceX;
}

double LuaProxy::Warp::entranceY() const
{
    return SMBX_Warp::Get(m_index)->entrance.y;
}

void LuaProxy::Warp::setEntranceY(double entranceY)
{
    SMBX_Warp::Get(m_index)->entrance.y = entranceY;
}

double LuaProxy::Warp::exitX() const
{
    return SMBX_Warp::Get(m_index)->exit.x;
}

void LuaProxy::Warp::setExitX(double exitX)
{
    SMBX_Warp::Get(m_index)->exit.x = exitX;
}

double LuaProxy::Warp::exitY() const
{
    return SMBX_Warp::Get(m_index)->exit.y;
}

void LuaProxy::Warp::setExitY(double exitY)
{
    SMBX_Warp::Get(m_index)->exit.y = exitY;
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
