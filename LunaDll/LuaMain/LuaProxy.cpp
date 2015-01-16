#include "LuaProxy.h"
#include "LuaHelper.h"
#include "../Rendering/Rendering.h"
#include "../Globals.h"
#include "../MOBs/PlayerMOB.h"
#include "../MOBs/NPCs.h"
#include "../Misc/MiscFuncs.h"
#include "../SMBX_Events/SMBXEvents.h"
#include "../Sound/Sound.h"
#include "../Level/Level.h"
#include "../CustomSprites/CSprite.h"
#include "../CustomSprites/CSpriteManager.h"
#include "../Blocks/Blocks.h"
#include "../Layer/Layer.h"
#include "../Animation/Animation.h"
#include "../World/Overworld.h"
#include "../World/WorldLevel.h"

#include "../SdlMusic/SdlMusPlayer.h"
#include "../GlobalFuncs.h"








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
    if(iftype >= 1 && iftype <= 5){
        SMBXAnimation* manimation = ::Animations::Get(m_animationIndex);
        void* ptr = ((&(*(byte*)manimation)) + offset);
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


LuaProxy::World::World()
{}

double LuaProxy::World::playerX()
{
	return SMBXOverworld::get()->XPos;
}

void LuaProxy::World::setPlayerX(double playerX)
{
	SMBXOverworld::get()->XPos = playerX;
}

double LuaProxy::World::playerY()
{
	return SMBXOverworld::get()->YPos;
}

void LuaProxy::World::setPlayerY(double playerY)
{
	SMBXOverworld::get()->YPos = playerY;
}

void LuaProxy::World::mem(int offset, L_FIELDTYPE ftype, luabind::object value)
{
	int iftype = (int)ftype;
	if(iftype >= 1 && iftype <= 5){
		Overworld* pOverworld = ::SMBXOverworld::get();
		void* ptr = ((&(*(byte*)pOverworld)) + offset);
		MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
	}
}

luabind::object LuaProxy::World::mem(int offset, L_FIELDTYPE ftype, lua_State* L)
{
	int iftype = (int)ftype;
	double val = 0;
	if(iftype >= 1 && iftype <= 5){
		Overworld* pOverworld = ::SMBXOverworld::get();
		void* ptr = ((&(*(byte*)pOverworld)) + offset);
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
	default:
		return luabind::object();
	}
}

short LuaProxy::World::currentWalkingDirection()
{
	return SMBXOverworld::get()->currentWalkingDirection;
}

void LuaProxy::World::setCurrentWalkingDirection(short currentWalkingDirection)
{
	SMBXOverworld::get()->currentWalkingDirection = currentWalkingDirection;
}

short LuaProxy::World::currentWalkingFrame()
{
	return SMBXOverworld::get()->currentWalkingFrame;
}

void LuaProxy::World::setCurrentWalkingFrame(short currentWalkingFrame)
{
	SMBXOverworld::get()->currentWalkingFrame = currentWalkingFrame;
}

short LuaProxy::World::currentWalkingFrameTimer()
{
	return SMBXOverworld::get()->currentWalkingFrameTimer;
}

void LuaProxy::World::setCurrentWalkingFrameTimer(short currentWalkingFrameTimer)
{
	SMBXOverworld::get()->currentWalkingFrameTimer = currentWalkingFrameTimer;
}

short LuaProxy::World::currentWalkingTimer()
{
	return SMBXOverworld::get()->currentWalkingTimer;
}

void LuaProxy::World::setCurrentWalkingTimer(short currentWalkingTimer)
{
	SMBXOverworld::get()->currentWalkingTimer = currentWalkingTimer;
}

bool LuaProxy::World::playerIsCurrentWalking()
{
	return (SMBXOverworld::get()->currentWalkingDirection || SMBXOverworld::get()->isCurrentlyWalking ? true : false);
}

luabind::object LuaProxy::World::levelTitle(lua_State* L)
{
	if(SMBXOverworld::get()->currentLevelTitle[0] && SMBXOverworld::get()->currentLevelTitle != (wchar_t*)0x423D00){
		return luabind::object(L, new VBStr(SMBXOverworld::get()->currentLevelTitle));
	}
	return luabind::object();
}

short LuaProxy::World::getCurrentDirection()
{
	short val = SMBXOverworld::get()->currentWalkingFrame;
	if(val == 0 || val == 1)
		return 3;
	if(val == 2 || val == 3)
		return 4;
	if(val == 4 || val == 5)
		return 2;
	if(val == 6 || val == 7)
		return 1;

	return 0;
}

LuaProxy::LevelObject::LevelObject(int index) : m_index(index)
{}

void LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, luabind::object value)
{
	int iftype = (int)ftype;
	if(iftype >= 1 && iftype <= 5){
		WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
		void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
		MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
	}
}

luabind::object LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, lua_State* L)
{
	int iftype = (int)ftype;
	double val = 0;
	if(iftype >= 1 && iftype <= 5){
		WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
		void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
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
	default:
		return luabind::object();
	}
}

bool LuaProxy::LevelObject::isValid()
{
	if((m_index < 0) || (m_index > (signed)GM_LEVEL_COUNT))
		return false;
	return true;
}


double LuaProxy::LevelObject::x()
{
	if(!isValid())
		return 0;
	return SMBXLevel::get(m_index)->XPos;
}


void LuaProxy::LevelObject::setX(double x)
{
	if(!isValid())
		return;
	SMBXLevel::get(m_index)->XPos = x;
}

double LuaProxy::LevelObject::y()
{
	if(!isValid())
		return 0;
	return SMBXLevel::get(m_index)->YPos;
}

void LuaProxy::LevelObject::setY(double y)
{
	if(!isValid())
		return;
	SMBXLevel::get(m_index)->YPos = y;
}

std::ostream& LuaProxy::operator<<(std::ostream& os, const VBStr& wStr)
{
	os << wStr.str();
	return os;
}

LuaProxy::VBStr LuaProxy::LevelObject::levelTitle()
{
	if(!isValid())
		return VBStr(0);
	return VBStr(SMBXLevel::get(m_index)->levelTitle);
}
