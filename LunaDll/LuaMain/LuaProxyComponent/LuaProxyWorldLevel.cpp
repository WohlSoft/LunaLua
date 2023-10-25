#include "../LuaProxy.h"
#include "../../SMBXInternal/WorldLevel.h"
#include "../../Misc/MiscFuncs.h"



unsigned short LuaProxy::LevelObject::count()
{
    return ::WorldLevel::Count();
}

luabind::object LuaProxy::LevelObject::get(lua_State* L)
{
    return LuaHelper::getObjList(::WorldLevel::Count(), [](unsigned short i){ return LuaProxy::LevelObject(i); }, L);
}

luabind::object LuaProxy::LevelObject::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableLevelObjectID;

    try
    {
        lookupTableLevelObjectID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::WorldLevel::MAX_ID, 0));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid LevelObject-ID!\nNeed LevelObject-ID between 1-%d\nGot LevelObject-ID: %d", ::WorldLevel::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for LevelObject-ID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        ::WorldLevel::Count(),
        [](unsigned short i){ return LuaProxy::LevelObject(i); },
        [&lookupTableLevelObjectID](unsigned short i){
        ::WorldLevel *levelObject = ::WorldLevel::Get(i);
        return (levelObject != NULL) &&
            (levelObject->id <= ::WorldLevel::MAX_ID) && lookupTableLevelObjectID.get()[levelObject->id];
    }, L);
}


luabind::object LuaProxy::LevelObject::getIntersecting(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::WorldLevel::Count(),
        [](unsigned short i) { return LuaProxy::LevelObject(i); },
        [x1, y1, x2, y2](unsigned short i) {
            ::WorldLevel* obj = ::WorldLevel::Get(i);
    if (obj == NULL) return false;
    if (x2 <= obj->momentum.x) return false;
    if (y2 <= obj->momentum.y) return false;
    if (obj->momentum.x + obj->momentum.width <= x1) return false;
    if (obj->momentum.y + obj->momentum.height <= y1) return false;
    return true;
        }, L);
}


luabind::object LuaProxy::LevelObject::getByName(const std::string& levelName, lua_State* L)
{
    for (int i = 0; i < ::WorldLevel::Count(); ++i){
        WorldLevel* theLevel = WorldLevel::Get(i);
        if (!theLevel)
            return luabind::object();
        if (theLevel->levelTitle == levelName)
            return luabind::object(L, LevelObject(i));
    }
    return luabind::object();
}

luabind::object LuaProxy::LevelObject::getByFilename(const std::string& levelFilename, lua_State* L)
{
    for (int i = 0; i < ::WorldLevel::Count(); ++i){
        WorldLevel* theLevel = WorldLevel::Get(i);
        if (!theLevel)
            return luabind::object();
        if (theLevel->levelFileName == levelFilename)
            return luabind::object(L, LevelObject(i));
    }
    return luabind::object();
}

luabind::object LuaProxy::LevelObject::findByName(const std::string& levelName, lua_State* L)
{
    luabind::object levels = luabind::newtable(L);
    for (int i = 0, j = 0; i < ::WorldLevel::Count(); ++i){
        WorldLevel* theLevel = WorldLevel::Get(i);
        if (!theLevel)
            break;
        if (((std::string)theLevel->levelTitle).find(levelName) != std::string::npos)
            levels[++j] = LevelObject(i);
    }
    return levels;
}

luabind::object LuaProxy::LevelObject::findByFilename(const std::string& levelFilename, lua_State* L)
{
    luabind::object levels = luabind::newtable(L);
    for (int i = 0, j = 0; i < ::WorldLevel::Count(); ++i){
        WorldLevel* theLevel = WorldLevel::Get(i);
        if (!theLevel)
            break;
        if (((std::string)theLevel->levelFileName).find(levelFilename) != std::string::npos)
            levels[++j] = LevelObject(i);
    }
    return levels;
}



LuaProxy::LevelObject::LevelObject(int index) : m_index(index)
{}

int LuaProxy::LevelObject::idx() const
{
    return m_index;
}

void LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, const luabind::object &value, lua_State* L)
{
    WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
    void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
    LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::LevelObject::mem(int offset, L_FIELDTYPE ftype, lua_State* L) const
{
    WorldLevel* pWorldLevel = ::SMBXLevel::get(m_index);
    void* ptr = ((&(*(byte*)pWorldLevel)) + offset);
    return LuaProxy::mem((int)ptr, ftype, L);
}

bool LuaProxy::LevelObject::isValid() const
{
    if((m_index < 0) || (m_index > (signed)GM_LEVEL_COUNT))
        return false;
    return true;
}


double LuaProxy::LevelObject::x() const
{
    if(!isValid())
        return 0;
    return SMBXLevel::get(m_index)->momentum.x;
}


void LuaProxy::LevelObject::setX(double x)
{
    if(!isValid())
        return;
    SMBXLevel::get(m_index)->momentum.x = x;
}

double LuaProxy::LevelObject::y() const
{
    if(!isValid())
        return 0;
    return SMBXLevel::get(m_index)->momentum.y;
}

void LuaProxy::LevelObject::setY(double y)
{
    if(!isValid())
        return;
    SMBXLevel::get(m_index)->momentum.y = y;
}

double LuaProxy::LevelObject::width() const
{
    if (!isValid())
        return 0;
    return SMBXLevel::get(m_index)->momentum.width;
}

void LuaProxy::LevelObject::setWidth(double width)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->momentum.width = width;
}

double LuaProxy::LevelObject::height() const
{
    if (!isValid())
        return 0;
    return SMBXLevel::get(m_index)->momentum.height;
}

void LuaProxy::LevelObject::setHeight(double height)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->momentum.height = height;
}


short LuaProxy::LevelObject::id() const
{
    if (!isValid())
        return 0;
    return SMBXLevel::get(m_index)->id;
}

void LuaProxy::LevelObject::setId(short id)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->id = id;
}

double LuaProxy::LevelObject::goToX()
{
    if (!isValid())
        return 0.0;
    return SMBXLevel::get(m_index)->goToX;
}

void LuaProxy::LevelObject::setGoToX(double goToX)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->goToX = goToX;
}

double LuaProxy::LevelObject::goToY()
{
    if (!isValid())
        return 0.0;
    return SMBXLevel::get(m_index)->goToY;
}

void LuaProxy::LevelObject::setGoToY(double goToY)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->goToY = goToY;
}

ExitType LuaProxy::LevelObject::topExitType() const
{
    if (!isValid())
        return EXITTYPE_NONE;
    return SMBXLevel::get(m_index)->topExitType;
}

void LuaProxy::LevelObject::setTopExitType(ExitType topExitType)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->topExitType = topExitType;
}

ExitType LuaProxy::LevelObject::leftExitType() const
{
    if (!isValid())
        return EXITTYPE_NONE;
    return SMBXLevel::get(m_index)->leftExitType;
}

void LuaProxy::LevelObject::setLeftExitType(ExitType leftExitType)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->leftExitType = leftExitType;
}

ExitType LuaProxy::LevelObject::bottomExitType() const
{
    if (!isValid())
        return EXITTYPE_NONE;
    return SMBXLevel::get(m_index)->bottomExitType;
}

void LuaProxy::LevelObject::setBottomExitType(ExitType bottomExitType)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->bottomExitType = bottomExitType;
}

ExitType LuaProxy::LevelObject::rightExitType() const
{
    if (!isValid())
        return EXITTYPE_NONE;
    return SMBXLevel::get(m_index)->rightExitType;
}

void LuaProxy::LevelObject::setRightExitType(ExitType rightExitType)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->rightExitType = rightExitType;
}

bool LuaProxy::LevelObject::visible() const
{
    if (!isValid())
        return false;
    return static_cast<bool>(SMBXLevel::get(m_index)->visible != 0);
}

void LuaProxy::LevelObject::setVisible(bool isHidden)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->visible = COMBOOL(isHidden);
}

short LuaProxy::LevelObject::levelWarpNumber() const
{
    if (!isValid())
        return 0;
    return SMBXLevel::get(m_index)->levelWarpNumber;
}

void LuaProxy::LevelObject::setLevelWarpNumber(short levelWarpNumber)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->levelWarpNumber = levelWarpNumber;
}

bool LuaProxy::LevelObject::isPathBackground() const
{
    if (!isValid())
        return false;
    return 0 != SMBXLevel::get(m_index)->isPathBackground;
}

void LuaProxy::LevelObject::setIsPathBackground(bool isPathBackground)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->isPathBackground = COMBOOL(isPathBackground);
}

bool LuaProxy::LevelObject::isBigBackground() const
{
    if (!isValid())
        return false;
    return 0 != SMBXLevel::get(m_index)->isBigBackground;
}

void LuaProxy::LevelObject::setIsBigBackground(bool isBigBackground)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->isBigBackground = COMBOOL(isBigBackground);
}

bool LuaProxy::LevelObject::isGameStartPoint() const
{
    if (!isValid())
        return false;
    return 0 != SMBXLevel::get(m_index)->isGameStartPoint;
}

void LuaProxy::LevelObject::setIsGameStartPoint(bool isGameStartPoint)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->isGameStartPoint = COMBOOL(isGameStartPoint);
}

bool LuaProxy::LevelObject::isAlwaysVisible() const
{
    if (!isValid())
        return false;
    return 0 != SMBXLevel::get(m_index)->isAlwaysVisible;
}

void LuaProxy::LevelObject::setIsAlwaysVisible(bool isAlwaysVisible)
{
    if (!isValid())
        return;
    SMBXLevel::get(m_index)->isAlwaysVisible = COMBOOL(isAlwaysVisible);
}

std::string LuaProxy::LevelObject::title() const
{
    if(!isValid())
        return "";
    return SMBXLevel::get(m_index)->levelTitle;
}


std::string LuaProxy::LevelObject::filename() const
{
    if (!isValid())
        return "";
    return SMBXLevel::get(m_index)->levelFileName;
}

