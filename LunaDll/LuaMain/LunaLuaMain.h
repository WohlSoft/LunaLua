// LunaLuaMain.h -- Main Lua Function Manager
#pragma once

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

namespace LunaLua{
    struct lapiData{
        std::string lapiEventTable;
    };
    std::map<lua_State*, lapiData> extraLapiData;
	static lua_State* mainState = 0;
    static lua_State* mainStateGlobal = 0;
    void initCodeFile(lua_State* &L, std::wstring main_path, std::wstring lapi_path, const char *chunckName);
    void init(std::wstring main_path);
    void DoCodeFile(lua_State* L);
    void Do();
    void TryCloseState(lua_State *&L);
    void TryClose();
}
