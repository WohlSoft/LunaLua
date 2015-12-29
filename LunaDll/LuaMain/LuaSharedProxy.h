#ifndef LuaSharedProxy_____hhhhhh
#define LuaSharedProxy_____hhhhhh
/*
 * LuaSharedProxy are proxies which are used outside of the Lua Module
 */


#include "LuabindIncludes.h"
#include <string>
class Event
{
public:
    Event(std::string eventName, bool cancleable);
    ~Event() = default;


    bool isCancellable();
    bool cancelled(lua_State* L);
    bool native_cancelled();
    void setCancelled(bool cancled, lua_State* L);
    std::string eventName();
    
    std::string getDirectEventName() const { return m_directEventName; }
    void setDirectEventName(std::string val) { m_directEventName = val; }

    bool getLoopable() const { return m_isLoopable; }
    void setLoopable(bool val) { m_isLoopable = val; }

private:
    std::string m_directEventName;
    
    std::string m_eventName;
    bool m_cancellable;
    bool m_isCancelled;
    bool m_isLoopable;
    
};

#endif