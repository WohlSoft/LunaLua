// LunaLuaMain.h -- Main Lua Function Manager
#ifndef LunaLuaMain_____hhhhhh
#define LunaLuaMain_____hhhhhh

#include <string>
#include <vector>
#include "LuabindIncludes.h"
#include "LuaSharedProxy.h"
#include "LuaHelper.h"
#include "../Misc/SafeFPUControl.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../EventStateMachine.h"
#include "../Misc/PerfTracker.h"

#include <luabind/adopt_policy.hpp>


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
    // Trigger onStart
    void triggerOnStart();
    // Does the event queue
    void doEvents();
    // Exit level/world
    void exitContext();
    // Shutdown lua module
    bool shutdown();
    // If the lua module is valid
    bool isValid(){return L != 0;}

    // Setting "ready" field
    bool isReady() const { return m_ready;  }
    void setReady(bool ready) { m_ready = ready; }

    void setWarning(const std::string& str);

    LuaLunaType getType() const { return m_type; }

    template<typename... Args>
    bool callLuaFunction(Args... args){
        if (!isValid())
            return true;

        if (!Player::Get(1)){
            shutdown();
            return true;
        }

        bool err = false;
        try
        {
            PerfTrackerState perfState(PerfTracker::PERF_LUA);
            SafeFPUControl noFPUExecptions;
            luabind::call_function<void>(args...);
        }
        catch (luabind::error& /*e*/)
        {
            err = true;
        }
        err = err || luabind::object_cast<bool>(luabind::globals(L)["__isLuaError"]);

        // If there was an error, shut down Lua
        if (err)
        {
            shutdown();
        }
        
        // If there was no error, allow a Lua-based game pause to take effect if pending
        if (!err && isValid())
        {
            g_EventHandler.checkPause();
        }


        return err;
    }

    template<typename... Args>
    void callEvent(const std::shared_ptr<Event>& e, Args... args){
        if (m_ready) {
            callLuaFunction(L, "__callEvent", e, args...);
        }
    }

private:
    LuaLunaType m_type;
    std::string m_luaEventTableName;
    std::vector<std::string> m_warningList;

    //private init functions
    void bindAll();
    void bindAllDeprecated();

    void setupDefaults();

    void checkWarnings();
    
    lua_State *L;
    bool m_ready; //This should prevent executing the event loop and catching events if SMBX is not ready.
    bool m_onStartRan;
};


#endif
