#include "../LuaProxy.h"
#include "../../SMBXInternal/CustomGraphics.h"
#include "../../CharacterData.h"

LuaProxy::PlayerSettings LuaProxy::PlayerSettings::get(Characters character, PowerupID powerupID, lua_State* L)
{
    PlayerSettings plSettings = PlayerSettings(character, powerupID);
    plSettings.isValid_throw(L);
    return plSettings;
}

int LuaProxy::PlayerSettings::getHitboxWidth(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerHitboxWidth(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setHitboxWidth(int width, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerHitboxWidth(m_powerupID, m_character, width);
}

int LuaProxy::PlayerSettings::getHitboxHeight(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerHitboxHeight(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setHitboxHeight(int height, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerHitboxHeight(m_powerupID, m_character, height);
}

int LuaProxy::PlayerSettings::getHitboxDuckHeight(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerHitboxDuckHeight(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setHitboxDuckHeight(int duckHeight, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerHitboxDuckHeight(m_powerupID, m_character, duckHeight);
}

int LuaProxy::PlayerSettings::getGrabOffsetX(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerGrabOffsetX(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setGrabOffsetX(int grabOffsetX, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerGrabOffsetX(m_powerupID, m_character, grabOffsetX);
}

int LuaProxy::PlayerSettings::getGrabOffsetY(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_CustomGraphics::getPlayerGrabOffsetY(m_powerupID, m_character);
}

void LuaProxy::PlayerSettings::setGrabOffsetY(int grabOffsetY, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_CustomGraphics::setPlayerGrabOffsetY(m_powerupID, m_character, grabOffsetY);
}

int LuaProxy::PlayerSettings::getSpriteOffsetX(int indexX, int indexY, lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    int errCode = 0;
    int result = SMBX_CustomGraphics::getOffsetX(m_character, SMBX_CustomGraphics::convIndexCoorToSpriteIndex(indexX, indexY), m_powerupID, &errCode);
    
    if (errCode == -1 || indexX < 0 || indexY < 0 || indexX > 9 || indexY > 9) {
        luaL_error(L, "indexX (%d) with indexY (%d) are invalid coordinate indexes", indexX, indexY);
        return 0;
    }

    return result;
}

void LuaProxy::PlayerSettings::setSpriteOffsetX(int indexX, int indexY, int value, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    int errCode = 0;
    SMBX_CustomGraphics::setOffsetX(m_character, SMBX_CustomGraphics::convIndexCoorToSpriteIndex(indexX, indexY), m_powerupID, value, &errCode);

    if (errCode == -1 || indexX < 0 || indexY < 0 || indexX > 9 || indexY > 9) {
        luaL_error(L, "indexX (%d) with indexY (%d) are invalid coordinate indexes", indexX, indexY);
        return;
    }
}

int LuaProxy::PlayerSettings::getSpriteOffsetY(int indexX, int indexY, lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    int errCode = 0;
    int result = SMBX_CustomGraphics::getOffsetY(m_character, SMBX_CustomGraphics::convIndexCoorToSpriteIndex(indexX, indexY), m_powerupID, &errCode);

    if (errCode == -1 || indexX < 0 || indexY < 0 || indexX > 9 || indexY > 9) {
        luaL_error(L, "indexX (%d) with indexY (%d) are invalid coordinate indexes", indexX, indexY);
        return 0;
    }

    return result;
}

void LuaProxy::PlayerSettings::setSpriteOffsetY(int indexX, int indexY, int value, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    int errCode = 0;
    SMBX_CustomGraphics::setOffsetY(m_character, SMBX_CustomGraphics::convIndexCoorToSpriteIndex(indexX, indexY), m_powerupID, value, &errCode);

    if (errCode == -1 || indexX < 0 || indexY < 0 || indexX > 9 || indexY > 9) {
        luaL_error(L, "indexX (%d) with indexY (%d) are invalid coordinate indexes", indexX, indexY);
        return;
    }
}

PowerupID LuaProxy::PlayerSettings::getPowerupID(lua_State* L) const
{
    if (!isValid_throw(L))
        return PLAYER_SMALL;
    return m_powerupID;
}

void LuaProxy::PlayerSettings::setPowerupID(PowerupID val, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    m_powerupID = val;
    isValid_throw(L);
}

Characters LuaProxy::PlayerSettings::getCharacter(lua_State* L) const
{
    if (!isValid_throw(L))
        return CHARACTER_MARIO;
    return m_character;
}

void LuaProxy::PlayerSettings::setCharacter(Characters val, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    m_character = val;
    isValid_throw(L);
}


bool LuaProxy::PlayerSettings::isValid_throw(lua_State* L) const
{
    if (runtimeHookGetExtCharacterHitBoxData(m_character, m_powerupID) != nullptr)
    {
        return true;
    }
    if ((int)m_character < CHARACTER_MARIO || (int)m_character > CHARACTER_LINK){
        luaL_error(L, "Invalid character id %d!", m_character);
        return false;
    }
    if ((int)m_powerupID < 1 || (int)m_powerupID > 10){
        luaL_error(L, "Invalid powerup id %d!", m_powerupID);
        return false;
    }
    return true;
}

// PlayerSettingsCharacterProperties
#define _DEFINE_CHARACTERPROPERTIES(TYPE, GET, SET, NATIVE_GET, NATIVE_SET) \
    TYPE LuaProxy::PlayerSettingsCharacterProperties::GET(lua_State* L) { return NATIVE_GET(m_character); } \
    void LuaProxy::PlayerSettingsCharacterProperties::SET(TYPE value, lua_State* L) { NATIVE_SET(m_character, value); }
_DEFINE_CHARACTERPROPERTIES(bool, getCanSlide,     setCanSlide,     ExtraCharacterData::canSlideGet,     ExtraCharacterData::canSlideSet    );
_DEFINE_CHARACTERPROPERTIES(bool, getCanRideYoshi, setCanRideYoshi, ExtraCharacterData::canRideYoshiGet, ExtraCharacterData::canRideYoshiSet);
_DEFINE_CHARACTERPROPERTIES(bool, getCanRideBoot,  setCanRideBoot,  ExtraCharacterData::canRideBootGet,  ExtraCharacterData::canRideBootSet );
_DEFINE_CHARACTERPROPERTIES(bool, getCanSpinJump,  setCanSpinJump,  ExtraCharacterData::canSpinJumpGet,  ExtraCharacterData::canSpinJumpSet );
#undef _DEFINE_CHARACTERPROPERTIES

LuaProxy::PlayerSettingsCharacterProperties LuaProxy::PlayerSettings::getCharacterProperties(Characters character, lua_State* L)
{
    return PlayerSettingsCharacterProperties(character);
}
