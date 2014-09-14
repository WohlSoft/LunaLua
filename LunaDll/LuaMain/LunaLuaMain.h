// LunaLuaMain.h -- Main Lua Function Manager
#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

namespace LunaLua{
    int lastSection;
	static lua_State* mainState = 0;
    void init(std::wstring main_path);
    void Do();
    void TryClose();
}
