#include "../LuaProxy.h"
#include "../LuaHelper.h"
#include "../../SMBXInternal/NPCs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/VB6StrPtr.h"
#include <vector>
#include "../../Misc/RuntimeHook.h"
#include "../../SMBXInternal/Reconstructed/Util/NpcToCoins.h"


int LuaProxy::NPC::count()
{
    return (int)GM_NPCS_COUNT;
}

luabind::object LuaProxy::NPC::get(lua_State* L)
{
    return LuaHelper::getObjList(GM_NPCS_COUNT, [](unsigned short i){ return LuaProxy::NPC(i); }, L);
}

luabind::object LuaProxy::NPC::get(luabind::object idFilter, luabind::object sectionFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableNpcID;
    std::unique_ptr<bool> lookupTableSectionID;

    try
    {
        lookupTableNpcID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::NPC::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid NPC-ID!\nNeed NPC-ID between 1-%d\nGot NPC-ID: %d", ::NPC::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for npcID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    try
    {
        lookupTableSectionID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, sectionFilter, 20, 0));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid Section!\nNeed Section-Index between 0-20\nGot Section-Index: %d", e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for section number (arg #2, expected table or number, got %s)", lua_typename(L, luabind::type(sectionFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        GM_NPCS_COUNT,
        [](unsigned short i){ return LuaProxy::NPC(i); },
        [&lookupTableNpcID, &lookupTableSectionID](unsigned short i){
            NPCMOB* thisnpc = ::NPC::Get(i);
            if (thisnpc == NULL) return false;
        
            short id = thisnpc->id;
            short section = ::NPC::GetSection(thisnpc);
            return (id <= ::NPC::MAX_ID) && lookupTableNpcID.get()[id] &&
                (section <= 20) && lookupTableSectionID.get()[section];
        }, L);
}

luabind::object LuaProxy::NPC::getIntersecting(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        GM_NPCS_COUNT,
        [](unsigned short i){ return LuaProxy::NPC(i); },
        [x1, y1, x2, y2](unsigned short i){
        ::NPCMOB *obj = ::NPC::Get(i);
        if (obj == NULL) return false;
        if (x2 <= obj->momentum.x) return false;
        if (y2 <= obj->momentum.y) return false;
        if (obj->momentum.x + obj->momentum.width <= x1) return false;
        if (obj->momentum.y + obj->momentum.height <= y1) return false;
        return true;
    }, L);
}

LuaProxy::NPC LuaProxy::NPC::spawn(short npcid, double x, double y, short section, lua_State* L)
{
    return LuaProxy::NPC::spawn(npcid, x, y, section, false, false, L);
}

LuaProxy::NPC LuaProxy::NPC::spawn(short npcid, double x, double y, short section, bool respawn, lua_State* L)
{
    return LuaProxy::NPC::spawn(npcid, x, y, section, respawn, false, L);
}

LuaProxy::NPC LuaProxy::NPC::spawn(short npcid, double x, double y, short section, bool respawn, bool centered, lua_State* L)
{

    if (npcid < 1 || npcid > 292){
        luaL_error(L, "Invalid NPC-ID!\nNeed NPC-ID between 1-292\nGot NPC-ID: %d", npcid);
        return LuaProxy::NPC(-1);
    }

    if (section < 0 || section > 20){
        luaL_error(L, "Invalid Section!\nNeed Section-Index between 0-20\nGot Section-Index: %d", section);
        return LuaProxy::NPC(-1);
    }

    if (GM_NPCS_COUNT >= 5000){
        luaL_error(L, "Over 5000 NPCs, cannot spawn more!");
        return LuaProxy::NPC(-1);
    }

    LuaProxy::NPC theNewNPC(GM_NPCS_COUNT);
    void* nativeAddr = theNewNPC.getNativeAddr();


    memset(nativeAddr, 0, 0x158);
    WORD* widthArray = GM_CONF_WIDTH;
    WORD* heightArray = GM_CONF_HEIGHT;
    WORD* gfxWidthArray = GM_CONF_GFXWIDTH;
    WORD* gfxHeightArray = GM_CONF_GFXHEIGHT;

    short width = widthArray[npcid];
    short height = heightArray[npcid];
    short gfxWidth = gfxWidthArray[npcid];
    short gfxHeight = gfxHeightArray[npcid];

    gfxWidth = (gfxWidth ? gfxWidth : width);
    gfxHeight = (gfxHeight ? gfxHeight : height);

    if (centered) {
        x -= 0.5 * (double)width;
        y -= 0.5 * (double)height;
    }

    PATCH_OFFSET(nativeAddr, 0x78, double, x);
    PATCH_OFFSET(nativeAddr, 0x80, double, y);
    PATCH_OFFSET(nativeAddr, 0x88, double, height);
    PATCH_OFFSET(nativeAddr, 0x90, double, width);
    PATCH_OFFSET(nativeAddr, 0x98, double, 0.0);
    PATCH_OFFSET(nativeAddr, 0xA0, double, 0.0);

    PATCH_OFFSET(nativeAddr, 0xA8, double, x);
    PATCH_OFFSET(nativeAddr, 0xB0, double, y);
    PATCH_OFFSET(nativeAddr, 0xB8, double, gfxHeight);
    PATCH_OFFSET(nativeAddr, 0xC0, double, gfxWidth);

    if (respawn) {
        PATCH_OFFSET(nativeAddr, 0xDC, WORD, npcid);
    }
    PATCH_OFFSET(nativeAddr, 0xE2, WORD, npcid);

    PATCH_OFFSET(nativeAddr, 0x12A, WORD, 180);
    PATCH_OFFSET(nativeAddr, 0x124, WORD, -1);
    PATCH_OFFSET(nativeAddr, 0x146, WORD, section);

    ++(GM_NPCS_COUNT);

    return theNewNPC;
}


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


void LuaProxy::NPC::setId(int id, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    ::NPC::Get(m_index)->id = id;
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

double LuaProxy::NPC::width(lua_State *L) const
{
    if (!isValid_throw(L))
        return 0;
    return ::NPC::Get(m_index)->momentum.width;
}

void LuaProxy::NPC::setWidth(double width, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::NPC::Get(m_index)->momentum.width = width;
}

double LuaProxy::NPC::height(lua_State *L) const
{
    if (!isValid_throw(L))
        return 0;
    return ::NPC::Get(m_index)->momentum.height;
}

void LuaProxy::NPC::setHeight(double height, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::NPC::Get(m_index)->momentum.height = height;
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


bool LuaProxy::NPC::isHidden(lua_State* L) const
{
    if (!isValid_throw(L))
        return false;
    return ::NPC::Get(m_index)->isHidden != 0;
}

void LuaProxy::NPC::setIsHidden(bool isHidden, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    ::NPC::Get(m_index)->isHidden = isHidden ? -1 : 0;
}

void LuaProxy::NPC::kill(lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->killFlag = 1;
    ::NPC::Get(m_index)->isHidden = COMBOOL(false);
}

void LuaProxy::NPC::kill(int killEffectID, lua_State* L)
{
	if(!isValid_throw(L))
		return;
	::NPC::Get(m_index)->killFlag = killEffectID;
    ::NPC::Get(m_index)->isHidden = COMBOOL(false);
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

bool LuaProxy::NPC::drawOnlyMask(lua_State * L) const
{
    if (!isValid_throw(L)) return false;

    return (bool)::NPC::Get(m_index)->isMaskOnly;
}

void LuaProxy::NPC::setDrawOnlyMask(bool drawOnlyMask, lua_State * L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->isMaskOnly = COMBOOL(drawOnlyMask);
}

bool LuaProxy::NPC::isInvincibleToSword(lua_State * L) const
{
    if (!isValid_throw(L)) return false;

    return (bool)::NPC::Get(m_index)->invincibilityToSword;
}

void LuaProxy::NPC::setIsInvincibleToSword(bool isInvincibleToSword, lua_State * L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->invincibilityToSword = COMBOOL(isInvincibleToSword);
}

bool LuaProxy::NPC::legacyBoss(lua_State * L) const
{
    if (!isValid_throw(L)) return false;

    return (bool)::NPC::Get(m_index)->legacyBoss;
}

void LuaProxy::NPC::setLegacyBoss(bool legacyBoss, lua_State * L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->legacyBoss = COMBOOL(legacyBoss);
}

bool LuaProxy::NPC::friendly(lua_State * L) const 
{
    if (!isValid_throw(L)) return false;
    
    return (bool)::NPC::Get(m_index)->friendly;
}

void LuaProxy::NPC::setFriendly(bool friendly, lua_State * L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->friendly = COMBOOL(friendly);
}

bool LuaProxy::NPC::dontMove(lua_State * L) const
{
    if (!isValid_throw(L)) return false;

    return (bool)::NPC::Get(m_index)->dontMove;
}

void LuaProxy::NPC::setDontMove(bool dontMove, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->dontMove2 = COMBOOL(dontMove);
    ::NPC::Get(m_index)->dontMove = COMBOOL(dontMove);
}

void LuaProxy::NPC::toIce(lua_State * L)
{
    if (!isValid_throw(L)) return;
    
    NPCMOB* dummy = ::NPC::GetDummyNPC();
    dummy->id = 265;
    short indexCollideWith = 0;
    short targetIndex = m_index + 1;
    CollidersType targetType = COLLIDERS_NPC;
    native_collideNPC(&targetIndex, &targetType, &indexCollideWith);
}

void LuaProxy::NPC::toCoin(lua_State * L)
{
    if (!isValid_throw(L)) return;

    Reconstructed::Util::npcToCoin(::NPC::Get(m_index));
}

bool LuaProxy::NPC::collidesBlockBottom(lua_State * L) const
{
    if (!isValid_throw(L)) return false;

    return ::NPC::Get(m_index)->collidesBelow == 2;
}


void LuaProxy::NPC::setCollidesBlockBottom(bool collidesBlockBottom, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->collidesBelow = (collidesBlockBottom ? 2 : 0);
}

bool LuaProxy::NPC::collidesBlockLeft(lua_State* L) const
{
    if (!isValid_throw(L)) return false;

    return ::NPC::Get(m_index)->collidesLeft == 2;
}

void LuaProxy::NPC::setCollidesBlockLeft(bool collidesBlockLeft, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->collidesLeft = (collidesBlockLeft ? 2 : 0);
}

bool LuaProxy::NPC::collidesBlockUp(lua_State* L) const
{
    if (!isValid_throw(L)) return false;

    return ::NPC::Get(m_index)->collidesAbove == 2;
}

void LuaProxy::NPC::setCollidesBlockUp(bool collidesBlockUp, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->collidesAbove = (collidesBlockUp ? 2 : 0);
}

bool LuaProxy::NPC::collidesBlockRight(lua_State* L) const
{
    if (!isValid_throw(L)) return false;

    return ::NPC::Get(m_index)->collidesRight == 2;
}

void LuaProxy::NPC::setCollidesBlockRight(bool collidesBlockRight, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->collidesRight = (collidesBlockRight ? 2 : 0);
}

bool LuaProxy::NPC::isUnderwater(lua_State* L) const
{
    if (!isValid_throw(L)) return false;

    return ::NPC::Get(m_index)->isUnderwater == 2;
}

void LuaProxy::NPC::setIsUnderwater(bool isUnderwater, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->isUnderwater = (isUnderwater ? 2 : 0);
}



short LuaProxy::NPC::animationFrame(lua_State* L) const
{
    if (!isValid_throw(L)) return 0;

    return ::NPC::Get(m_index)->animationFrame;
}

void LuaProxy::NPC::setAnimationFrame(short animationFrame, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->animationFrame = animationFrame;
}

float LuaProxy::NPC::animationTimer(lua_State* L) const
{
    if (!isValid_throw(L)) return 0.0f;

    return ::NPC::Get(m_index)->animationTimer;
}

void LuaProxy::NPC::setAnimationTimer(float animationTimer, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->animationTimer = animationTimer;
}

short LuaProxy::NPC::killFlag(lua_State* L) const
{
    if (!isValid_throw(L)) return 0;

    return ::NPC::Get(m_index)->killFlag;
}

void LuaProxy::NPC::setKillFlag(short killFlag, lua_State* L)
{
    if (!isValid_throw(L)) return;

    ::NPC::Get(m_index)->killFlag = killFlag;
}




double LuaProxy::NPC::ai1(lua_State * L) const
{
    if (!isValid_throw(L)) return 0.0;
    return ::NPC::Get(m_index)->ai1;
}

void LuaProxy::NPC::setAi1(double ai1, lua_State * L)
{
    if (!isValid_throw(L)) return;
    ::NPC::Get(m_index)->ai1 = ai1;
}

double LuaProxy::NPC::ai2(lua_State * L) const
{
    if (!isValid_throw(L)) return 0.0;
    return ::NPC::Get(m_index)->ai2;
}

void LuaProxy::NPC::setAi2(double ai2, lua_State * L)
{
    if (!isValid_throw(L)) return;
    ::NPC::Get(m_index)->ai2 = ai2;
}

double LuaProxy::NPC::ai3(lua_State * L) const
{
    if (!isValid_throw(L)) return 0.0;
    return ::NPC::Get(m_index)->ai3;
}

void LuaProxy::NPC::setAi3(double ai3, lua_State * L)
{
    if (!isValid_throw(L)) return;
    ::NPC::Get(m_index)->ai3 = ai3;
}

double LuaProxy::NPC::ai4(lua_State * L) const
{
    if (!isValid_throw(L)) return 0.0;
    return ::NPC::Get(m_index)->ai4;
}

void LuaProxy::NPC::setAi4(double ai4, lua_State * L)
{
    if (!isValid_throw(L)) return;
    ::NPC::Get(m_index)->ai4 = ai4;
}

double LuaProxy::NPC::ai5(lua_State * L) const
{
    if (!isValid_throw(L)) return 0.0;
    return ::NPC::Get(m_index)->ai5;
}

void LuaProxy::NPC::setAi5(double ai5, lua_State * L)
{
    if (!isValid_throw(L)) return;
    ::NPC::Get(m_index)->ai5 = ai5;
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
