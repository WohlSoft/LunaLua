#include "../LuaProxy.h"
#include "../LuaHelper.h"
#include "../../SMBXInternal/NPCs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/VB6StrPtr.h"

LuaProxy::NPC::NPC(int index)
{
	m_index = index;
}

int LuaProxy::NPC::id(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return (int)::NPC::Get(m_index)->id;
}

float LuaProxy::NPC::direction(lua_State* L) const
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

double LuaProxy::NPC::x(lua_State* L) const
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

double LuaProxy::NPC::y(lua_State* L) const
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

double LuaProxy::NPC::speedX(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.speedX;
}

void LuaProxy::NPC::setSpeedX(double speedX, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.speedX = speedX;
}

double LuaProxy::NPC::speedY(lua_State* L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::NPC::Get(m_index)->momentum.speedY;
}

void LuaProxy::NPC::setSpeedY(double speedY, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->momentum.speedY = speedY;
}

void LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State* L)
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
	::NPC::Get(m_index)->killFlag = 1;
}

void LuaProxy::NPC::kill(int killEffectID, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->killFlag = killEffectID;
}

luabind::object LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State* L) const
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* mnpc = ::NPC::Get(m_index);
	void* ptr = ((&(*(byte*)mnpc)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}

LuaProxy::VBStr LuaProxy::NPC::attachedLayerName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->attachedLayerName.ptr);
}

void LuaProxy::NPC::setAttachedLayerName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->attachedLayerName, value, L);
}

luabind::object LuaProxy::NPC::attachedLayerObj(lua_State *L) const
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return findlayer(((std::string)thisnpc->attachedLayerName).c_str(), L);
}

void LuaProxy::NPC::setAttachedLayerObj(const LuaProxy::Layer &value, lua_State *L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	thisnpc->attachedLayerName = ::Layer::Get(value.layerIndex())->ptLayerName;
}

LuaProxy::VBStr LuaProxy::NPC::activateEventName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->activateEventLayerName.ptr);
}

void LuaProxy::NPC::setActivateEventName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->activateEventLayerName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::deathEventName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->deathEventName.ptr);
}

void LuaProxy::NPC::setDeathEventName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->deathEventName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::talkEventName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->talkEventName.ptr);
}

void LuaProxy::NPC::setTalkEventName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->talkEventName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::noMoreObjInLayer(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->noMoreObjInLayerEventName.ptr);
}

void LuaProxy::NPC::setNoMoreObjInLayer(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->noMoreObjInLayerEventName, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::msg(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->talkMsg.ptr);
}

void LuaProxy::NPC::setMsg(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->talkMsg, value, L);
}

LuaProxy::VBStr LuaProxy::NPC::layerName(lua_State* L) const
{
	if(!isValid_throw(L))
		return VBStr((wchar_t*)0);

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return VBStr(thisnpc->layerName.ptr);
}

void LuaProxy::NPC::setLayerName(const luabind::object &value, lua_State* L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	LuaHelper::assignVB6StrPtr(&thisnpc->layerName, value, L);
}

luabind::object LuaProxy::NPC::layerObj(lua_State *L) const
{
	if(!isValid_throw(L))
		return luabind::object();

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	return findlayer(((std::string)thisnpc->layerName).c_str(), L);
}

void LuaProxy::NPC::setLayerObj(const LuaProxy::Layer &value, lua_State *L)
{
	if (!isValid_throw(L)) return;

	NPCMOB* thisnpc = ::NPC::Get(m_index);
	thisnpc->layerName = ::Layer::Get(value.layerIndex())->ptLayerName;
}

bool LuaProxy::NPC::isValid() const
{
	return !(m_index < 0 || m_index > GM_NPCS_COUNT-1);
}


bool LuaProxy::NPC::isValid_throw(lua_State *L) const
{
	if(!isValid()){
		luaL_error(L, "Invalid NPC-Pointer");
		return false;
	}
	return true;
}

void* LuaProxy::NPC::getNativeAddr() const
{
	return (void*)::NPC::Get(m_index);
}
