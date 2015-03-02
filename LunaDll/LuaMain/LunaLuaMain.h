// LunaLuaMain.h -- Main Lua Function Manager
#ifndef LunaLuaMain_____hhhhhh
#define LunaLuaMain_____hhhhhh

#include "LuabindIncludes.h"
#include "LuaSharedProxy.h"
#include "LuaHelper.h"

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

	template<typename... Args>
	void callEvent(Event* e, Args... args){
		if (!isValid())
			return;

		if (!Player::Get(1)){
			shutdown();
			return;
		}

		bool err = false;
		try
		{
			luabind::object evTable = LuaHelper::getEventCallbase(L, m_luaEventTableName);
			luabind::object cl = evTable[e->eventName()];
			luabind::call_function<void>(cl, e, args...);
		}
		catch (luabind::error& /*e*/)
		{
			err = true;
		}
		if (err)
			shutdown();
	}

private:
	LuaLunaType m_type;
	std::string m_luaEventTableName;

	//private init functions
	void bindAll();
	void setupDefaults();

	lua_State *L;
};


#endif
