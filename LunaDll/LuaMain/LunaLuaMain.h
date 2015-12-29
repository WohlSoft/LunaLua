// LunaLuaMain.h -- Main Lua Function Manager
#ifndef LunaLuaMain_____hhhhhh
#define LunaLuaMain_____hhhhhh

#include "LuabindIncludes.h"
#include "LuaSharedProxy.h"
#include "LuaHelper.h"
#include "../Misc/SafeFPUControl.h"
#include "../SMBXInternal/PlayerMOB.h"

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

    //Setting "ready" field
    bool isReady() const { return m_ready;  }
    void setReady(bool ready) { m_ready = ready; }

    template<typename... Args>
    bool callLuaFunction(Args... args){
        if (!isValid())
            return true;

        if (!m_ready)
            return true;

        if (!Player::Get(1)){
            shutdown();
            return true;
        }

        bool err = false;
        try
        {
            SafeFPUControl noFPUExecptions;
            luabind::call_function<void>(args...);
        }
        catch (luabind::error& /*e*/)
        {
            err = true;
        }
        err = err || luabind::object_cast<bool>(luabind::globals(L)["__isLuaError"]);

        if (err)
            shutdown();

        return err;
    }

    template<typename... Args>
    void callEvent(Event* e, Args... args){
        callLuaFunction(L, "__callEvent", e, args...);
    }

private:
	LuaLunaType m_type;
	std::string m_luaEventTableName;

	//private init functions
	void bindAll();
    void bindAllDeprecated();

	void setupDefaults();

	lua_State *L;
    bool m_ready; //This should prevent executing the event loop and catching events if SMBX is not ready.
    bool m_eventLoopOnceExecuted; //This should be an alternative to "onLoad". With this flag the event "onStart" is beeing called, if it is false.
};


#endif
