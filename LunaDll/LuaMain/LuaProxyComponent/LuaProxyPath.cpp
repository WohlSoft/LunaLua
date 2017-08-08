#include "../LuaProxy.h"
#include "../../SMBXInternal/Path.h"

unsigned short LuaProxy::Path::count()
{
    return ::SMBXPath::Count();
}

luabind::object LuaProxy::Path::get(lua_State* L)
{
    return LuaHelper::getObjList(::SMBXPath::Count(), [](unsigned short i) { return LuaProxy::Path(i); }, L);
}

// TODO: Consider if there's a good way to use C++ templates to make it so
//       entity id filtering code isn't duplicated.
luabind::object LuaProxy::Path::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTablePathID;

    try
    {
        lookupTablePathID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::SMBXPath::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid Path-ID!\nNeed Path-ID between 1-%d\nGot Path-ID: %d", ::SMBXPath::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for pathID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        ::SMBXPath::Count(),
        [](unsigned short i) { return LuaProxy::Path(i); },
        [&lookupTablePathID](unsigned short i) {
        ::SMBXPath *bgo = ::SMBXPath::Get(i);
        return (bgo != NULL) &&
            (bgo->id <= ::SMBXPath::MAX_ID) && lookupTablePathID.get()[bgo->id];
    }, L);
}

LuaProxy::Path::Path(unsigned short index)
{
    m_index = index;
}

int LuaProxy::Path::idx() const
{
    return m_index;
}

luabind::object LuaProxy::Path::getIntersecting(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBXPath::Count(),
        [](unsigned short i) { return LuaProxy::Path(i); },
        [x1, y1, x2, y2](unsigned short i) {
        ::SMBXPath *obj = ::SMBXPath::Get(i);
        if (obj == NULL) return false;
        if (x2 <= obj->momentum.x) return false;
        if (y2 <= obj->momentum.y) return false;
        if (obj->momentum.x + obj->momentum.width <= x1) return false;
        if (obj->momentum.y + obj->momentum.height <= y1) return false;
        return true;
    }, L);
}

short LuaProxy::Path::id(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXPath::Get(m_index)->id;
}

void LuaProxy::Path::setId(short id, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXPath::Get(m_index)->id = id;
}

double LuaProxy::Path::x(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXPath::Get(m_index)->momentum.x;
}

void LuaProxy::Path::setX(double x, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBXPath::Get(m_index)->momentum.x = x;
}

double LuaProxy::Path::y(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXPath::Get(m_index)->momentum.y;
}

void LuaProxy::Path::setY(double y, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBXPath::Get(m_index)->momentum.y = y;
}

double LuaProxy::Path::width(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXPath::Get(m_index)->momentum.width;
}

void LuaProxy::Path::setWidth(double width, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXPath::Get(m_index)->momentum.width = width;
}

double LuaProxy::Path::height(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBXPath::Get(m_index)->momentum.height;
}

void LuaProxy::Path::setHeight(double height, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBXPath::Get(m_index)->momentum.height = height;
}

bool LuaProxy::Path::visible(lua_State * L) const
{
    if (!isValid_throw(L))
        return false;
    return static_cast<bool>(SMBXPath::Get(m_index)->visible);
}

void LuaProxy::Path::setVisible(bool visible, lua_State * L)
{
    if (!isValid_throw(L))
        return;
    SMBXPath::Get(m_index)->visible = COMBOOL(visible);
}



bool LuaProxy::Path::isValid() const
{
    return (m_index < ::SMBXPath::Count());
}

bool LuaProxy::Path::isValid_throw(lua_State *L) const
{
    if (!isValid()) {
        luaL_error(L, "Invalid Path-Pointer");
        return false;
    }
    return true;
}
