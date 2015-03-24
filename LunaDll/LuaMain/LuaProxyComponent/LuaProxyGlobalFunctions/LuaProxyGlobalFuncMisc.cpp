#include "../../LuaProxy.h"
#include "../../../Defines.h"

void LuaProxy::Misc::npcToCoins()
{
    typedef void npcToCoinsFunc();
    npcToCoinsFunc* f = (npcToCoinsFunc*)GF_NPC_TO_COINS;
    f();
}

LuaProxy::VBStr LuaProxy::Misc::getInput()
{
    return VBStr((wchar_t*)GM_INPUTSTR_BUF_PTR);
}