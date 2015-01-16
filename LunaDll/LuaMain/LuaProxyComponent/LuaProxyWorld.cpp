#include "../LuaProxy.h"
#include "../../World/Overworld.h"
#include "../../Misc/MiscFuncs.h"

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