#include "../LuaProxy.h"
#include "../../Blocks/Blocks.h"
#include "../../Misc/MiscFuncs.h"
#include "../../GlobalFuncs.h"

LuaProxy::Block::Block(int index) : m_index(index)
{}

void LuaProxy::Block::mem(int offset, LuaProxy::L_FIELDTYPE ftype, luabind::object value)
{
	int iftype = (int)ftype;
	if(iftype >= 1 && iftype <= 5){
		::Block* pBlock = &::Blocks::GetBase()[m_index];
		void* ptr = ((&(*(byte*)pBlock)) + offset);
		MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
	}
}

luabind::object LuaProxy::Block::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
	int iftype = (int)ftype;
	double val = 0;
	::Block* pBlock = &::Blocks::GetBase()[m_index];
	void* ptr = ((&(*(byte*)pBlock)) + offset);
	if(iftype >= 1 && iftype <= 5){
		val = GetMem((int)ptr, (FIELDTYPE)ftype);
	}
	switch (ftype) {
	case LFT_BYTE:
		return luabind::object(L, (byte)val);
	case LFT_WORD:
		return luabind::object(L, (short)val);
	case LFT_DWORD:
		return luabind::object(L, (int)val);
	case LFT_FLOAT:
		return luabind::object(L, (float)val);
	case LFT_DFLOAT:
		return luabind::object(L, (double)val);
	case LFT_STRING:
		return luabind::object(L, VBStr((wchar_t*)ptr));
	default:
		return luabind::object();
	}
}


double LuaProxy::Block::x()
{
	if(!isValid())
		return 0;
	return ::Blocks::Get(m_index)->XPos;
}

void LuaProxy::Block::setX(double x)
{
	if(!isValid())
		return;
	::Blocks::Get(m_index)->XPos = x;
}

double LuaProxy::Block::y()
{
	if(!isValid())
		return 0;
	return ::Blocks::Get(m_index)->YPos;
}

void LuaProxy::Block::setY(double y)
{
	if(!isValid())
		return;
	::Blocks::Get(m_index)->YPos = y;
}

double LuaProxy::Block::speedX()
{
	if(!isValid())
		return 0;
	return ::Blocks::Get(m_index)->XSpeed;
}

void LuaProxy::Block::setSpeedX(double speedX)
{
	if(!isValid())
		return;
	::Blocks::Get(m_index)->XSpeed = speedX;
}

double LuaProxy::Block::speedY()
{
	if(!isValid())
		return 0;
	return ::Blocks::Get(m_index)->YSpeed;
}

void LuaProxy::Block::setSpeedY(double speedY)
{
	if(!isValid())
		return;
	::Blocks::Get(m_index)->YSpeed = speedY;
}

short LuaProxy::Block::id()
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

bool LuaProxy::Block::slippery()
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

bool LuaProxy::Block::invisible()
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

int LuaProxy::Block::collidesWith(LuaProxy::Player *player)
{
	if(!isValid())
		return 0;

	if(!player->isValid())
		return 0;

	int ind = player->getIndex();
	::Block* tarBlock = ::Blocks::Get(m_index);
	PlayerMOB* tarPlayer = ::Player::Get(ind);

	double playerX = tarPlayer->CurXPos - 0.20;
	double playerY = tarPlayer->CurYPos - 0.20;
	double playerX2 = tarPlayer->CurXPos + tarPlayer->Width + 0.20;
	double playerY2 = tarPlayer->CurYPos + tarPlayer->Height + 0.20;

	if(playerX > tarBlock->XPos + tarBlock->W ||
		playerX2 < tarBlock->XPos  ||
		playerY > tarBlock->YPos + tarBlock->H ||
		playerY2 < tarBlock->YPos)
		return 0;

	return ::Blocks::TestCollision(tarPlayer, tarBlock);
}

LuaProxy::VBStr LuaProxy::Block::layerName()
{
	if(!isValid())
		return VBStr((wchar_t*)0);

	::Block* thisblock = ::Blocks::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisblock)) + 0x18);
	return VBStr(ptr);
}

luabind::object LuaProxy::Block::layerObj(lua_State *L)
{
	if(!isValid())
		return luabind::object();

	::Block* thisblock = ::Blocks::Get(m_index);
	wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisblock)) + 0x18);
	return findlayer(utf8_encode(std::wstring(ptr)).c_str(),L);
}

bool LuaProxy::Block::isValid()
{
	return !(m_index < 0 || m_index > GM_BLOCK_COUNT);
}