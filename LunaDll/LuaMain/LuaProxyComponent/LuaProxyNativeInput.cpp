#include "../LuaProxy.h"

#include "../../SMBXInternal/NativeInput.h"
#include "../../Defines.h"

#pragma warning(disable: 4800)



LuaProxy::InputConfig::InputConfig(unsigned short index) : m_index(index)
{}

int LuaProxy::InputConfig::idx() const
{
    return m_index;
}

short LuaProxy::InputConfig::inputType(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    else
        return SMBXInput::getPlayerInputType(m_index);
}

void LuaProxy::InputConfig::setInputType(short inputType, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXInput::setPlayerInputType(m_index, inputType);
}


short LuaProxy::InputConfig::up(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->up;
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
    return 0;
}

short LuaProxy::InputConfig::down(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->down;
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
    return 0;
}

short LuaProxy::InputConfig::left(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->left;
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
    return 0;
}

short LuaProxy::InputConfig::right(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->right;
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
    return 0;
}

short LuaProxy::InputConfig::run(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->run;
    else
        return SMBXNativeJoystick::Get(m_index)->run;
}

short LuaProxy::InputConfig::altrun(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->altrun;
    else
        return SMBXNativeJoystick::Get(m_index)->altrun;
}

short LuaProxy::InputConfig::jump(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->jump;
    else
        return SMBXNativeJoystick::Get(m_index)->jump;
}

short LuaProxy::InputConfig::altjump(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->altjump;
    else
        return SMBXNativeJoystick::Get(m_index)->altjump;
}

short LuaProxy::InputConfig::dropitem(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->dropitem;
    else
        return SMBXNativeJoystick::Get(m_index)->dropitem;
}

short LuaProxy::InputConfig::pause(lua_State* L)
{
    if (!isValid_throw(L))
        return 0;
    if (inputType(L) == 0)
        return SMBXNativeKeyboard::Get(m_index)->pause;
    else
        return SMBXNativeJoystick::Get(m_index)->pause;
}


void LuaProxy::InputConfig::setUp(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0){
        SMBXNativeKeyboard::Get(m_index)->up = keycode;
        return;
    }
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
}


void LuaProxy::InputConfig::setDown(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0){
        SMBXNativeKeyboard::Get(m_index)->down = keycode;
        return;
    }
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
}

void LuaProxy::InputConfig::setLeft(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0){
        SMBXNativeKeyboard::Get(m_index)->left = keycode;
        return;
    }
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
}

void LuaProxy::InputConfig::setRight(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0){
        SMBXNativeKeyboard::Get(m_index)->right = keycode;
        return;
    }
    luaL_error(L, "You cannot modify or get movement controls from an joystick. Please check type with .inputType!");
}

void LuaProxy::InputConfig::setRun(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0)
        SMBXNativeKeyboard::Get(m_index)->run = keycode;
    else
        SMBXNativeJoystick::Get(m_index)->run = keycode;
}

void LuaProxy::InputConfig::setAltRun(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0)
        SMBXNativeKeyboard::Get(m_index)->altrun = keycode;
    else
        SMBXNativeJoystick::Get(m_index)->altrun = keycode;
}

void LuaProxy::InputConfig::setJump(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0)
        SMBXNativeKeyboard::Get(m_index)->jump = keycode;
    else
        SMBXNativeJoystick::Get(m_index)->jump = keycode;
}

void LuaProxy::InputConfig::setAltJump(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0)
        SMBXNativeKeyboard::Get(m_index)->altjump = keycode;
    else
        SMBXNativeJoystick::Get(m_index)->altjump = keycode;
}

void LuaProxy::InputConfig::setDropItem(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0)
        SMBXNativeKeyboard::Get(m_index)->dropitem = keycode;
    else
        SMBXNativeJoystick::Get(m_index)->dropitem = keycode;
}

void LuaProxy::InputConfig::setPause(unsigned short keycode, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    if (inputType(L) == 0)
        SMBXNativeKeyboard::Get(m_index)->pause = keycode;
    else
        SMBXNativeJoystick::Get(m_index)->pause = keycode;
}


bool LuaProxy::InputConfig::isValid() const
{
    return (m_index == 1 || m_index == 2);
}


bool LuaProxy::InputConfig::isValid_throw(lua_State *L) const
{
    if (!isValid()){
        luaL_error(L, "Invalid KeyboardConfig-Pointer");
        return false;
    }
    return true;
}
