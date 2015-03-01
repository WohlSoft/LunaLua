// LunaLuaMain.h -- Main Lua Function Manager
#ifndef LunaLuaMain_____hhhhhh
#define LunaLuaMain_____hhhhhh

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>
#include <luabind/operator.hpp>

class CLunaLua
{
public:
	static const std::wstring LuaLibsPath;
	static std::wstring getLuaLibsPath();
	//Construct a new LunaLua Object
	CLunaLua();
	//Destructs a LunaLua Object and shutdown the engine if needed.
	~CLunaLua();

	//The Codefile type
	enum LuaLunaType {
		LUNALUA_LEVEL,
		LUNALUA_WORLD
	};

	

	//Init a lua code file
	void init(LuaLunaType type, std::wstring codePath, std::wstring levelPath = std::wstring());
	//Does the event queue
	void doEvents();
	//Shutdown lua module
	bool shutdown();
	//If the lua module is valid
	bool isValid(){return L != 0;}

private:
	LuaLunaType m_type;
	std::string m_luaEventTableName;


	//private init functions
	void bindAll();
	void setupDefaults();

	lua_State *L;
};

#endif
