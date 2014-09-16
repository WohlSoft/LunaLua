#include "LuaProxy.h"
#include "../Rendering.h"
#include "../Globals.h"
#include "../PlayerMOB.h"
#include "../NPCs.h"



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
