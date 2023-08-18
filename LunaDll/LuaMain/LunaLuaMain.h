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

class CLunaFFILock
{
private: // Static members
    static DWORD currentLockTlsIdx;
private: // Local members
    const char* const funcName;
    const CLunaFFILock* const parentLock;
public: // Constructor and destructor
    CLunaFFILock(const char* _funcName) :
        funcName(_funcName),
        parentLock((const CLunaFFILock*)TlsGetValue(currentLockTlsIdx))
    {
        TlsSetValue(currentLockTlsIdx, this);
        SafeFPUControl::clear(); // Make sure no FPU exceptions are set when we enter a FFI call
    }
    ~CLunaFFILock() {
        if ((const CLunaFFILock*)TlsGetValue(currentLockTlsIdx) == this)
        {
            TlsSetValue(currentLockTlsIdx, (void*)parentLock);
        }
    }
public: // Static methods
    static void reset()
    {
        TlsSetValue(currentLockTlsIdx, nullptr);
    }
    static const char* getCurrentFuncName()
    {
        const CLunaFFILock* const currentLock = (const CLunaFFILock*)TlsGetValue(currentLockTlsIdx);
        if (currentLock == nullptr)
        {
            return nullptr;
        }
        return currentLock->funcName;
    }
};

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

    // Used for queueing up players to run the onSectionChange event
    void queuePlayerSectionChangeEvent(int playerIdx);


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
            const char* currentFFIFunc = CLunaFFILock::getCurrentFuncName();
            if (currentFFIFunc != nullptr)
            {
                std::string errMsg("Error! The event ");
                errMsg += e->eventName();
                errMsg += " was called during ";
                errMsg += currentFFIFunc;
                MessageBoxA(0, errMsg.c_str(), "Error", MB_ICONWARNING | MB_TASKMODAL);
                _exit(1);
            }

            callLuaFunction(L, "__callEvent", e, args...);

            // If a player changed sections,
            if (m_executeSectionChangeFlag)
            {
                // execute onSectionChange
                m_executeSectionChangeFlag = false;
                // disable writing to playerSectionChangeList while iterating over to execute events 
                m_disableSectionChangeEvent = true;
                for (int i = 0; i < m_playerSectionChangeList.size(); i++) {
                    int playerIdx = m_playerSectionChangeList[i]; // player who changed sections
                    std::shared_ptr<Event> sectionChangeEvent = std::make_shared<Event>("onSectionChange", false);
                    sectionChangeEvent->setDirectEventName("onSectionChange");
                    sectionChangeEvent->setLoopable(false);
                    gLunaLua.callEvent(sectionChangeEvent, (int)(Player::Get(playerIdx)->CurrentSection), playerIdx);
                }
                // allow future sectionChange events
                m_disableSectionChangeEvent = false;
                m_playerSectionChangeList.clear();
            }
        }
    }

    luabind::object newTable()
    {
        if (!m_ready) return luabind::object();
        return luabind::newtable(L);
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

    bool m_disableSectionChangeEvent; // set to true while calling queued onSectionChange events - prevents it from trying to call again mid-loop
    bool m_executeSectionChangeFlag; // whether to execute section change at the end of the next event called
    std::vector<int> m_playerSectionChangeList; // list of player indexes who changed sections

};

namespace CachedReadFile {
    void clearData();
    void holdCached(bool isWorld);
    void releaseCached(bool isWorld);
};

#endif
