#include "LuaHelper.h"
#include "LunaLuaMain.h"

luabind::object LuaHelper::getEventCallbase(lua_State *base, std::string eventTable)
{
	luabind::object _G = luabind::globals(base);
    return _G[eventTable.c_str()];
}


bool LuaHelper::is_function(lua_State *luaState, const char *fname)
{
    luabind::object g = luabind::globals(luaState);
    luabind::object fun = g[fname];
    if (fun.is_valid()) {
        if (luabind::type(fun)==LUA_TFUNCTION) {
            return true;
        }
    }
    return false;
}


