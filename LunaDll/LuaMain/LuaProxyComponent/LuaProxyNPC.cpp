#include "../LuaProxy.h"
#include "../../MOBs/NPCs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/VB6StrPtr.h"

LuaProxy::NPC::NPC(int index)
{
	m_index = index;
}

int LuaProxy::NPC::id(lua_State* L)
{
	if(!isValid_throw(L))
		return 0;
	return (int)::NPC::Get(m_index)->id;
}

float LuaProxy::NPC::direction(lua_State* L)
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->directionFaced;
}

void LuaProxy::NPC::setDirection(float direction, lua_State* L)
{
	if(!isValid_throw(L))
		return;

	NPCMOB* npc =  ::NPC::Get(m_index);
	setSpeedX(0.0, L);
    npc->directionFaced = direction;
}

double LuaProxy::NPC::x(lua_State* L)
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.x;
}

void LuaProxy::NPC::setX(double x, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.x = x;
}

double LuaProxy::NPC::y(lua_State* L)
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.y;
}

void LuaProxy::NPC::setY(double y, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.y = y;
}

double LuaProxy::NPC::speedX(lua_State* L)
{
	if(!isValid_throw(L))
		return 0;
	return *((double*)((&(*(byte*)::NPC::Get(m_index))) + 0x98));
}

void LuaProxy::NPC::setSpeedX(double speedX, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	double* ptr=(double*)((&(*(byte*)::NPC::Get(m_index))) + 0x98);
	*ptr = speedX;
}

double LuaProxy::NPC::speedY(lua_State* L)
{
	if(!isValid_throw(L))
		return 0;
	return *((double*)((&(*(byte*)::NPC::Get(m_index))) + 0xA0));
}

void LuaProxy::NPC::setSpeedY(double speedY, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	double* ptr=(double*)((&(*(byte*)::NPC::Get(m_index))) + 0xA0);
	*ptr = speedY;
}

void LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, luabind::object value, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	
	NPCMOB* mnpc = ::NPC::Get(m_index);
	void* ptr = ((&(*(byte*)mnpc)) + offset);
	LuaProxy::mem((int)ptr, ftype, value, L);
}

void LuaProxy::NPC::kill(lua_State* L)
{
	if(!isValid_throw(L))
		return;
	NPCMOB* mnpc = ::NPC::Get(m_index);
	void* ptr = ((&(*(byte*)mnpc)) + 290);
	MemAssign((int)ptr, 1.0, OP_Assign, (FIELDTYPE)FT_WORD);
}

void LuaProxy::NPC::kill(int killEffectID, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	NPCMOB* mnpc = ::NPC::Get(m_index);
	void* ptr = ((&(*(byte*)mnpc)) + 290);
	MemAssign((int)ptr, (double)killEffectID, OP_Assign, (FIELDTYPE)FT_WORD);
}

luabind::object LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State* L)
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* mnpc = ::NPC::Get(m_index);
	void* ptr = ((&(*(byte*)mnpc)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}

LuaProxy::VBStr LuaProxy::NPC::attachedLayerName(lua_State* L)
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)));
	return VBStr(ptr);
}

luabind::object LuaProxy::NPC::attachedLayerObj(lua_State *L)
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)));
	return findlayer(utf8_encode(std::wstring(ptr)).c_str(),L);
}

LuaProxy::VBStr LuaProxy::NPC::activateEventName(lua_State* L)
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x2C);
	return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::deathEventName(lua_State* L)
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x30);
	return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::talkEventName(lua_State* L)
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x34);
	return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::noMoreObjInLayer(lua_State* L)
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x38);
	return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::msg(lua_State* L)
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x4C);
	return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::layerName(lua_State* L)
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x3C);
	return VBStr(ptr);
}

luabind::object LuaProxy::NPC::layerObj(lua_State *L)
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x3C);
	return findlayer(utf8_encode(std::wstring(ptr)).c_str(),L);
}

bool LuaProxy::NPC::isValid()
{
	return !(m_index < 0 || m_index > GM_NPCS_COUNT-1);
}


bool LuaProxy::NPC::isValid_throw(lua_State *L)
{
	if(!isValid()){
		luaL_error(L, "Invalid NPC-Pointer");
		return false;
	}
	return true;
}

void* LuaProxy::NPC::getNativeAddr()
{
	return (void*)::NPC::Get(m_index);
}