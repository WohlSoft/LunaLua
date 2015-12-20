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

luabind::object LuaProxy::BGO::getIntersecting(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBX_BGO::Count(),
        [](unsigned short i){ return LuaProxy::BGO(i); },
        [x1, y1, x2, y2](unsigned short i){
        ::SMBX_BGO *obj = ::SMBX_BGO::Get(i);
        if (obj == NULL) return false;
        if (x2 <= obj->momentum.x) return false;
        if (y2 <= obj->momentum.y) return false;
        if (obj->momentum.x + obj->momentum.width <= x1) return false;
        if (obj->momentum.y + obj->momentum.height <= y1) return false;
        return true;
    }, L);
}

bool LuaProxy::BGO::isHidden(lua_State* L) const
{
    if (!isValid_throw(L))
        return false;
    return 0 != SMBX_BGO::Get(m_index)->isHidden;
}

void LuaProxy::BGO::setIsHidden(bool isHidden, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->isHidden = isHidden ? -1 : 0;
}

short LuaProxy::BGO::id(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_BGO::Get(m_index)->id;
}

void LuaProxy::BGO::setId(short id, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->id = id;
}

double LuaProxy::BGO::x(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_BGO::Get(m_index)->momentum.x;
}

void LuaProxy::BGO::setX(double x, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->momentum.x = x;
}

double LuaProxy::BGO::y(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_BGO::Get(m_index)->momentum.y;
}

void LuaProxy::BGO::setY(double y, lua_State* L) const
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->momentum.y = y;
}

double LuaProxy::BGO::width(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_BGO::Get(m_index)->momentum.width;
}

void LuaProxy::BGO::setWidth(double width, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->momentum.width = width;
}

double LuaProxy::BGO::height(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_BGO::Get(m_index)->momentum.height;
}

void LuaProxy::BGO::setHeight(double height, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->momentum.height = height;
}

double LuaProxy::BGO::speedX(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_BGO::Get(m_index)->momentum.speedX;
}

void LuaProxy::BGO::setSpeedX(double speedX, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->momentum.speedX = speedX;
}

double LuaProxy::BGO::speedY(lua_State* L) const
{
    if (!isValid_throw(L))
        return 0;
    return SMBX_BGO::Get(m_index)->momentum.speedY;
}

void LuaProxy::BGO::setSpeedY(double speedY, lua_State* L)
{
    if (!isValid_throw(L))
        return;
    SMBX_BGO::Get(m_index)->momentum.speedY = speedY;
}


std::string LuaProxy::BGO::layerName(lua_State* L) const
{
    if (!isValid_throw(L))
        return "";

    return SMBX_BGO::Get(m_index)->ptLayerName;
}

void LuaProxy::BGO::setLayerName(const luabind::object& value, lua_State* L)
{
    if (!isValid_throw(L))
        return;

    SMBX_BGO* thisbgo = ::SMBX_BGO::Get(m_index);
    LuaHelper::assignVB6StrPtr(&thisbgo->ptLayerName, value, L);
}

luabind::object LuaProxy::BGO::layer(lua_State* L) const
{
    if (!isValid_throw(L))
        return luabind::object(L, Layer(0));

    return Layer::get(SMBX_BGO::Get(m_index)->ptLayerName, L);
}

void LuaProxy::BGO::setLayer(const Layer& layer, lua_State* L)
{
    if (!isValid_throw(L))
        return;

    return SMBX_BGO::Get(m_index)->ptLayerName = layer.layerName().str();
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
