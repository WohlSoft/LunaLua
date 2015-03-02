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

	bool isCancleable();
	bool cancled(lua_State* L);
	bool native_cancled();
	void setCancled(bool cancled, lua_State* L);
	std::string eventName();
private:

	std::string m_eventName;
	bool m_cancleable;
	bool m_isCancled;

};

#endif