#include "LuaProxy.h"
#include "LuaHelper.h"
#include "../Rendering.h"
#include "../Globals.h"
#include "../PlayerMOB.h"
#include "../NPCs.h"
#include "../MiscFuncs.h"



std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void LuaProxy::windowDebug(const char *debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}

void LuaProxy::print(const char *text, int x, int y)
{
    gLunaRender.SafePrint(utf8_decode(std::string(text)), 3, (float)x, (float)y);
}


void LuaProxy::print(const char *text, int type, int x, int y)
{
    gLunaRender.SafePrint(utf8_decode(std::string(text)), type, (float)x, (float)y);
}

LuaProxy::Player::Player()
{}

int LuaProxy::Player::section()
{
    return (int)::Player::Get(1)->CurrentSection;
}

LuaProxy::Section LuaProxy::Player::sectionObj()
{
    return Section((int)::Player::Get(1)->CurrentSection);
}

void LuaProxy::Player::kill()
{
    int tempint = 1;
    ::Player::Kill(&tempint);
}

void LuaProxy::Player::harm()
{
    int tempint = 1;
    ::Player::Harm(&tempint);
}

RECT LuaProxy::Player::screen()
{
    return ::Player::GetScreenPosition(::Player::Get(1));
}

double LuaProxy::Player::x()
{
    return ::Player::Get(1)->CurXPos;
}

void LuaProxy::Player::setX(double x)
{
    ::Player::Get(1)->CurXPos = x;
}

double LuaProxy::Player::y()
{
    return ::Player::Get(1)->CurYPos;
}

void LuaProxy::Player::setY(double y)
{
    ::Player::Get(1)->CurYPos = y;
}

double LuaProxy::Player::speedX()
{
    return ::Player::Get(1)->CurXSpeed;
}

void LuaProxy::Player::setSpeedX(double speedX)
{
    ::Player::Get(1)->CurXSpeed = speedX;
}

double LuaProxy::Player::speedY()
{
    return ::Player::Get(1)->CurYSpeed;
}

void LuaProxy::Player::setSpeedY(double speedY)
{
    ::Player::Get(1)->CurYSpeed = speedY;
}

int LuaProxy::Player::powerup()
{
    return (int)::Player::Get(1)->CurrentPowerup;
}

void LuaProxy::Player::setPowerup(int powerup)
{
    if(powerup > 0 && powerup < 9)
        ::Player::Get(1)->CurrentPowerup = powerup;
}

int LuaProxy::Player::reservePowerup()
{
    return (int)::Player::Get(1)->PowerupBoxContents;
}

void LuaProxy::Player::setReservePowerup(int reservePowerup)
{
    if(reservePowerup >= 0)
        ::Player::Get(1)->PowerupBoxContents = reservePowerup;
}

luabind::object LuaProxy::Player::holdingNPC(lua_State *L)
{

    if(::Player::Get(1)->HeldNPCIndex != 0) {
        return luabind::object(L, new NPC(::Player::Get(1)->HeldNPCIndex-1));
    }

    return luabind::object();
}

void LuaProxy::Player::mem(int offset, LuaProxy::L_FIELDTYPE ftype, luabind::object value)
{
    int iftype = (int)ftype;
    if(iftype >= 1 && iftype <= 5){
        PlayerMOB* pPlayer = ::Player::Get(1);
        void* ptr = ((&(*(byte*)pPlayer)) + offset);
        MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
    }
}

luabind::object LuaProxy::Player::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
    int iftype = (int)ftype;
    double val = 0;
    if(iftype >= 1 && iftype <= 5){
        PlayerMOB* pPlayer = ::Player::Get(1);
        void* ptr = ((&(*(byte*)pPlayer)) + offset);
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
    default:
        return luabind::object();
    }
}


LuaProxy::NPC::NPC(int index)
{
    m_index = index;
}

int LuaProxy::NPC::id()
{
    if(!isValid())
        return 0;
    return (int)::NPC::Get(m_index)->Identity;
}

float LuaProxy::NPC::direction()
{
    if(!isValid())
        return 0;
    return ::NPC::Get(m_index)->FacingDirection;
}

void LuaProxy::NPC::setDirection(float direction)
{
    if(!isValid())
        return;

    NPCMOB* npc =  ::NPC::Get(m_index);
    setSpeedX(0.0);
    npc->FacingDirection = direction;
}

double LuaProxy::NPC::x()
{
    if(!isValid())
        return 0;
    return ::NPC::Get(m_index)->Xpos;
}

void LuaProxy::NPC::setX(double x)
{
    if(!isValid())
        return;
    ::NPC::Get(m_index)->Xpos = x;
}

double LuaProxy::NPC::y()
{
    if(!isValid())
        return 0;
    return ::NPC::Get(m_index)->Ypos;
}

void LuaProxy::NPC::setY(double y)
{
    if(!isValid())
        return;
    ::NPC::Get(m_index)->Ypos;
}

double LuaProxy::NPC::speedX()
{
    if(!isValid())
        return 0;
    return *((double*)((&(*(byte*)::NPC::Get(m_index))) + 0x98));
}

void LuaProxy::NPC::setSpeedX(double speedX)
{
    if(!isValid())
        return;
    double* ptr=(double*)((&(*(byte*)::NPC::Get(m_index))) + 0x98);
    *ptr = speedX;
}

double LuaProxy::NPC::speedY()
{
    if(!isValid())
        return 0;
    return *((double*)((&(*(byte*)::NPC::Get(m_index))) + 0xA0));
}

void LuaProxy::NPC::setSpeedY(double speedY)
{
    if(!isValid())
        return;
    double* ptr=(double*)((&(*(byte*)::NPC::Get(m_index))) + 0xA0);
    *ptr = speedY;
}

void LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, luabind::object value)
{
    int iftype = (int)ftype;
    if(iftype >= 1 && iftype <= 5){
        NPCMOB* mnpc = ::NPC::Get(m_index);
        void* ptr = ((&(*(byte*)mnpc)) + offset);
        MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
    }
}

luabind::object LuaProxy::NPC::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State* L)
{
    int iftype = (int)ftype;
    double val = 0;
    if(iftype >= 1 && iftype <= 5){
        NPCMOB* mnpc = ::NPC::Get(m_index);
        void* ptr = ((&(*(byte*)mnpc)) + offset);
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
    default:
        return luabind::object();
    }
}

bool LuaProxy::NPC::isValid()
{
    return !(m_index < 0 || m_index > GM_NPCS_COUNT);
}


int LuaProxy::totalNPCs()
{
    return (int)GM_NPCS_COUNT;
}


luabind::object LuaProxy::npcs(lua_State *L)
{
    luabind::object vnpcs = luabind::newtable(L);
    for(int i = 0; i < GM_NPCS_COUNT; i++) {
        vnpcs[i] = new LuaProxy::NPC(i);
    }
    return vnpcs;
}


luabind::object LuaProxy::findNPCs(int ID, int section, lua_State *L)
{
    luabind::object vnpcs = luabind::newtable(L);
    int r = 0;

    bool anyID = (ID == -1 ? true : false);
    bool anySec = (section == -1 ? true : false);
    NPCMOB* thisnpc = NULL;

    for(int i = 0; i < GM_NPCS_COUNT; i++) {
        thisnpc = ::NPC::Get(i);
        if(thisnpc->Identity == ID || anyID) {
            if(::NPC::GetSection(thisnpc) == section || anySec) {
                vnpcs[r] = LuaProxy::NPC(i);
                ++r;
            }
        }
    }
    return vnpcs;
}


LuaProxy::Section::Section(int sectionNum)
{
    m_secNum = sectionNum;
}

LuaProxy::RECTd LuaProxy::Section::boundary()
{
    double* boundtable = (double*)GM_LVL_BOUNDARIES;
    boundtable = boundtable + (m_secNum * 6);

    RECTd rectd;
    rectd.left = boundtable[0];
    rectd.top = boundtable[1];
    rectd.bottom = boundtable[2];
    rectd.right = boundtable[3];

    return rectd;
}

void LuaProxy::Section::setBoundary(LuaProxy::RECTd boundary)
{
    double* boundtable = (double*)GM_LVL_BOUNDARIES;
    boundtable = boundtable + (m_secNum * 6);
    boundtable[0] = boundary.left;
    boundtable[1] = boundary.top;
    boundtable[2] = boundary.bottom;
    boundtable[3] = boundary.right;
}


void LuaProxy::mem(int mem, LuaProxy::L_FIELDTYPE ftype, luabind::object value)
{
    int iftype = (int)ftype;
    if(iftype >= 1 && iftype <= 5){
        void* ptr = ((&(*(byte*)mem)));
        MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
    }
}


luabind::object LuaProxy::mem(int mem, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
    int iftype = (int)ftype;
    double val = 0;
    if(iftype >= 1 && iftype <= 5){
        void* ptr = ((&(*(byte*)mem)));
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
    default:
        return luabind::object();
    }
}

char LuaProxy::pressTest(short oldp, short newp)
{
    if(oldp == 0 && newp == -1)
        return 1;

    if(oldp == -1 && oldp == 0)
        return -1;

    return 0;
}

void LuaProxy::processKeyboardEvent(short oldp, short newp, int index, lua_State *L)
{
    char pTest = pressTest(oldp, newp);
    if(pTest == 1){
        if(LuaHelper::is_function(L, "onKeyDown")){
            luabind::call_function<void>(L, "onKeyDown", index);
        }
    }else if(pTest == -1){
        if(LuaHelper::is_function(L, "onKeyUp")){
            luabind::call_function<void>(L, "onKeyUp", index);
        }
    }
}

void LuaProxy::processKeyboardEvents(lua_State* L)
{
    PlayerMOB* player = ::Player::Get(1);
    processKeyboardEvent(playerUPressing, player->UKeyState, 0, L);
    processKeyboardEvent(playerDPressing, player->DKeyState, 1, L);
    processKeyboardEvent(playerLPressing, player->LKeyState, 2, L);
    processKeyboardEvent(playerRPressing, player->RKeyState, 3, L);
    processKeyboardEvent(playerJPressing, player->JKeyState, 4, L);
    processKeyboardEvent(playerSJPressing, player->SJKeyState, 5, L);
    processKeyboardEvent(playerXPressing, player->XKeyState, 6, L);
    processKeyboardEvent(playerRNPressing, player->RNKeyState, 7, L);
    processKeyboardEvent(playerSELPressing, player->SELKeyState, 8, L);
    processKeyboardEvent(playerSTRPressing, player->STRKeyState, 9, L);
}



void LuaProxy::processJumpEvent(lua_State *L)
{
    PlayerMOB* player = ::Player::Get(1);
    if(playerJumping == 0 && player->HasJumped == -1){
        if(LuaHelper::is_function(L, "onJump")){
            luabind::call_function<void>(L, "onJump");
        }
    }else if(playerJumping == -1 && player->HasJumped == 0){
        if(LuaHelper::is_function(L, "onJumpEnd")){
            luabind::call_function<void>(L, "onJumpEnd");
        }
    }
}


void LuaProxy::finishEventHandling()
{
    PlayerMOB* player = ::Player::Get(1);
    playerUPressing = player->UKeyState;
    playerDPressing = player->DKeyState;
    playerLPressing = player->LKeyState;
    playerRPressing = player->RKeyState;
    playerJPressing = player->JKeyState;
    playerSJPressing = player->SJKeyState;
    playerXPressing = player->XKeyState;
    playerRNPressing = player->RNKeyState;
    playerSELPressing = player->SELKeyState;
    playerSTRPressing = player->STRKeyState;
    playerJumping = player->HasJumped;
}
