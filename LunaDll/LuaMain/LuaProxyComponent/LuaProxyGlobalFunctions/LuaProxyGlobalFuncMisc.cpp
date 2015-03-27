#include "../../LuaProxy.h"
#include "../../../Defines.h"

void LuaProxy::Misc::npcToCoins()
{
    native_npcToCoins();
}

LuaProxy::VBStr LuaProxy::Misc::getInput()
{
    return VBStr((wchar_t*)GM_INPUTSTR_BUF_PTR);
}