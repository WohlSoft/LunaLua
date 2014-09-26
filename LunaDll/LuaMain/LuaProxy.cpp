#include "LuaProxy.h"
#include "LuaHelper.h"
#include "../Rendering.h"
#include "../Globals.h"
#include "../PlayerMOB.h"
#include "../NPCs.h"
#include "../MiscFuncs.h"
#include "../SMBXEvents.h"
#include "../Sound.h"
#include "../Level.h"




std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

void LuaProxy::windowDebug(const char *debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}

void LuaProxy::print(const char *text, int x, int y)
{
    LuaProxy::print(text, 3, x, y);
}


void LuaProxy::print(const char *text, int type, int x, int y)
{
    std::wstring txt = utf8_decode(std::string(text));
    if(type==3)
        for (wstring::iterator it = txt.begin(); it != txt.end(); ++it)
            *it = towupper(*it);

    gLunaRender.SafePrint(txt, type, (float)x, (float)y);
}

LuaProxy::Player::Player() : m_index(1)
{}

LuaProxy::Player::Player(int index) : m_index(index)
{}

int LuaProxy::Player::section()
{
    return (int)::Player::Get(m_index)->CurrentSection;
}

LuaProxy::Section LuaProxy::Player::sectionObj()
{
    return Section((int)::Player::Get(m_index)->CurrentSection);
}

void LuaProxy::Player::kill()
{
    int tempint = m_index;
    ::Player::Kill(&tempint);
}

void LuaProxy::Player::harm()
{
    int tempint = m_index;
    ::Player::Harm(&tempint);
}

RECT LuaProxy::Player::screen()
{
    return ::Player::GetScreenPosition(::Player::Get(m_index));
}

double LuaProxy::Player::x()
{
    return ::Player::Get(m_index)->CurXPos;
}

void LuaProxy::Player::setX(double x)
{
    ::Player::Get(m_index)->CurXPos = x;
}

double LuaProxy::Player::y()
{
    return ::Player::Get(m_index)->CurYPos;
}

void LuaProxy::Player::setY(double y)
{
    ::Player::Get(m_index)->CurYPos = y;
}

double LuaProxy::Player::speedX()
{
    return ::Player::Get(m_index)->CurXSpeed;
}

void LuaProxy::Player::setSpeedX(double speedX)
{
    ::Player::Get(m_index)->CurXSpeed = speedX;
}

double LuaProxy::Player::speedY()
{
    return ::Player::Get(m_index)->CurYSpeed;
}

void LuaProxy::Player::setSpeedY(double speedY)
{
    ::Player::Get(m_index)->CurYSpeed = speedY;
}

int LuaProxy::Player::powerup()
{
    return (int)::Player::Get(m_index)->CurrentPowerup;
}

void LuaProxy::Player::setPowerup(int powerup)
{
    if(powerup > 0 && powerup < 9)
        ::Player::Get(m_index)->CurrentPowerup = powerup;
}

int LuaProxy::Player::reservePowerup()
{
    return (int)::Player::Get(m_index)->PowerupBoxContents;
}

void LuaProxy::Player::setReservePowerup(int reservePowerup)
{
    if(reservePowerup >= 0)
        ::Player::Get(m_index)->PowerupBoxContents = reservePowerup;
}

luabind::object LuaProxy::Player::holdingNPC(lua_State *L)
{

    if(::Player::Get(m_index)->HeldNPCIndex != 0) {
        return luabind::object(L, new NPC(::Player::Get(m_index)->HeldNPCIndex-1));
    }

    return luabind::object();
}

void LuaProxy::Player::mem(int offset, LuaProxy::L_FIELDTYPE ftype, luabind::object value)
{
    int iftype = (int)ftype;
    if(iftype >= 1 && iftype <= 5){
        PlayerMOB* pPlayer = ::Player::Get(m_index);
        void* ptr = ((&(*(byte*)pPlayer)) + offset);
        MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
    }
}

luabind::object LuaProxy::Player::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
    int iftype = (int)ftype;
    double val = 0;
    if(iftype >= 1 && iftype <= 5){
        PlayerMOB* pPlayer = ::Player::Get(m_index);
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

bool LuaProxy::Player::isValid()
{
    return ::Player::Get(m_index) != 0;
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

void LuaProxy::processKeyboardEvent(short oldp, short newp, int index, int playerIndex, lua_State *L)
{
    char pTest = pressTest(oldp, newp);
    if(pTest == 1){
        if(LuaHelper::is_function(L, "onKeyDown")){
            luabind::call_function<void>(L, "onKeyDown", index, playerIndex);
        }
    }else if(pTest == -1){
        if(LuaHelper::is_function(L, "onKeyUp")){
            luabind::call_function<void>(L, "onKeyUp", index, playerIndex);
        }
    }
}

void LuaProxy::processKeyboardEvents(lua_State* L)
{
    for(int i = 1; i <= 2; ++i){
        PlayerMOB* player = ::Player::Get(i);
        LuaEventData* evData = getEvData(i);
        if(player && evData){
            processKeyboardEvent(evData->playerUPressing, player->UKeyState, 0, i, L);
            processKeyboardEvent(evData->playerDPressing, player->DKeyState, 1, i, L);
            processKeyboardEvent(evData->playerLPressing, player->LKeyState, 2, i, L);
            processKeyboardEvent(evData->playerRPressing, player->RKeyState, 3, i, L);
            processKeyboardEvent(evData->playerJPressing, player->JKeyState, 4, i, L);
            processKeyboardEvent(evData->playerSJPressing, player->SJKeyState, 5, i, L);
            processKeyboardEvent(evData->playerXPressing, player->XKeyState, 6, i, L);
            processKeyboardEvent(evData->playerRNPressing, player->RNKeyState, 7, i, L);
            processKeyboardEvent(evData->playerSELPressing, player->SELKeyState, 8, i, L);
            processKeyboardEvent(evData->playerSTRPressing, player->STRKeyState, 9, i, L);
        }
    }
}



void LuaProxy::processJumpEvent(lua_State *L)
{
    for(int i = 1; i <= 2; ++i){
        PlayerMOB* player = ::Player::Get(i);
        LuaEventData* evData = getEvData(i);
        if(player && evData){
            if(evData->playerJumping == 0 && player->HasJumped == -1){
                if(LuaHelper::is_function(L, "onJump")){
                    luabind::call_function<void>(L, "onJump", i);
                }
            }else if(evData->playerJumping == -1 && player->HasJumped == 0){
                if(LuaHelper::is_function(L, "onJumpEnd")){
                    luabind::call_function<void>(L, "onJumpEnd", i);
                }
            }
        }
    }

}


void LuaProxy::finishEventHandling()
{
    for(int i = 1; i <= 2; ++i){
        PlayerMOB* player = ::Player::Get(i);
        LuaEventData* evData = getEvData(i);
        if(player && evData){
            evData->playerUPressing = player->UKeyState;
            evData->playerDPressing = player->DKeyState;
            evData->playerLPressing = player->LKeyState;
            evData->playerRPressing = player->RKeyState;
            evData->playerJPressing = player->JKeyState;
            evData->playerSJPressing = player->SJKeyState;
            evData->playerXPressing = player->XKeyState;
            evData->playerRNPressing = player->RNKeyState;
            evData->playerSELPressing = player->SELKeyState;
            evData->playerSTRPressing = player->STRKeyState;
            evData->playerJumping = player->HasJumped;
        }

    }

}


void LuaProxy::triggerEvent(const char *evName)
{
    SMBXEvents::TriggerEvent(utf8_decode(std::string(evName)), 0);

}


void LuaProxy::playSFX(int index)
{
    SMBXSound::PlaySFX(index);
}


LuaProxy::LuaEventData *LuaProxy::getEvData(int playerIndex)
{
    if(playerIndex==1)
        return &evPlayer1;
    if(playerIndex==2)
        return &evPlayer2;

    return 0;
}


void LuaProxy::playSFX(const char *filename)
{
    wstring world_dir = wstring((wchar_t*)GM_FULLDIR);
    wstring full_path = world_dir.append(Level::GetName());
    full_path = removeExtension(full_path);
    full_path = full_path.append(L"\\"); // < path into level folder
    full_path = full_path + utf8_decode(filename);
    PlaySound(full_path.c_str(), 0, SND_FILENAME | SND_ASYNC);
}


void LuaProxy::SaveBankProxy::setValue(const char *key, double value)
{
    gSavedVarBank.SetVar(utf8_decode(std::string(key)), value);
}


luabind::object LuaProxy::SaveBankProxy::getValue(const char *key, lua_State* L)
{
    wstring wkey = utf8_decode(std::string(key));
    if(!gSavedVarBank.VarExists(wkey))
        return luabind::object();

    return luabind::object(L, gSavedVarBank.GetVar(wkey));
}


bool LuaProxy::SaveBankProxy::isValueSet(const char *key)
{
    return gSavedVarBank.VarExists(utf8_decode(std::string(key)));
}


luabind::object LuaProxy::SaveBankProxy::values(lua_State *L)
{
    luabind::object valTable = luabind::newtable(L);

    map<wstring, double> cpMap;
    gSavedVarBank.CopyBank(&cpMap);
    for(map<wstring, double>::iterator it = cpMap.begin(); it != cpMap.end(); ++it) {
        valTable[utf8_encode(it->first.c_str())] = it->second;
    }
	return valTable;
}


void LuaProxy::SaveBankProxy::save()
{
    gSavedVarBank.WriteBank();
}
