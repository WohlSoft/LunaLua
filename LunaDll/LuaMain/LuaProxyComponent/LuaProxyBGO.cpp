#include "../../Defines.h"
#include "../LuaProxy.h"
#include "../../SMBXInternal/BGOs.h"

unsigned short LuaProxy::BGO::count()
{
    return GM_BGO_COUNT;
}

luabind::object LuaProxy::BGO::get(lua_State* L)
{
    return get(luabind::object(L, -1), L);
}

// TODO: Consider if there's a good way to use C++ templates to make it so
//       entity id filtering code isn't duplicated.
luabind::object LuaProxy::BGO::get(luabind::object idFilter, lua_State* L)
{
    bool lookupTableBgoID[191];

    memset(&lookupTableBgoID, false, sizeof(lookupTableBgoID));

    if (luabind::type(idFilter) == LUA_TNUMBER){
        int theBgoID = luabind::object_cast<int>(idFilter);

        if (theBgoID == -1){
            memset(&lookupTableBgoID, true, sizeof(lookupTableBgoID));
        }
        else{
            if (theBgoID < 1 || theBgoID > 190){
                luaL_error(L, "Invalid BGO-ID!\nNeed BGO-ID between 1-190\nGot BGO-ID: %d", theBgoID);
                return luabind::object();
            }
            lookupTableBgoID[theBgoID] = true;
        }
    }
    else if (luabind::type(idFilter) == LUA_TTABLE){
        for (luabind::iterator i(idFilter), end; i != end; ++i)
        {
            int theBgoID = luabind::object_cast<int>((luabind::object)*i);
            if (theBgoID < 1 || theBgoID > 190){
                luaL_error(L, "Invalid BGO-ID!\nNeed BGO-ID between 1-190\nGot BGO-ID: %d", theBgoID);
                return luabind::object();
            }
            lookupTableBgoID[theBgoID] = true;
        }
    }
    else{
        const char* invalidBgoIDType = lua_typename(L, luabind::type(idFilter));
        luaL_error(L, "Invalid args for bgoID (arg #1, expected table or number, got %s)", invalidBgoIDType);
    }

    luabind::object retBGOs = luabind::newtable(L);
    int bgoIndex = 1;

    for (int i = 0; i < GM_BGO_COUNT; i++) {
        SMBX_BGO* thisnpc = ::SMBX_BGO::Get(i);
        if (thisnpc != NULL && lookupTableBgoID[thisnpc->id]) {
            retBGOs[bgoIndex++] = LuaProxy::BGO(i);
        }
    }

    return retBGOs;
}

LuaProxy::BGO::BGO(unsigned short index)
{
    m_index = index;
}

bool LuaProxy::BGO::isValid() const
{
    return (m_index < GM_BGO_COUNT);
}


bool LuaProxy::BGO::isValid_throw(lua_State *L) const
{
    if (!isValid()){
        luaL_error(L, "Invalid BGO-Pointer");
        return false;
    }
    return true;
}
