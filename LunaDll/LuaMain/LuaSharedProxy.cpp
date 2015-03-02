#include "LuaSharedProxy.h"



Event::Event(std::string eventName, bool cancleable)
{
	m_eventName = eventName;
	m_cancleable = cancleable;
}

Event::~Event()
{}

bool Event::isCancleable()
{
	return m_cancleable;
}

bool Event::cancled(lua_State* L)
{
	if (!isCancleable())
		luaL_error(L, "Cannot cancle a non-cancleable event");
	return m_isCancled;
}

bool Event::native_cancled()
{
	return m_isCancled;
}

void Event::setCancled(bool cancled, lua_State* L)
{
	if (!isCancleable())
		luaL_error(L, "Cannot cancle a non-cancleable event");
	m_isCancled = cancled;
}

std::string Event::eventName()
{
	return m_eventName;
}
