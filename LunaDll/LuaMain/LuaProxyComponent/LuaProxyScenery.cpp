#include "../LuaProxy.h"
#include "../../SMBXInternal/Scenery.h"

unsigned short LuaProxy::Scenery::count()
{
    return ::SMBXScenery::Count();
}

luabind::object LuaProxy::Scenery::get(lua_State* L)
{
    return LuaHelper::getObjList(::SMBXScenery::Count(), [](unsigned short i) { return LuaProxy::Scenery(i); }, L);
}

// TODO: Consider if there's a good way to use C++ templates to make it so
//       entity id filtering code isn't duplicated.
luabind::object LuaProxy::Scenery::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableSceneryID;

    try
    {
        lookupTableSceneryID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::SMBXScenery::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid Scenery-ID!\nNeed Scenery-ID between 1-%d\nGot Scenery-ID: %d", ::SMBXScenery::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for sceneryID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        ::SMBXScenery::Count(),
        [](unsigned short i) { return LuaProxy::Scenery(i); },
        [&lookupTableSceneryID](unsigned short i) {
        ::SMBXScenery *bgo = ::SMBXScenery::Get(i);
        return (bgo != NULL) &&
            (bgo->id <= ::SMBXScenery::MAX_ID) && lookupTableSceneryID.get()[bgo->id];
    }, L);
}

LuaProxy::Scenery::Scenery(unsigned short index)
{
    m_index = index;
}

int LuaProxy::Scenery::idx() const
{
    return m_index;
}

luabind::object LuaProxy::Scenery::getIntersecting(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBXScenery::Count(),
        [](unsigned short i) { return LuaProxy::Scenery(i); },
        [x1, y1, x2, y2](unsigned short i) {
        ::SMBXScenery *obj = ::SMBXScenery::Get(i);
        if (obj == NULL) return false;
        if (x2 <= obj->momentum.x) return false;
        if (y2 <= obj->momentum.y) return false;
        if (obj->momentum.x + obj->momentum.width <= x1) return false;
        if (obj->momentum.y + obj->momentum.height <= y1) return false;
        return true;
    }, L);
}

short LuaProxy::Scenery::id(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXScenery::Get(m_index)->id;
}

void LuaProxy::Scenery::setId(short id, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXScenery::Get(m_index)->id = id;
}

double LuaProxy::Scenery::x(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXScenery::Get(m_index)->momentum.x;
}

void LuaProxy::Scenery::setX(double x, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBXScenery::Get(m_index)->momentum.x = x;
}

double LuaProxy::Scenery::y(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXScenery::Get(m_index)->momentum.y;
}

void LuaProxy::Scenery::setY(double y, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBXScenery::Get(m_index)->momentum.y = y;
}

double LuaProxy::Scenery::width(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXScenery::Get(m_index)->momentum.width;
}

void LuaProxy::Scenery::setWidth(double width, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXScenery::Get(m_index)->momentum.width = width;
}

double LuaProxy::Scenery::height(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXScenery::Get(m_index)->momentum.height;
}

void LuaProxy::Scenery::setHeight(double height, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXScenery::Get(m_index)->momentum.height = height;
}

bool LuaProxy::Scenery::visible(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return 0 != SMBXScenery::Get(m_index)->visible;
}

void LuaProxy::Scenery::setVisible(bool visible, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXScenery::Get(m_index)->visible = COMBOOL(visible);
}

bool LuaProxy::Scenery::isValid() const
{
    return (m_index < ::SMBXScenery::Count());
}

bool LuaProxy::Scenery::isValid_throw(lua_State *L) const
{
    if (!isValid()) {
        luaL_error(L, "Invalid Scenery-Pointer");
        return false;
    }
    return true;
}
