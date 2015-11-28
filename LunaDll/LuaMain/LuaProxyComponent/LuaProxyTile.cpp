#include "../LuaProxy.h"
#include "../../SMBXInternal/Tile.h"

unsigned short LuaProxy::Tile::count()
{
    return ::SMBXTile::Count();
}

luabind::object LuaProxy::Tile::get(lua_State* L)
{
    return LuaHelper::getObjList(::SMBXTile::Count(), [](unsigned short i) { return LuaProxy::Tile(i); }, L);
}

// TODO: Consider if there's a good way to use C++ templates to make it so
//       entity id filtering code isn't duplicated.
luabind::object LuaProxy::Tile::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableTileID;

    try
    {
        lookupTableTileID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::SMBXTile::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid Tile-ID!\nNeed Tile-ID between 1-%d\nGot Tile-ID: %d", ::SMBXTile::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for bgoID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        ::SMBXTile::Count(),
        [](unsigned short i) { return LuaProxy::Tile(i); },
        [&lookupTableTileID](unsigned short i) {
        ::SMBXTile *bgo = ::SMBXTile::Get(i);
        return (bgo != NULL) &&
            (bgo->id <= ::SMBXTile::MAX_ID) && lookupTableTileID.get()[bgo->id];
    }, L);
}

LuaProxy::Tile::Tile(unsigned short index)
{
    m_index = index;
}

luabind::object LuaProxy::Tile::getIntersecting(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBXTile::Count(),
        [](unsigned short i) { return LuaProxy::Tile(i); },
        [x1, y1, x2, y2](unsigned short i) {
        ::SMBXTile *obj = ::SMBXTile::Get(i);
        if (obj == NULL) return false;
        if (x2 <= obj->momentum.x) return false;
        if (y2 <= obj->momentum.y) return false;
        if (obj->momentum.x + obj->momentum.width <= x1) return false;
        if (obj->momentum.y + obj->momentum.height <= y1) return false;
        return true;
    }, L);
}

short LuaProxy::Tile::id(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXTile::Get(m_index)->id;
}

void LuaProxy::Tile::setId(short id, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXTile::Get(m_index)->id = id;
}

double LuaProxy::Tile::x(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXTile::Get(m_index)->momentum.x;
}

void LuaProxy::Tile::setX(double x, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBXTile::Get(m_index)->momentum.x = x;
}

double LuaProxy::Tile::y(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXTile::Get(m_index)->momentum.y;
}

void LuaProxy::Tile::setY(double y, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBXTile::Get(m_index)->momentum.y = y;
}

double LuaProxy::Tile::width(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXTile::Get(m_index)->momentum.width;
}

void LuaProxy::Tile::setWidth(double width, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXTile::Get(m_index)->momentum.width = width;
}

double LuaProxy::Tile::height(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXTile::Get(m_index)->momentum.height;
}

void LuaProxy::Tile::setHeight(double height, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXTile::Get(m_index)->momentum.height = height;
}

double LuaProxy::Tile::speedX(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXTile::Get(m_index)->momentum.speedX;
}

void LuaProxy::Tile::setSpeedX(double speedX, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXTile::Get(m_index)->momentum.speedX = speedX;
}

double LuaProxy::Tile::speedY(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXTile::Get(m_index)->momentum.speedY;
}

void LuaProxy::Tile::setSpeedY(double speedY, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXTile::Get(m_index)->momentum.speedY = speedY;
}

bool LuaProxy::Tile::isValid() const
{
    return (m_index < ::SMBXTile::Count());
}

bool LuaProxy::Tile::isValid_throw(lua_State *L) const
{
    if (!isValid()) {
        luaL_error(L, "Invalid Tile-Pointer");
        return false;
    }
    return true;
}
