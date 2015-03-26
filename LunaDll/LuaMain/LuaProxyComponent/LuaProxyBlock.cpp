#include "../LuaProxy.h"
#include "../../SMBXInternal/Blocks.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"




int LuaProxy::Block::count()
{
    return ::Block::Count();
}

luabind::object LuaProxy::Block::get(lua_State* L)
{
    return LuaHelper::getObjList(::Block::Count(), [](unsigned short i){ return LuaProxy::Block(i); }, L);
}

luabind::object LuaProxy::Block::get(luabind::object idFilter, lua_State* L)
{
    std::unique_ptr<bool> lookupTableBlockID;

    try
    {
        lookupTableBlockID = std::unique_ptr<bool>(LuaHelper::generateFilterTable(L, idFilter, ::Block::MAX_ID));
    }
    catch (LuaHelper::invalidIDException* e)
    {
        luaL_error(L, "Invalid BGO-ID!\nNeed BGO-ID between 1-%d\nGot BGO-ID: %d", ::Block::MAX_ID, e->usedID());
        return luabind::object();
    }
    catch (LuaHelper::invalidTypeException* /*e*/)
    {
        luaL_error(L, "Invalid args for bgoID (arg #1, expected table or number, got %s)", lua_typename(L, luabind::type(idFilter)));
        return luabind::object();
    }

    return LuaHelper::getObjList(
        ::Block::Count(),
        [](unsigned short i){ return LuaProxy::BGO(i); },
        [&lookupTableBlockID](unsigned short i){
        ::Block *block = ::Block::Get(i);
        return (block != NULL) &&
            (block->BlockType <= ::Block::MAX_ID) && lookupTableBlockID.get()[block->BlockType];
    }, L);
}


LuaProxy::Block::Block(int index) : m_index(index)
{}

void LuaProxy::Block::mem(int offset, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State *L)
{
	::Block* pBlock = &::Blocks::GetBase()[m_index];
	void* ptr = ((&(*(byte*)pBlock)) + offset);
	LuaProxy::mem((int)ptr, ftype, value, L);
}

luabind::object LuaProxy::Block::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L) const
{
	::Block* pBlock = &::Blocks::GetBase()[m_index];
	void* ptr = ((&(*(byte*)pBlock)) + offset);
	return LuaProxy::mem((int)ptr, ftype, L);
}


double LuaProxy::Block::x() const
{
	if(!isValid())
		return 0;
    return ::Blocks::Get(m_index)->mometum.x;
}

void LuaProxy::Block::setX(double x)
{
	if(!isValid())
		return;
    ::Blocks::Get(m_index)->mometum.x = x;
}

double LuaProxy::Block::y() const
{
	if(!isValid())
		return 0;
    return ::Blocks::Get(m_index)->mometum.y;
}

void LuaProxy::Block::setY(double y)
{
	if(!isValid())
		return;
    ::Blocks::Get(m_index)->mometum.y = y;
}

double LuaProxy::Block::speedX() const
{
	if(!isValid())
		return 0;
    return ::Blocks::Get(m_index)->mometum.speedX;
}

void LuaProxy::Block::setSpeedX(double speedX)
{
	if(!isValid())
		return;
    ::Blocks::Get(m_index)->mometum.speedX = speedX;
}

double LuaProxy::Block::speedY() const
{
	if(!isValid())
		return 0;
    return ::Blocks::Get(m_index)->mometum.speedY;
}

void LuaProxy::Block::setSpeedY(double speedY)
{
	if(!isValid())
		return;
    ::Blocks::Get(m_index)->mometum.speedY = speedY;
}

short LuaProxy::Block::id() const
{
	if(!isValid())
		return 0;
	return ::Blocks::Get(m_index)->BlockType;
}

void LuaProxy::Block::setId(short id)
{
	if(!isValid())
		return;

	::Blocks::Get(m_index)->BlockType = id;
}

bool LuaProxy::Block::slippery() const
{
	if(!isValid())
		return false;

	return (bool)::Blocks::Get(m_index)->Slippery;
}

void LuaProxy::Block::setSlippery(bool slippery)
{
	if(!isValid())
		return;

	::Blocks::Get(m_index)->Slippery = (slippery ? 0xFFFF : 0);
}

bool LuaProxy::Block::invisible() const
{
	if(!isValid())
		return 0;

	return (bool)::Blocks::Get(m_index)->IsInvisible;
}

void LuaProxy::Block::setInvisible(bool invisible)
{
	if(!isValid())
		return;

	::Blocks::Get(m_index)->IsInvisible = (invisible ? 0xFFFF : 0);
}

int LuaProxy::Block::collidesWith(const LuaProxy::Player *player) const
{
	if(!isValid())
		return 0;

	if(!player->isValid())
		return 0;

	int ind = player->getIndex();
	::Block* tarBlock = ::Blocks::Get(m_index);
	PlayerMOB* tarPlayer = ::Player::Get(ind);

    double playerX = tarPlayer->momentum.x - 0.20;
    double playerY = tarPlayer->momentum.y - 0.20;
    double playerX2 = tarPlayer->momentum.x + tarPlayer->momentum.width + 0.20;
    double playerY2 = tarPlayer->momentum.y + tarPlayer->momentum.height + 0.20;

    if (playerX > tarBlock->mometum.x + tarBlock->mometum.width ||
        playerX2 < tarBlock->mometum.x ||
        playerY > tarBlock->mometum.y + tarBlock->mometum.height ||
        playerY2 < tarBlock->mometum.y)
		return 0;

	return ::Blocks::TestCollision(tarPlayer, tarBlock);
}

LuaProxy::VBStr LuaProxy::Block::layerName() const
{
	if(!isValid())
		return VBStr((wchar_t*)0);

	::Block* thisblock = ::Blocks::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisblock)) + 0x18);
	return VBStr(ptr);
}

luabind::object LuaProxy::Block::layerObj(lua_State *L) const
{
	if(!isValid())
		return luabind::object();

	::Block* thisblock = ::Blocks::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisblock)) + 0x18);
	return findlayer(utf8_encode(std::wstring(ptr)).c_str(),L);
}

bool LuaProxy::Block::isValid() const
{
	return !(m_index < 0 || m_index > GM_BLOCK_COUNT);
}