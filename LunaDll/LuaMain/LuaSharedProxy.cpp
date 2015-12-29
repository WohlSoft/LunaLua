#include "LuaSharedProxy.h"



Event::Event(std::string eventName, bool cancellable) : 
    m_directEventName(""),
    m_eventName(eventName),
    m_cancellable(cancellable),
    m_isCancelled(false),
    m_isLoopable(true) 
{}

bool Event::isCancellable()
{
	return m_cancellable;
}

bool Event::cancelled(lua_State* L)
{
	if (!isCancellable())
		luaL_error(L, "Cannot cancle a non-cancleable event");
	return m_isCancelled;
}

bool Event::native_cancelled()
{
	return m_isCancelled;
}

void Event::setCancelled(bool cancled, lua_State* L)
{
	if (!isCancellable())
		luaL_error(L, "Cannot cancle a non-cancleable event");
	m_isCancelled = cancled;
}

std::string Event::eventName()
{
	return m_eventName;
}
