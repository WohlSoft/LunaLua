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
	return ::Animations::Get(m_animationIndex)->AnimationID;
}

void LuaProxy::Animation::setId(short id)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->AnimationID = id;
}

double LuaProxy::Animation::x()
{
	if(!isValid())
		return 0;
	return ::Animations::Get(m_animationIndex)->XPos;
}

void LuaProxy::Animation::setX(double x)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->XPos = x;
}

double LuaProxy::Animation::y()
{
	if(!isValid())
		return 0;
	return ::Animations::Get(m_animationIndex)->YPos;
}

void LuaProxy::Animation::setY(double y)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->YPos = y;
}

double LuaProxy::Animation::speedX()
{
	if(!isValid())
		return 0;
	return ::Animations::Get(m_animationIndex)->XSpeed;
}

void LuaProxy::Animation::setSpeedX(double speedX)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->XSpeed = speedX;
}

double LuaProxy::Animation::speedY()
{
	if(!isValid())
		return 0;
	return ::Animations::Get(m_animationIndex)->YSpeed;
}

void LuaProxy::Animation::setSpeedY(double speedY)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->YSpeed = speedY;
}

double LuaProxy::Animation::height()
{
	if(!isValid())
		return 0;
	return ::Animations::Get(m_animationIndex)->Height;
}

void LuaProxy::Animation::setHeight(double height)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->Height = height;
}

double LuaProxy::Animation::width()
{
	if(!isValid())
		return 0;
	return ::Animations::Get(m_animationIndex)->Width;
}

void LuaProxy::Animation::setWidth(double width)
{
	if(!isValid())
		return;
	::Animations::Get(m_animationIndex)->Width = width;
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
