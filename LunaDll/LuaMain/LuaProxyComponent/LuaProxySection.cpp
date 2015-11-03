#include "../LuaProxy.h"
#include "../../Defines.h"


luabind::object LuaProxy::Section::get(lua_State * L)
{
    return LuaHelper::getObjList(21, [](unsigned short i) { return LuaProxy::Section(i); }, L);
}

LuaProxy::Section LuaProxy::Section::get(short secNumber, lua_State * L)
{
    if (secNumber < 1 || secNumber > 21) {
        luaL_error(L, "Section number must be between 1 and 21. (Got id %d)", secNumber);
        return LuaProxy::Section(1);
    }
    return LuaProxy::Section(secNumber - 1);
}

LuaProxy::Section::Section(int sectionNum)
{
	m_secNum = sectionNum;
}

LuaProxy::RECTd LuaProxy::Section::boundary() const
{
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (m_secNum * 6);

	RECTd rectd;
	rectd.left = boundtable[0];
	rectd.top = boundtable[1];
	rectd.bottom = boundtable[2];
	rectd.right = boundtable[3];

	return rectd;
}

void LuaProxy::Section::setBoundary(const LuaProxy::RECTd &boundary)
{
	double* boundtable = (double*)GM_LVL_BOUNDARIES;
	boundtable = boundtable + (m_secNum * 6);
	boundtable[0] = boundary.left;
	boundtable[1] = boundary.top;
	boundtable[2] = boundary.bottom;
	boundtable[3] = boundary.right;
}

short LuaProxy::Section::musicID() const
{
    return GM_SEC_MUSIC_TBL[m_secNum];
}

void LuaProxy::Section::setMusicID(short musicID)
{
    GM_SEC_MUSIC_TBL[m_secNum] = musicID;
}

bool LuaProxy::Section::isLevelWarp() const
{
    return (bool)(GM_SEC_ISWARP[m_secNum] != 0);
}

void LuaProxy::Section::setIsLevelWarp(bool isLevelWarp)
{
    GM_SEC_ISWARP[m_secNum] = COMBOOL(isLevelWarp);
}

bool LuaProxy::Section::hasOffscreenExit() const
{
    return GM_SEC_OFFSCREEN[m_secNum] != 0;
}

void LuaProxy::Section::setHasOffscreenExit(bool hasOffscreenExit)
{
    GM_SEC_OFFSCREEN[m_secNum] = COMBOOL(hasOffscreenExit);
}

short LuaProxy::Section::backgroundID() const
{
    return GM_SEC_BG_ARRAY[m_secNum];
}

void LuaProxy::Section::setBackgroundID(short backgroundID)
{
    GM_SEC_BG_ARRAY[m_secNum] = backgroundID;
}

bool LuaProxy::Section::noTurnBack() const
{
    return (bool)(GM_SEC_NOTURNBACK[m_secNum] != 0);
}

void LuaProxy::Section::setNoTurnBack(bool noTurnBack)
{
    GM_SEC_NOTURNBACK[m_secNum] = COMBOOL(noTurnBack);
}

bool LuaProxy::Section::isUnderwater() const
{
    return (bool)(GM_SEC_ISUNDERWATER[m_secNum] != 0);
}

void LuaProxy::Section::setIsUnderwater(bool isUnderwater)
{
    GM_SEC_ISUNDERWATER[m_secNum] = COMBOOL(isUnderwater);
}

