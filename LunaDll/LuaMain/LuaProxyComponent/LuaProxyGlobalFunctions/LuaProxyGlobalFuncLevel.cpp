#include "../../LuaProxy.h"
#include "../../../Defines.h"
#include "../../../GlobalFuncs.h"

void LuaProxy::Level::exit(int levelExitType)
{
    GM_LEVEL_EXIT_TYPE = levelExitType;
    LuaProxy::Level::exit();
}

void LuaProxy::Level::exit()
{
    GM_EPISODE_MODE = 0xFFFF;
    GM_LEVEL_MODE = 0;
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
    return (std::string)GM_LVLFILENAME_PTR;
}

std::string LuaProxy::Level::name()
{
    return (std::string)GM_LVLNAME_PTR;
}