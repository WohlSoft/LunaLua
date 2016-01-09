#include "../LuaProxy.h"
#include "../../SMBXInternal/Animation.h"
#include "../../Misc/MiscFuncs.h"

int LuaProxy::Animation::count()
{
    return SMBXAnimation::Count();
}

luabind::object LuaProxy::Animation::get(lua_State* L)
{
    return LuaHelper::getObjList(::SMBXAnimation::Count(), [](unsigned short i){ return LuaProxy::Animation(i); }, L);
}

luabind::object LuaProxy::Animation::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableEffectID;

    try
    {
        lookupTableEffectID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::SMBXAnimation::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid Effect-ID!\nNeed Effect-ID between 1-%d\nGot Effect-ID: %d", ::SMBXAnimation::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for EffectID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        ::SMBXAnimation::Count(),
        [](unsigned short i){ return LuaProxy::Animation(i); },
        [&lookupTableEffectID](unsigned short i){
        ::SMBXAnimation *effect = ::SMBXAnimation::Get(i);
        return (effect != NULL) &&
            (effect->AnimationID <= ::SMBXAnimation::MAX_ID) && lookupTableEffectID.get()[effect->AnimationID];
    }, L);
}

luabind::object LuaProxy::Animation::getIntersecting(double x1, double y1, double x2, double y2, lua_State* L)
{
    return LuaHelper::getObjList(
        ::SMBXAnimation::Count(),
        [](unsigned short i){ return LuaProxy::Animation(i); },
        [x1, y1, x2, y2](unsigned short i){
        ::SMBXAnimation *obj = ::SMBXAnimation::Get(i);
        if (obj == NULL) return false;
        if (x2 <= obj->momentum.x) return false;
        if (y2 <= obj->momentum.y) return false;
        if (obj->momentum.x + obj->momentum.width <= x1) return false;
        if (obj->momentum.y + obj->momentum.height <= y1) return false;
        return true;
    }, L);
}

LuaProxy::Animation LuaProxy::Animation::spawnEffect(short effectID, double x, double y, lua_State* L)
{
    return spawnEffect(effectID, x, y, 1.0f, L);
}

LuaProxy::Animation LuaProxy::Animation::spawnEffect(short effectID, double x, double y, float animationFrame, lua_State* L)
{
    typedef void __stdcall animationFunc(short*, Momentum*, float*, short*, short*);
    animationFunc* spawnEffectFunc = (animationFunc*)GF_RUN_ANIM;

    if (effectID < 1 || effectID > SMBXAnimation::MAX_ID){
        luaL_error(L, "Invalid Effect-ID!\nNeed Effect-ID between 1-148\nGot Effect-ID: %d", effectID);
        return LuaProxy::Animation(-1);
    }

    if (GM_ANIM_COUNT >= 996){
        luaL_error(L, "Over 996 Effects, cannot spawn more!");
        return LuaProxy::Animation(-1);
    }

    Momentum coor = {0};                    //Arg 2
    coor.x = x;
    coor.y = y;            //Arg 3
    short npcID = 0;                        //Arg 4
    short onlyDrawMask = COMBOOL(false);    //Arg 5

    spawnEffectFunc(&effectID, &coor, &animationFrame, &npcID, &onlyDrawMask);
    return LuaProxy::Animation(GM_ANIM_COUNT - 1);
}




LuaProxy::Animation::Animation(int animationIndex)
{
    m_animationIndex = animationIndex;
}

void LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State *L)
{
    SMBXAnimation* manimation = SMBXAnimation::Get(m_animationIndex);
    void* ptr = ((&(*(byte*)manimation)) + offset);
    LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L) const
{
    SMBXAnimation* manimation = SMBXAnimation::Get(m_animationIndex);
    void* ptr = ((&(*(byte*)manimation)) + offset);
    return LuaProxy::mem((int)ptr, ftype, L);
}

short LuaProxy::Animation::id() const
{
    if(!isValid())
        return 0;
    return static_cast<short>(SMBXAnimation::Get(m_animationIndex)->AnimationID);
}

void LuaProxy::Animation::setId(short id)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->AnimationID = static_cast<EffectID>(id);
}

double LuaProxy::Animation::x() const
{
    if(!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->momentum.x;
}

void LuaProxy::Animation::setX(double x)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->momentum.x = x;
}

double LuaProxy::Animation::y() const
{
    if(!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->momentum.y;
}

void LuaProxy::Animation::setY(double y)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->momentum.y = y;
}

double LuaProxy::Animation::speedX() const
{
    if(!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->momentum.speedX;
}

void LuaProxy::Animation::setSpeedX(double speedX)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->momentum.speedX = speedX;
}

double LuaProxy::Animation::speedY() const
{
    if(!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->momentum.speedY;
}

void LuaProxy::Animation::setSpeedY(double speedY)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->momentum.speedY = speedY;
}

double LuaProxy::Animation::height() const
{
    if(!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->momentum.height;
}

void LuaProxy::Animation::setHeight(double height)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->momentum.height = height;
}

double LuaProxy::Animation::width() const
{
    if(!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->momentum.width;
}

void LuaProxy::Animation::setWidth(double width)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->momentum.width = width;
}

float LuaProxy::Animation::subTimer() const
{
    if (!isValid())
        return 0;

    return SMBXAnimation::Get(m_animationIndex)->subTimer;
}

void LuaProxy::Animation::setSubTimer(float subTimer)
{
    if (!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->subTimer = subTimer;
}


short LuaProxy::Animation::timer() const
{
    if (!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->Timer;
}

void LuaProxy::Animation::setTimer(short timer)
{
    if(!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->Timer = timer;

}

short LuaProxy::Animation::npcID() const
{
    if (!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->npcID;
}

void LuaProxy::Animation::setNpcID(short npcID)
{
    if (!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->npcID = npcID;
}

bool LuaProxy::Animation::drawOnlyMask() const
{
    if (!isValid())
        return false;
    return 0 != SMBXAnimation::Get(m_animationIndex)->onlyDrawMask;
}

void LuaProxy::Animation::setDrawOnlyMask(bool drawOnlyMask)
{
    if (!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->onlyDrawMask = COMBOOL(drawOnlyMask);
}

short LuaProxy::Animation::animationFrame() const
{
    if (!isValid())
        return 0;
    return SMBXAnimation::Get(m_animationIndex)->animationFrame;
}

void LuaProxy::Animation::setAnimationFrame(short animationFrame)
{
    if (!isValid())
        return;
    SMBXAnimation::Get(m_animationIndex)->animationFrame = animationFrame;
}


bool LuaProxy::Animation::isValid() const
{
    return !(m_animationIndex < 0 || m_animationIndex > GM_ANIM_COUNT);
}


