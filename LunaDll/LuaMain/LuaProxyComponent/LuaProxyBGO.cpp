#include "../LuaProxy.h"
#include "../../SMBXInternal/BGOs.h"

unsigned short LuaProxy::BGO::count()
{
    return ::SMBX_BGO::Count();
}

luabind::object LuaProxy::BGO::get(lua_State* L)
{
    return LuaHelper::getObjList(::SMBX_BGO::Count(), [](unsigned short i){ return LuaProxy::BGO(i); }, L);
}

// TODO: Consider if there's a good way to use C++ templates to make it so
//       entity id filtering code isn't duplicated.
luabind::object LuaProxy::BGO::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableBgoID;

    try
    {
        lookupTableBgoID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::SMBX_BGO::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid BGO-ID!\nNeed BGO-ID between 1-%d\nGot BGO-ID: %d", ::SMBX_BGO::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for bgoID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }
    
    return LuaHelper::getObjList(
        ::SMBX_BGO::Count(),
        [](unsigned short i){ return LuaProxy::BGO(i); },
        [&lookupTableBgoID](unsigned short i){
            ::SMBX_BGO *bgo = ::SMBX_BGO::Get(i);
            return (bgo != NULL) &&
                   (bgo->id <= ::SMBX_BGO::MAX_ID) && lookupTableBgoID.get()[bgo->id];
        }, L);
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
