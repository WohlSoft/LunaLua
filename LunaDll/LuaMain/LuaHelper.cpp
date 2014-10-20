#include "LuaHelper.h"

#include "LunaLuaMain.h"

luabind::object LuaHelper::getEventCallbase(lua_State *base)
{
    luabind::object _G = luabind::globals(base);
    std::string strEventTable = LunaLua::extraLapiData[base].lapiEventTable;
    return _G[strEventTable.c_str()];
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


