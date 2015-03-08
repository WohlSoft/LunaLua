#include "../LuaProxy.h"
#include "../../Animation/Animation.h"
#include "../../Misc/MiscFuncs.h"

LuaProxy::Animation::Animation(int animationIndex)
{
	m_animationIndex = animationIndex;
}

void LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, luabind::object value)
{
	int iftype = (int)ftype;
	if(iftype >= 1 && iftype <= 5){
		SMBXAnimation* manimation = ::Animations::Get(m_animationIndex);
		void* ptr = ((&(*(byte*)manimation)) + offset);
		MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
	}
}

luabind::object LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
	int iftype = (int)ftype;
	double val = 0;
	SMBXAnimation* manimation = ::Animations::Get(m_animationIndex);
	void* ptr = ((&(*(byte*)manimation)) + offset);
	if(iftype >= 1 && iftype <= 6){
		val = GetMem((int)ptr, (FIELDTYPE)ftype);
	}
	switch (ftype) {
	case LFT_BYTE:
		return luabind::object(L, (byte)val);
	case LFT_WORD:
		return luabind::object(L, (short)val);
	case LFT_DWORD:
		return luabind::object(L, (int)val);
	case LFT_FLOAT:
		return luabind::object(L, (float)val);
	case LFT_DFLOAT:
		return luabind::object(L, (double)val);
	case LFT_STRING:
		return luabind::object(L, VBStr((wchar_t*)(int)val));
	default:
		return luabind::object();
	}
}

short LuaProxy::Animation::id()
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

double LuaProxy::Animation::x()
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

double LuaProxy::Animation::y()
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

double LuaProxy::Animation::speedX()
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

double LuaProxy::Animation::speedY()
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

double LuaProxy::Animation::height()
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

double LuaProxy::Animation::width()
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

short LuaProxy::Animation::timer()
{
	return ::Animations::Get(m_animationIndex)->Timer;
}

void LuaProxy::Animation::setTimer(short timer)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->Timer = timer;

}

bool LuaProxy::Animation::isValid()
{
	return !(m_animationIndex < 0 || m_animationIndex > GM_NPCS_COUNT);
}
