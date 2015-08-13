#include "../LuaProxy.h"
#include "../../SMBXInternal/PlayerMOB.h"
#include "../../Misc/MiscFuncs.h"



int LuaProxy::Player::count()
{
    return GM_PLAYERS_COUNT;
}

luabind::object LuaProxy::Player::get(lua_State* L)
{
    return LuaHelper::getObjList(GM_PLAYERS_COUNT, [](unsigned short i){ return LuaProxy::Player(i + 1); }, L);
}

luabind::object LuaProxy::Player::getTemplates(lua_State* L)
{
    return LuaHelper::getObjList(5, [](unsigned short i){ return LuaProxy::Player(i + 1001); }, L);
}


LuaProxy::Player::Player() : m_index(1)
{}

LuaProxy::Player::Player(int index) : m_index(index)
{}

int LuaProxy::Player::section(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return (int)::Player::Get(m_index)->CurrentSection;
}

LuaProxy::Section LuaProxy::Player::sectionObj(lua_State *L) const
{
	if(!isValid_throw(L))
		return Section(1);
	return Section((int)::Player::Get(m_index)->CurrentSection);
}

void LuaProxy::Player::kill(lua_State *L)
{
	if(!isValid_throw(L))
		return;
	short tempint = m_index;
	::Player::Kill(&tempint);
}

void LuaProxy::Player::harm(lua_State *L)
{
	if(!isValid_throw(L))
		return;
	short tempint = m_index;
	::Player::Harm(&tempint);
}

RECT LuaProxy::Player::screen(lua_State *L) const
{
	if(!isValid_throw(L))
		return RECT();
	return ::Player::GetScreenPosition(::Player::Get(m_index));
}

double LuaProxy::Player::x(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->momentum.x;
}

void LuaProxy::Player::setX(double x, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->momentum.x = x;
}

double LuaProxy::Player::y(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->momentum.y;
}

void LuaProxy::Player::setY(double y, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->momentum.y = y;
}

double LuaProxy::Player::width(lua_State *L) const
{
    if (!isValid_throw(L))
        return 0;
    return ::Player::Get(m_index)->momentum.width;
}

void LuaProxy::Player::setWidth(double width, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->momentum.width = width;
}

double LuaProxy::Player::height(lua_State *L) const
{
    if (!isValid_throw(L))
        return 0;
    return ::Player::Get(m_index)->momentum.height;
}

void LuaProxy::Player::setHeight(double height, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->momentum.height = height;
}

double LuaProxy::Player::speedX(lua_State *L) const
{
    return ::Player::Get(m_index)->momentum.speedX;
}

void LuaProxy::Player::setSpeedX(double speedX, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->momentum.speedX = speedX;
}

double LuaProxy::Player::speedY(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->momentum.speedY;
}

void LuaProxy::Player::setSpeedY(double speedY, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->momentum.speedY = speedY;
}

int LuaProxy::Player::powerup(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return (int)::Player::Get(m_index)->CurrentPowerup;
}

void LuaProxy::Player::setPowerup(int powerup, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	if(powerup > 0 && powerup < 9)
		::Player::Get(m_index)->CurrentPowerup = powerup;
}

int LuaProxy::Player::reservePowerup(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return (int)::Player::Get(m_index)->PowerupBoxContents;
}

void LuaProxy::Player::setReservePowerup(int reservePowerup, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	if(reservePowerup >= 0)
		::Player::Get(m_index)->PowerupBoxContents = reservePowerup;
}

luabind::object LuaProxy::Player::holdingNPC(lua_State *L) const
{
	if(!isValid_throw(L))
		return luabind::object();
	if(::Player::Get(m_index)->HeldNPCIndex != 0) {
		return luabind::object(L, NPC(::Player::Get(m_index)->HeldNPCIndex-1));
	}

	return luabind::object();
}

bool LuaProxy::Player::upKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.upKeyState;
}

void LuaProxy::Player::setUpKeyPressing(bool upKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.upKeyState = COMBOOL(upKeyPressing);
}

bool LuaProxy::Player::downKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.downKeyState;
}

void LuaProxy::Player::setDownKeyPressing(bool downKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.downKeyState = COMBOOL(downKeyPressing);
}

bool LuaProxy::Player::leftKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.leftKeyState;
}

void LuaProxy::Player::setLeftKeyPressing(bool leftKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.leftKeyState = COMBOOL(leftKeyPressing);
}

bool LuaProxy::Player::rightKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.rightKeyState;
}

void LuaProxy::Player::setRightKeyPressing(bool rightKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.rightKeyState = COMBOOL(rightKeyPressing);
}

bool LuaProxy::Player::jumpKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.jumpKeyState;
}

void LuaProxy::Player::setJumpKeyPressing(bool jumpKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.jumpKeyState = COMBOOL(jumpKeyPressing);
}

bool LuaProxy::Player::altJumpKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.altJumpKeyState;
}

void LuaProxy::Player::setAltJumpKeyPressing(bool altJumpKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.altJumpKeyState = COMBOOL(altJumpKeyPressing);
}

bool LuaProxy::Player::runKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.runKeyState;
}

void LuaProxy::Player::setRunKeyPressing(bool runKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.runKeyState = COMBOOL(runKeyPressing);
}

bool LuaProxy::Player::altRunKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.altRunKeyState;
}

void LuaProxy::Player::setAltRunKeyPressing(bool altRunKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.altRunKeyState = COMBOOL(altRunKeyPressing);
}

bool LuaProxy::Player::dropItemKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.dropItemKeyState;
}

void LuaProxy::Player::setDropItemKeyPressing(bool dropItemKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.dropItemKeyState = COMBOOL(dropItemKeyPressing);
}

bool LuaProxy::Player::pauseKeyPressing(lua_State *L) const
{
    if (!isValid_throw(L))
        return false;
    return (bool)::Player::Get(m_index)->keymap.pauseKeyState;
}

void LuaProxy::Player::setPauseKeyPressing(bool pauseKeyPressing, lua_State *L)
{
    if (!isValid_throw(L))
        return;
    ::Player::Get(m_index)->keymap.pauseKeyState = COMBOOL(pauseKeyPressing);
}



void LuaProxy::Player::mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State *L)
{
	if(!isValid_throw(L))
		return;

	PlayerMOB* pPlayer = ::Player::Get(m_index);
	void* ptr = ((&(*(byte*)pPlayer)) + offset);
	LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::Player::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L) const
{
	if(!isValid_throw(L))
		return luabind::object();

	PlayerMOB* pPlayer = ::Player::Get(m_index);
	void* ptr = ((&(*(byte*)pPlayer)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}

bool LuaProxy::Player::isValid() const
{
	return ::Player::Get(m_index) != 0;
}


bool LuaProxy::Player::isValid_throw(lua_State *L) const
{
	if(!isValid()){
		luaL_error(L, "Invalid Player-Pointer");
		return false;
	}
	return true;
}


/* Generated by code! [START]*/
short LuaProxy::Player::toadDoubleJReady(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->ToadDoubleJReady;
}

void LuaProxy::Player::setToadDoubleJReady(short var_toadDoubleJReady, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->ToadDoubleJReady = var_toadDoubleJReady;
}

short LuaProxy::Player::sparklingEffect(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SparklingEffect;
}

void LuaProxy::Player::setSparklingEffect(short var_sparklingEffect, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SparklingEffect = var_sparklingEffect;
}

short LuaProxy::Player::unknownCTRLLock1(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnknownCTRLLock1;
}

void LuaProxy::Player::setUnknownCTRLLock1(short var_unknownCTRLLock1, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnknownCTRLLock1 = var_unknownCTRLLock1;
}

short LuaProxy::Player::unknownCTRLLock2(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnknownCTRLLock2;
}

void LuaProxy::Player::setUnknownCTRLLock2(short var_unknownCTRLLock2, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnknownCTRLLock2 = var_unknownCTRLLock2;
}

short LuaProxy::Player::quicksandEffectTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->QuicksandEffectTimer;
}

void LuaProxy::Player::setQuicksandEffectTimer(short var_quicksandEffectTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->QuicksandEffectTimer = var_quicksandEffectTimer;
}

short LuaProxy::Player::onSlipperyGround(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->OnSlipperyGround;
}

void LuaProxy::Player::setOnSlipperyGround(short var_onSlipperyGround, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->OnSlipperyGround = var_onSlipperyGround;
}

short LuaProxy::Player::isAFairy(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->IsAFairy;
}

void LuaProxy::Player::setIsAFairy(short var_isAFairy, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->IsAFairy = var_isAFairy;
}

short LuaProxy::Player::fairyAlreadyInvoked(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->FairyAlreadyInvoked;
}

void LuaProxy::Player::setFairyAlreadyInvoked(short var_fairyAlreadyInvoked, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->FairyAlreadyInvoked = var_fairyAlreadyInvoked;
}

short LuaProxy::Player::fairyFramesLeft(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->FairyFramesLeft;
}

void LuaProxy::Player::setFairyFramesLeft(short var_fairyFramesLeft, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->FairyFramesLeft = var_fairyFramesLeft;
}

short LuaProxy::Player::sheathHasKey(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SheathHasKey;
}

void LuaProxy::Player::setSheathHasKey(short var_sheathHasKey, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SheathHasKey = var_sheathHasKey;
}

short LuaProxy::Player::sheathAttackCooldown(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SheathAttackCooldown;
}

void LuaProxy::Player::setSheathAttackCooldown(short var_sheathAttackCooldown, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SheathAttackCooldown = var_sheathAttackCooldown;
}

short LuaProxy::Player::hearts(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Hearts;
}

void LuaProxy::Player::setHearts(short var_hearts, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Hearts = var_hearts;
}

short LuaProxy::Player::peachHoverAvailable(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->PeachHoverAvailable;
}

void LuaProxy::Player::setPeachHoverAvailable(short var_peachHoverAvailable, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->PeachHoverAvailable = var_peachHoverAvailable;
}

short LuaProxy::Player::pressingHoverButton(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->PressingHoverButton;
}

void LuaProxy::Player::setPressingHoverButton(short var_pressingHoverButton, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->PressingHoverButton = var_pressingHoverButton;
}

short LuaProxy::Player::peachHoverTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->PeachHoverTimer;
}

void LuaProxy::Player::setPeachHoverTimer(short var_peachHoverTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->PeachHoverTimer = var_peachHoverTimer;
}

short LuaProxy::Player::unused1(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused1;
}

void LuaProxy::Player::setUnused1(short var_unused1, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused1 = var_unused1;
}

float LuaProxy::Player::peachHoverTrembleSpeed(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->PeachHoverTrembleSpeed;
}

void LuaProxy::Player::setPeachHoverTrembleSpeed(float var_peachHoverTrembleSpeed, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->PeachHoverTrembleSpeed = var_peachHoverTrembleSpeed;
}

short LuaProxy::Player::peachHoverTrembleDir(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->PeachHoverTrembleDir;
}

void LuaProxy::Player::setPeachHoverTrembleDir(short var_peachHoverTrembleDir, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->PeachHoverTrembleDir = var_peachHoverTrembleDir;
}

short LuaProxy::Player::itemPullupTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->ItemPullupTimer;
}

void LuaProxy::Player::setItemPullupTimer(short var_itemPullupTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->ItemPullupTimer = var_itemPullupTimer;
}

float LuaProxy::Player::itemPullupMomentumSave(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->ItemPullupMomentumSave;
}

void LuaProxy::Player::setItemPullupMomentumSave(float var_itemPullupMomentumSave, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->ItemPullupMomentumSave = var_itemPullupMomentumSave;
}

short LuaProxy::Player::unused2(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused2;
}

void LuaProxy::Player::setUnused2(short var_unused2, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused2 = var_unused2;
}

short LuaProxy::Player::unkClimbing1(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnkClimbing1;
}

void LuaProxy::Player::setUnkClimbing1(short var_unkClimbing1, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnkClimbing1 = var_unkClimbing1;
}

short LuaProxy::Player::unkClimbing2(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnkClimbing2;
}

void LuaProxy::Player::setUnkClimbing2(short var_unkClimbing2, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnkClimbing2 = var_unkClimbing2;
}

short LuaProxy::Player::unkClimbing3(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnkClimbing3;
}

void LuaProxy::Player::setUnkClimbing3(short var_unkClimbing3, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnkClimbing3 = var_unkClimbing3;
}

short LuaProxy::Player::waterState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->WaterState;
}

void LuaProxy::Player::setWaterState(short var_waterState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->WaterState = var_waterState;
}

short LuaProxy::Player::isInWater(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->IsInWater;
}

void LuaProxy::Player::setIsInWater(short var_isInWater, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->IsInWater = var_isInWater;
}

short LuaProxy::Player::waterStrokeTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->WaterStrokeTimer;
}

void LuaProxy::Player::setWaterStrokeTimer(short var_waterStrokeTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->WaterStrokeTimer = var_waterStrokeTimer;
}

short LuaProxy::Player::unknownHoverTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnknownHoverTimer;
}

void LuaProxy::Player::setUnknownHoverTimer(short var_unknownHoverTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnknownHoverTimer = var_unknownHoverTimer;
}

short LuaProxy::Player::slidingState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SlidingState;
}

void LuaProxy::Player::setSlidingState(short var_slidingState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SlidingState = var_slidingState;
}

short LuaProxy::Player::slidingGroundPuffs(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SlidingGroundPuffs;
}

void LuaProxy::Player::setSlidingGroundPuffs(short var_slidingGroundPuffs, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SlidingGroundPuffs = var_slidingGroundPuffs;
}

short LuaProxy::Player::climbingState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->ClimbingState;
}

void LuaProxy::Player::setClimbingState(short var_climbingState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->ClimbingState = var_climbingState;
}

short LuaProxy::Player::unknownTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnknownTimer;
}

void LuaProxy::Player::setUnknownTimer(short var_unknownTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnknownTimer = var_unknownTimer;
}

short LuaProxy::Player::unknownFlag(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnknownFlag;
}

void LuaProxy::Player::setUnknownFlag(short var_unknownFlag, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnknownFlag = var_unknownFlag;
}

short LuaProxy::Player::unknownPowerupState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UnknownPowerupState;
}

void LuaProxy::Player::setUnknownPowerupState(short var_unknownPowerupState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UnknownPowerupState = var_unknownPowerupState;
}

short LuaProxy::Player::slopeRelated(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SlopeRelated;
}

void LuaProxy::Player::setSlopeRelated(short var_slopeRelated, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SlopeRelated = var_slopeRelated;
}

short LuaProxy::Player::tanookiStatueActive(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->TanookiStatueActive;
}

void LuaProxy::Player::setTanookiStatueActive(short var_tanookiStatueActive, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->TanookiStatueActive = var_tanookiStatueActive;
}

short LuaProxy::Player::tanookiMorphCooldown(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->TanookiMorphCooldown;
}

void LuaProxy::Player::setTanookiMorphCooldown(short var_tanookiMorphCooldown, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->TanookiMorphCooldown = var_tanookiMorphCooldown;
}

short LuaProxy::Player::tanookiActiveFrameCount(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->TanookiActiveFrameCount;
}

void LuaProxy::Player::setTanookiActiveFrameCount(short var_tanookiActiveFrameCount, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->TanookiActiveFrameCount = var_tanookiActiveFrameCount;
}

short LuaProxy::Player::isSpinjumping(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->IsSpinjumping;
}

void LuaProxy::Player::setIsSpinjumping(short var_isSpinjumping, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->IsSpinjumping = var_isSpinjumping;
}

short LuaProxy::Player::spinjumpStateCounter(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SpinjumpStateCounter;
}

void LuaProxy::Player::setSpinjumpStateCounter(short var_spinjumpStateCounter, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SpinjumpStateCounter = var_spinjumpStateCounter;
}

short LuaProxy::Player::spinjumpLandDirection(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SpinjumpLandDirection;
}

void LuaProxy::Player::setSpinjumpLandDirection(short var_spinjumpLandDirection, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SpinjumpLandDirection = var_spinjumpLandDirection;
}

short LuaProxy::Player::currentKillCombo(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->CurrentKillCombo;
}

void LuaProxy::Player::setCurrentKillCombo(short var_currentKillCombo, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->CurrentKillCombo = var_currentKillCombo;
}

short LuaProxy::Player::groundSlidingPuffsState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->GroundSlidingPuffsState;
}

void LuaProxy::Player::setGroundSlidingPuffsState(short var_groundSlidingPuffsState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->GroundSlidingPuffsState = var_groundSlidingPuffsState;
}

short LuaProxy::Player::warpNearby(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->WarpNearby;
}

void LuaProxy::Player::setWarpNearby(short var_warpNearby, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->WarpNearby = var_warpNearby;
}

short LuaProxy::Player::unknown5C(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown5C;
}

void LuaProxy::Player::setUnknown5C(short var_unknown5C, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown5C = var_unknown5C;
}

short LuaProxy::Player::unknown5E(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown5E;
}

void LuaProxy::Player::setUnknown5E(short var_unknown5E, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown5E = var_unknown5E;
}

short LuaProxy::Player::hasJumped(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->HasJumped;
}

void LuaProxy::Player::setHasJumped(short var_hasJumped, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->HasJumped = var_hasJumped;
}

double LuaProxy::Player::curXPos(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->momentum.x;
}

void LuaProxy::Player::setCurXPos(double var_curXPos, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->momentum.x = var_curXPos;
}

double LuaProxy::Player::curYPos(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->momentum.y;
}

void LuaProxy::Player::setCurYPos(double var_curYPos, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->momentum.y = var_curYPos;
}

double LuaProxy::Player::curXSpeed(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->momentum.speedX;
}

void LuaProxy::Player::setCurXSpeed(double var_curXSpeed, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->momentum.speedX = var_curXSpeed;
}

double LuaProxy::Player::curYSpeed(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->momentum.speedY;
}

void LuaProxy::Player::setCurYSpeed(double var_curYSpeed, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->momentum.speedY = var_curYSpeed;
}

short LuaProxy::Player::identity(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return static_cast<int>(::Player::Get(m_index)->Identity);
}

void LuaProxy::Player::setIdentity(short var_identity, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Identity = static_cast<Characters>(var_identity);
}

short LuaProxy::Player::uKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->keymap.upKeyState;
}

void LuaProxy::Player::setUKeyState(short var_uKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.upKeyState = var_uKeyState;
}

short LuaProxy::Player::dKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.downKeyState;
}

void LuaProxy::Player::setDKeyState(short var_dKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.downKeyState = var_dKeyState;
}

short LuaProxy::Player::lKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.leftKeyState;
}

void LuaProxy::Player::setLKeyState(short var_lKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.leftKeyState = var_lKeyState;
}

short LuaProxy::Player::rKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.rightKeyState;
}

void LuaProxy::Player::setRKeyState(short var_rKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.rightKeyState = var_rKeyState;
}

short LuaProxy::Player::jKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.jumpKeyState;
}

void LuaProxy::Player::setJKeyState(short var_jKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.jumpKeyState = var_jKeyState;
}

short LuaProxy::Player::sJKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.altJumpKeyState;
}

void LuaProxy::Player::setSJKeyState(short var_sJKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.altJumpKeyState = var_sJKeyState;
}

short LuaProxy::Player::xKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.runKeyState;
}

void LuaProxy::Player::setXKeyState(short var_xKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.runKeyState = var_xKeyState;
}

short LuaProxy::Player::rNKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.altRunKeyState;
}

void LuaProxy::Player::setRNKeyState(short var_rNKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.altRunKeyState = var_rNKeyState;
}

short LuaProxy::Player::sELKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.dropItemKeyState;
}

void LuaProxy::Player::setSELKeyState(short var_sELKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.dropItemKeyState = var_sELKeyState;
}

short LuaProxy::Player::sTRKeyState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
    return ::Player::Get(m_index)->keymap.pauseKeyState;
}

void LuaProxy::Player::setSTRKeyState(short var_sTRKeyState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
    ::Player::Get(m_index)->keymap.pauseKeyState = var_sTRKeyState;
}

short LuaProxy::Player::facingDirection(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->FacingDirection;
}

void LuaProxy::Player::setFacingDirection(short var_facingDirection, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->FacingDirection = var_facingDirection;
}

short LuaProxy::Player::mountType(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->MountType;
}

void LuaProxy::Player::setMountType(short var_mountType, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->MountType = var_mountType;
}

short LuaProxy::Player::mountColor(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->MountColor;
}

void LuaProxy::Player::setMountColor(short var_mountColor, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->MountColor = var_mountColor;
}

short LuaProxy::Player::mountState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->MountState;
}

void LuaProxy::Player::setMountState(short var_mountState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->MountState = var_mountState;
}

short LuaProxy::Player::mountHeightOffset(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->MountHeightOffset;
}

void LuaProxy::Player::setMountHeightOffset(short var_mountHeightOffset, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->MountHeightOffset = var_mountHeightOffset;
}

short LuaProxy::Player::mountGfxIndex(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->MountGfxIndex;
}

void LuaProxy::Player::setMountGfxIndex(short var_mountGfxIndex, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->MountGfxIndex = var_mountGfxIndex;
}

short LuaProxy::Player::currentPowerup(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->CurrentPowerup;
}

void LuaProxy::Player::setCurrentPowerup(short var_currentPowerup, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->CurrentPowerup = var_currentPowerup;
}

short LuaProxy::Player::currentPlayerSprite(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->CurrentPlayerSprite;
}

void LuaProxy::Player::setCurrentPlayerSprite(short var_currentPlayerSprite, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->CurrentPlayerSprite = var_currentPlayerSprite;
}

short LuaProxy::Player::unused116(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused116;
}

void LuaProxy::Player::setUnused116(short var_unused116, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused116 = var_unused116;
}

float LuaProxy::Player::gfxMirrorX(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->GfxMirrorX;
}

void LuaProxy::Player::setGfxMirrorX(float var_gfxMirrorX, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->GfxMirrorX = var_gfxMirrorX;
}

short LuaProxy::Player::upwardJumpingForce(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->UpwardJumpingForce;
}

void LuaProxy::Player::setUpwardJumpingForce(short var_upwardJumpingForce, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->UpwardJumpingForce = var_upwardJumpingForce;
}

short LuaProxy::Player::jumpButtonHeld(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->JumpButtonHeld;
}

void LuaProxy::Player::setJumpButtonHeld(short var_jumpButtonHeld, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->JumpButtonHeld = var_jumpButtonHeld;
}

short LuaProxy::Player::spinjumpButtonHeld(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SpinjumpButtonHeld;
}

void LuaProxy::Player::setSpinjumpButtonHeld(short var_spinjumpButtonHeld, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SpinjumpButtonHeld = var_spinjumpButtonHeld;
}

short LuaProxy::Player::forcedAnimationState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->ForcedAnimationState;
}

void LuaProxy::Player::setForcedAnimationState(short var_forcedAnimationState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->ForcedAnimationState = var_forcedAnimationState;
}

float LuaProxy::Player::unknown124(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown124;
}

void LuaProxy::Player::setUnknown124(float var_unknown124, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown124 = var_unknown124;
}

float LuaProxy::Player::unknown128(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown128;
}

void LuaProxy::Player::setUnknown128(float var_unknown128, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown128 = var_unknown128;
}

short LuaProxy::Player::downButtonMirror(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->DownButtonMirror;
}

void LuaProxy::Player::setDownButtonMirror(short var_downButtonMirror, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->DownButtonMirror = var_downButtonMirror;
}

short LuaProxy::Player::inDuckingPosition(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->InDuckingPosition;
}

void LuaProxy::Player::setInDuckingPosition(short var_inDuckingPosition, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->InDuckingPosition = var_inDuckingPosition;
}

short LuaProxy::Player::selectButtonMirror(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->SelectButtonMirror;
}

void LuaProxy::Player::setSelectButtonMirror(short var_selectButtonMirror, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->SelectButtonMirror = var_selectButtonMirror;
}

short LuaProxy::Player::unknown132(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown132;
}

void LuaProxy::Player::setUnknown132(short var_unknown132, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown132 = var_unknown132;
}

short LuaProxy::Player::downButtonTapped(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->DownButtonTapped;
}

void LuaProxy::Player::setDownButtonTapped(short var_downButtonTapped, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->DownButtonTapped = var_downButtonTapped;
}

short LuaProxy::Player::unknown136(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown136;
}

void LuaProxy::Player::setUnknown136(short var_unknown136, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown136 = var_unknown136;
}

float LuaProxy::Player::xMomentumPush(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->XMomentumPush;
}

void LuaProxy::Player::setXMomentumPush(float var_xMomentumPush, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->XMomentumPush = var_xMomentumPush;
}

short LuaProxy::Player::deathState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->DeathState;
}

void LuaProxy::Player::setDeathState(short var_deathState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->DeathState = var_deathState;
}

short LuaProxy::Player::deathTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->DeathTimer;
}

void LuaProxy::Player::setDeathTimer(short var_deathTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->DeathTimer = var_deathTimer;
}

short LuaProxy::Player::blinkTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->BlinkTimer;
}

void LuaProxy::Player::setBlinkTimer(short var_blinkTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->BlinkTimer = var_blinkTimer;
}

short LuaProxy::Player::blinkState(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->BlinkState;
}

void LuaProxy::Player::setBlinkState(short var_blinkState, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->BlinkState = var_blinkState;
}

short LuaProxy::Player::unknown144(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown144;
}

void LuaProxy::Player::setUnknown144(short var_unknown144, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown144 = var_unknown144;
}

short LuaProxy::Player::layerStateStanding(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->LayerStateStanding;
}

void LuaProxy::Player::setLayerStateStanding(short var_layerStateStanding, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->LayerStateStanding = var_layerStateStanding;
}

short LuaProxy::Player::layerStateLeftContact(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->LayerStateLeftContact;
}

void LuaProxy::Player::setLayerStateLeftContact(short var_layerStateLeftContact, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->LayerStateLeftContact = var_layerStateLeftContact;
}

short LuaProxy::Player::layerStateTopContact(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->LayerStateTopContact;
}

void LuaProxy::Player::setLayerStateTopContact(short var_layerStateTopContact, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->LayerStateTopContact = var_layerStateTopContact;
}

short LuaProxy::Player::layerStateRightContact(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->LayerStateRightContact;
}

void LuaProxy::Player::setLayerStateRightContact(short var_layerStateRightContact, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->LayerStateRightContact = var_layerStateRightContact;
}

short LuaProxy::Player::pushedByMovingLayer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->PushedByMovingLayer;
}

void LuaProxy::Player::setPushedByMovingLayer(short var_pushedByMovingLayer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->PushedByMovingLayer = var_pushedByMovingLayer;
}

short LuaProxy::Player::unused150(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused150;
}

void LuaProxy::Player::setUnused150(short var_unused150, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused150 = var_unused150;
}

short LuaProxy::Player::unused152(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused152;
}

void LuaProxy::Player::setUnused152(short var_unused152, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused152 = var_unused152;
}

short LuaProxy::Player::heldNPCIndex(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->HeldNPCIndex;
}

void LuaProxy::Player::setHeldNPCIndex(short var_heldNPCIndex, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->HeldNPCIndex = var_heldNPCIndex;
}

short LuaProxy::Player::unknown156(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown156;
}

void LuaProxy::Player::setUnknown156(short var_unknown156, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown156 = var_unknown156;
}

short LuaProxy::Player::powerupBoxContents(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->PowerupBoxContents;
}

void LuaProxy::Player::setPowerupBoxContents(short var_powerupBoxContents, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->PowerupBoxContents = var_powerupBoxContents;
}

short LuaProxy::Player::currentSection(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->CurrentSection;
}

void LuaProxy::Player::setCurrentSection(short var_currentSection, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->CurrentSection = var_currentSection;
}

short LuaProxy::Player::warpTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->WarpTimer;
}

void LuaProxy::Player::setWarpTimer(short var_warpTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->WarpTimer = var_warpTimer;
}

short LuaProxy::Player::unknown15E(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown15E;
}

void LuaProxy::Player::setUnknown15E(short var_unknown15E, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown15E = var_unknown15E;
}

short LuaProxy::Player::projectileTimer1(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->ProjectileTimer1;
}

void LuaProxy::Player::setProjectileTimer1(short var_projectileTimer1, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->ProjectileTimer1 = var_projectileTimer1;
}

short LuaProxy::Player::projectileTimer2(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->ProjectileTimer2;
}

void LuaProxy::Player::setProjectileTimer2(short var_projectileTimer2, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->ProjectileTimer2 = var_projectileTimer2;
}

short LuaProxy::Player::tailswipeTimer(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->TailswipeTimer;
}

void LuaProxy::Player::setTailswipeTimer(short var_tailswipeTimer, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->TailswipeTimer = var_tailswipeTimer;
}

short LuaProxy::Player::unknown166(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown166;
}

void LuaProxy::Player::setUnknown166(short var_unknown166, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown166 = var_unknown166;
}

float LuaProxy::Player::takeoffSpeed(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->TakeoffSpeed;
}

void LuaProxy::Player::setTakeoffSpeed(float var_takeoffSpeed, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->TakeoffSpeed = var_takeoffSpeed;
}

short LuaProxy::Player::canFly(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->CanFly;
}

void LuaProxy::Player::setCanFly(short var_canFly, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->CanFly = var_canFly;
}

short LuaProxy::Player::isFlying(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->IsFlying;
}

void LuaProxy::Player::setIsFlying(short var_isFlying, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->IsFlying = var_isFlying;
}

short LuaProxy::Player::flightTimeRemaining(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->FlightTimeRemaining;
}

void LuaProxy::Player::setFlightTimeRemaining(short var_flightTimeRemaining, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->FlightTimeRemaining = var_flightTimeRemaining;
}

short LuaProxy::Player::holdingFlightRunButton(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->HoldingFlightRunButton;
}

void LuaProxy::Player::setHoldingFlightRunButton(short var_holdingFlightRunButton, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->HoldingFlightRunButton = var_holdingFlightRunButton;
}

short LuaProxy::Player::holdingFlightButton(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->HoldingFlightButton;
}

void LuaProxy::Player::setHoldingFlightButton(short var_holdingFlightButton, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->HoldingFlightButton = var_holdingFlightButton;
}

short LuaProxy::Player::nPCBeingStoodOnIndex(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->NPCBeingStoodOnIndex;
}

void LuaProxy::Player::setNPCBeingStoodOnIndex(short var_nPCBeingStoodOnIndex, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->NPCBeingStoodOnIndex = var_nPCBeingStoodOnIndex;
}

short LuaProxy::Player::unknown178(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown178;
}

void LuaProxy::Player::setUnknown178(short var_unknown178, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown178 = var_unknown178;
}

short LuaProxy::Player::unknown17A(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unknown17A;
}

void LuaProxy::Player::setUnknown17A(short var_unknown17A, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unknown17A = var_unknown17A;
}

short LuaProxy::Player::unused17C(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused17C;
}

void LuaProxy::Player::setUnused17C(short var_unused17C, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused17C = var_unused17C;
}

short LuaProxy::Player::unused17E(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused17E;
}

void LuaProxy::Player::setUnused17E(short var_unused17E, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused17E = var_unused17E;
}

short LuaProxy::Player::unused180(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused180;
}

void LuaProxy::Player::setUnused180(short var_unused180, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused180 = var_unused180;
}

short LuaProxy::Player::unused182(lua_State *L) const
{
	if(!isValid_throw(L))
		return 0;
	return ::Player::Get(m_index)->Unused182;
}

void LuaProxy::Player::setUnused182(short var_unused182, lua_State *L)
{
	if(!isValid_throw(L))
		return;
	::Player::Get(m_index)->Unused182 = var_unused182;
}

int LuaProxy::Player::getIndex() const
{
	return m_index;
}
/* Generated by code! [END]*/