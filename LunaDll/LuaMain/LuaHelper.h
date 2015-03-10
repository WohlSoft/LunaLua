// LuaHelper.h Helper Functions
#ifndef LuaHelper_HHH
#define LuaHelper_HHH

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>
#include "../Misc/VB6StrPtr.h"

namespace LuaHelper {
    luabind::object getEventCallbase(lua_State* base, std::string eventTable);
    bool is_function(struct lua_State * luaState, const char * fname);
	void assignVB6StrPtr(VB6StrPtr* ptr, luabind::object value, lua_State* L);
}

#endif
