#include "../LuaProxy.h"
#include "../../SMBXInternal/BGOs.h"

unsigned short LuaProxy::BGO::count()
{
    return ::SMBX_BGO::Count();
}

luabind::object LuaProxy::BGO::get(lua_State* L)
{
    return get(luabind::object(L, -1), L);
}

// TODO: Consider if there's a good way to use C++ templates to make it so
//       entity id filtering code isn't duplicated.
luabind::object LuaProxy::BGO::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableBgoID;

    try
    {
        lookupTableBgoID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, 292));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid BGO-ID!\nNeed BGO-ID between 1-190\nGot BGO-ID: %d", e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for npcID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    luabind::object retBGOs = luabind::newtable(L);
    int bgoIndex = 1;

    for (int i = 0; i < ::SMBX_BGO::Count(); i++) {
        SMBX_BGO* thisnpc = ::SMBX_BGO::Get(i);
        if (thisnpc != NULL && lookupTableBgoID.get()[thisnpc->id]) {
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
    return (m_index < ::SMBX_BGO::Count());
}


bool LuaProxy::BGO::isValid_throw(lua_State *L) const
{
    if (!isValid()){
        luaL_error(L, "Invalid BGO-Pointer");
        return false;
    }
    return true;
}
