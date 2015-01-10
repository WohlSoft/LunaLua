// LunaLuaMain.h -- Main Lua Function Manager
#ifndef LunaLuaMain_____hhhhhh
#define LunaLuaMain_____hhhhhh

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>
#include <luabind/operator.hpp>

namespace LunaLua{
    struct lapiData{
        std::string lapiEventTable;
    };

    extern std::map<lua_State*, lapiData> extraLapiData;
	static lua_State* mainStateV2 = 0;
	void initCodeFiles(lua_State* &L, std::wstring levelPath, std::wstring lapi_path);
	void initCodeFileWorld(lua_State* &L, std::wstring episodePath, std::wstring lapi_path);
    void init(std::wstring main_path);
	void initWorld(std::wstring main_path);
    void DoCodeFile(lua_State* L);
    void Do();
	void DoWorld();
	void DoCodeWorldFile(lua_State* L);
    void TryCloseState(lua_State *&L);
    void TryClose();
}

#endif