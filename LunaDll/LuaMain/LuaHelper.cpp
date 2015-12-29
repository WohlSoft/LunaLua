#include "LuaHelper.h"
#include "LunaLuaMain.h"
#include "../Misc/VB6StrPtr.h"
#include "LuaProxy.h"

luabind::object LuaHelper::getEventCallbase(lua_State *base, std::string eventTable)
{
    luabind::object _G = luabind::globals(base);
    return _G[eventTable.c_str()];
}


bool LuaHelper::is_function(lua_State *luaState, const char *fname)
{
    luabind::object g = luabind::globals(luaState);
    luabind::object fun = g[fname];
    if (fun.is_valid()) {
        if (luabind::type(fun)==LUA_TFUNCTION) {
            return true;
        }
    }
    return false;
}

void LuaHelper::assignVB6StrPtr(VB6StrPtr* ptr, const luabind::object &value, lua_State* L)
{
    // Copy from native Lua string
    boost::optional<std::string const> opt_str = luabind::object_cast_nothrow<std::string const>(value);
    if (opt_str != boost::none)
    {
        *ptr = *opt_str;
        return;
    }

    // Copy from VBStr object
    boost::optional<LuaProxy::VBStr const> opt_obj = luabind::object_cast_nothrow<LuaProxy::VBStr const>(value);
    if (opt_obj != boost::none)
    {
        *ptr = *((VB6StrPtr*)&(*opt_obj).m_wcharptr);
        return;
    }

    luaL_error(L, "Cannot cast to string");
}

bool* LuaHelper::generateFilterTable(lua_State* L, luabind::object theFilter, int maxVal, int minVal /*= 1*/)
{
    int mallocVal = maxVal + 1;
    bool* filterTable = (bool*)calloc(mallocVal, sizeof(bool));
    if (luabind::type(theFilter) == LUA_TNUMBER){
        int theID = luabind::object_cast<int>(theFilter);
        if (theID == -1){
            memset((void*)filterTable, 0xFF, mallocVal*sizeof(bool));
        }
        else{
            if (theID < minVal || theID > maxVal){
                throw new LuaHelper::invalidIDException(theID);
            }
            filterTable[theID] = true;
        }
    }
    else if (luabind::type(theFilter) == LUA_TTABLE){
        for (luabind::iterator i(theFilter), end; i != end; ++i)
        {
            int theID = luabind::object_cast<int>((luabind::object)*i);
            if (theID < minVal || theID > maxVal){
                throw new LuaHelper::invalidIDException(theID);
            }
            filterTable[theID] = true;
        }
    }
    else{
        throw new LuaHelper::invalidTypeException();
    }
    return filterTable;
}

