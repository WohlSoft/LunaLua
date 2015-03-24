#include "../../LuaProxy.h"
#include "../../../Defines.h"
#include "../../../GlobalFuncs.h"

void LuaProxy::Level::exit()
{
    GM_WORLD_MODE = 0xFFFF;
}

unsigned short LuaProxy::Level::winState()
{
    return GM_WINNING;
}

void LuaProxy::Level::winState(unsigned short value)
{
    GM_WINNING = value;
}

std::string LuaProxy::Level::filename()
{
    return utf8_encode(std::wstring((wchar_t*)GM_LVLFILENAME_PTR));
}

std::string LuaProxy::Level::name()
{
    return utf8_encode(std::wstring((wchar_t*)GM_LVLNAME_PTR));
}