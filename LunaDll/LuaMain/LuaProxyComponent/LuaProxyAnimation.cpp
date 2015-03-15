#include "../LuaProxy.h"
#include "../../Animation/Animation.h"
#include "../../Misc/MiscFuncs.h"

LuaProxy::Animation::Animation(int animationIndex)
{
	m_animationIndex = animationIndex;
}

void LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State *L)
{
	SMBXAnimation* manimation = ::Animations::Get(m_animationIndex);
	void* ptr = ((&(*(byte*)manimation)) + offset);
	LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L) const
{
	SMBXAnimation* manimation = ::Animations::Get(m_animationIndex);
	void* ptr = ((&(*(byte*)manimation)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}

short LuaProxy::Animation::id() const
{
	if(!isValid())
		return 0;
	return static_cast<short>(::Animations::Get(m_animationIndex)->AnimationID);
}

void LuaProxy::Animation::setId(short id)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->AnimationID = static_cast<EffectID>(id);
}

double LuaProxy::Animation::x() const
{
	if(!isValid())
		return 0;
    return ::Animations::Get(m_animationIndex)->momentum.x;
}

void LuaProxy::Animation::setX(double x)
{
	if(!isValid())
		return;
    ::Animations::Get(m_animationIndex)->momentum.x = x;
}

double LuaProxy::Animation::y() const
{
	if(!isValid())
		return 0;
    return ::Animations::Get(m_animationIndex)->momentum.y;
}

void LuaProxy::Animation::setY(double y)
{
	if(!isValid())
		return;
    ::Animations::Get(m_animationIndex)->momentum.y = y;
}

double LuaProxy::Animation::speedX() const
{
	if(!isValid())
		return 0;
    return ::Animations::Get(m_animationIndex)->momentum.speedX;
}

void LuaProxy::Animation::setSpeedX(double speedX)
{
	if(!isValid())
		return;
    ::Animations::Get(m_animationIndex)->momentum.speedX = speedX;
}

double LuaProxy::Animation::speedY() const
{
	if(!isValid())
		return 0;
    return ::Animations::Get(m_animationIndex)->momentum.speedY;
}

void LuaProxy::Animation::setSpeedY(double speedY)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->momentum.speedY = speedY;
}

double LuaProxy::Animation::height() const
{
	if(!isValid())
		return 0;
    return ::Animations::Get(m_animationIndex)->momentum.height;
}

void LuaProxy::Animation::setHeight(double height)
{
	if(!isValid())
		return;
    ::Animations::Get(m_animationIndex)->momentum.height = height;
}

double LuaProxy::Animation::width() const
{
	if(!isValid())
		return 0;
    return ::Animations::Get(m_animationIndex)->momentum.width;
}

void LuaProxy::Animation::setWidth(double width)
{
	if(!isValid())
		return;
    ::Animations::Get(m_animationIndex)->momentum.width = width;
}

short LuaProxy::Animation::timer() const
{
	return ::Animations::Get(m_animationIndex)->Timer;
}

void LuaProxy::Animation::setTimer(short timer)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->Timer = timer;

}

bool LuaProxy::Animation::isValid() const
{
	return !(m_animationIndex < 0 || m_animationIndex > GM_NPCS_COUNT);
}
