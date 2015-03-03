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
	~Event();

	bool isCancellable();
	bool cancelled(lua_State* L);
	bool native_cancelled();
	void setCancelled(bool cancled, lua_State* L);
	std::string eventName();
private:

	std::string m_eventName;
	bool m_cancellable;
	bool m_isCancelled;

};

#endif