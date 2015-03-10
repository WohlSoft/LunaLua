#include "LuaHelper.h"
#include "LunaLuaMain.h"
#include "../Misc/VB6StrPtr.h"
#include "LuaProxy.h"

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

void LuaHelper::assignVB6StrPtr(VB6StrPtr* ptr, luabind::object value, lua_State* L)
{
	// Copy 
	boost::optional<std::string> opt_str = luabind::object_cast_nothrow<std::string>(value);
	if (opt_str != boost::none)
	{
		*((VB6StrPtr*)ptr) = *opt_str;
		return;
	}

	boost::optional<LuaProxy::VBStr> opt_obj = luabind::object_cast_nothrow<LuaProxy::VBStr>(value);
	if (opt_obj != boost::none)
	{
		*((VB6StrPtr*)ptr) = (*opt_obj).str();
		return;
	}

	luaL_error(L, "Cannot cast to string");
}

