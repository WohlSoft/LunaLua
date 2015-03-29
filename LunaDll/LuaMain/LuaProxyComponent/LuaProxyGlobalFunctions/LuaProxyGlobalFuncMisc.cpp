#include "../../LuaProxy.h"
#include "../../../Defines.h"

void LuaProxy::Misc::npcToCoins()
{
    native_npcToCoins();
}

void LuaProxy::Misc::doPOW()
{
    native_doPow();
}


std::string LuaProxy::Misc::cheatBuffer()
{
    return *(VB6StrPtr*)&(GM_INPUTSTR_BUF_PTR);
}

void LuaProxy::Misc::cheatBuffer(const luabind::object &value, lua_State* L)
{
    LuaHelper::assignVB6StrPtr((VB6StrPtr*)&(GM_INPUTSTR_BUF_PTR), value, L);
}
