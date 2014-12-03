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
#include "../CSprite.h"
#include "../CSpriteManager.h"
#include "../Blocks.h"
#include "../Layer.h"
#include "../Animation.h"

#include "../libs/ini-reader/INIReader.h"



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

//type - Player's state/powerup
//ini_file - path to INI-file which contains the hitbox redefinations
void LuaProxy::loadHitboxes(int _character, int _powerup, const char *ini_file)
{
	if( (_powerup < 1) || (_powerup>7)) return;
	if(( _character < 1) || (_character > 5)) return;

	int powerup = _powerup-1;
	int character = _character-1;

	wstring world_dir = wstring((wchar_t*)GM_FULLDIR);
    wstring full_path = world_dir.append(Level::GetName());
    full_path = removeExtension(full_path);
    full_path = full_path.append(L"\\"); // < path into level folder
    full_path = full_path + utf8_decode(ini_file);

	std::wstring ws = full_path;
	std::string s;
    const std::locale locale("");
    typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
    const converter_type& converter = std::use_facet<converter_type>(locale);
    std::vector<char> to(ws.length() * converter.max_length());
    std::mbstate_t state;
    const wchar_t* from_next;
    char* to_next;
    const converter_type::result result = converter.out(state,
		full_path.data(), full_path.data() + full_path.length(),
		from_next, &to[0], &to[0] + to.size(), to_next);
    if (result == converter_type::ok || result == converter_type::noconv)
	{
      s = std::string(&to[0], to_next);
    }

	
	INIReader hitBoxFile( s.c_str() );
		if (hitBoxFile.ParseError() < 0)
		{
			MessageBoxA(0, std::string(s+"\n\nError of read INI file").c_str(), "Error", 0);
		    return;
		}


		short *hitbox_width = (short *)(0xB2C788);
		short *hitbox_height = (short *)(0xB2C6FC);
		short *hitbox_height_duck = (short *)(0xB2C742);
		

    //Parser of hitbox properties from PGE Calibrator INI File

			//Frames X and Y on playable character sprite from 0 to 9

			//hitBoxFile.Get("frame-y-x", "used", "false");
		//if(used==false) --> skip this frame
		//{
				//Size of hitbox
			//hitBoxFile.Get("frame-x-x", "width", "default value");
			//hitBoxFile.Get("frame-x-x", "height", "default value");
				//Offset relative to
			//hitBoxFile.Get("frame-x-x", "offsetX", "default value");
			//hitBoxFile.Get("frame-x-x", "offsetY", "default value");
				//Later will be available grab offset x and grab offset y
			//hitBoxFile.Get("frame-x-x", "grabOffsetX", "default value");
			//hitBoxFile.Get("frame-x-x", "grabOffsetY", "default value");
		//}
		std::string width = "0";
		std::string height = "0";
		std::string height_duck = "0";

		switch(character)
		{
		case 0:
		case 1:
		case 2:
		case 3:
				//normal
				width = hitBoxFile.Get("frame-5-0", "width", "-1");
				height = hitBoxFile.Get("frame-5-0", "height", "-1");
				//duck
				height_duck = hitBoxFile.Get("frame-5-6", "height", "-1");
			break;
		case 4:
				//normal
				width = hitBoxFile.Get("frame-5-0", "width", "-1");
				height = hitBoxFile.Get("frame-5-0", "height", "-1");
				//duck
				height_duck = hitBoxFile.Get("frame-5-4", "height", "-1");
			break;
		default:
			MessageBoxA(0, "Wrong character ID", "Error", 0);
			return;
		}

		if(atoi(width.c_str()) > 1)
			hitbox_width[powerup*5+character] = (short)atoi(width.c_str());
		if(atoi(height.c_str()) > 1)
			hitbox_height[powerup*5+character] = (short)atoi(height.c_str());
		if(atoi(height_duck.c_str()) > 1)
			hitbox_height_duck[powerup*5+character] = (short)atoi(height_duck.c_str());
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
    ::NPC::Get(m_index)->Ypos = y;
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

void LuaProxy::NPC::kill()
{
    NPCMOB* mnpc = ::NPC::Get(m_index);
    void* ptr = ((&(*(byte*)mnpc)) + 290);
    MemAssign((int)ptr, 1.0, OP_Assign, (FIELDTYPE)FT_WORD);
}

void LuaProxy::NPC::kill(int killEffectID)
{
    NPCMOB* mnpc = ::NPC::Get(m_index);
    void* ptr = ((&(*(byte*)mnpc)) + 290);
    MemAssign((int)ptr, (double)killEffectID, OP_Assign, (FIELDTYPE)FT_WORD);
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

LuaProxy::VBStr LuaProxy::NPC::attachedLayerName()
{
    if(!isValid())
        return VBStr(0);

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)));
    return VBStr(ptr);
}

luabind::object LuaProxy::NPC::attachedLayerObj(lua_State *L)
{
    if(!isValid())
        return luabind::object();

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)));
    return findlayer(utf8_encode(std::wstring(ptr)).c_str(),L);
}

LuaProxy::VBStr LuaProxy::NPC::activateEventName()
{
    if(!isValid())
        return VBStr(0);

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x2C);
    return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::deathEventName()
{
    if(!isValid())
        return VBStr(0);

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x30);
    return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::talkEventName()
{
    if(!isValid())
        return VBStr(0);

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x34);
    return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::noMoreObjInLayer()
{
    if(!isValid())
        return VBStr(0);

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x38);
    return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::msg()
{
    if(!isValid())
        return VBStr(0);

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x4C);
    return VBStr(ptr);
}

LuaProxy::VBStr LuaProxy::NPC::layerName()
{
    if(!isValid())
        return VBStr(0);

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x3C);
    return VBStr(ptr);
}

luabind::object LuaProxy::NPC::layerObj(lua_State *L)
{
    if(!isValid())
        return luabind::object();

    NPCMOB* thisnpc = ::NPC::Get(m_index);
    wchar_t* ptr = *(wchar_t**)((&(*(byte*)thisnpc)) + 0x3C);
    return findlayer(utf8_encode(std::wstring(ptr)).c_str(),L);
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


void LuaProxy::triggerEvent(const char *evName)
{
    SMBXEvents::TriggerEvent(utf8_decode(std::string(evName)), 0);

}


void LuaProxy::playSFX(int index)
{
    SMBXSound::PlaySFX(index);
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


void LuaProxy::playMusic(int section)
{
    SMBXSound::PlayMusic(section);
}


void LuaProxy::loadImage(const char *filename, int resNumber, int transColor)
{
    gLunaRender.LoadBitmapResource(utf8_decode(std::string(filename)), resNumber, transColor);
}


void LuaProxy::placeSprite(int type, int imgResource, int xPos, int yPos, const char *extra, int time)
{
    CSpriteRequest req;
    req.type = type;
    req.img_resource_code = imgResource;
    req.x = xPos;
    req.y = yPos;
    req.time = time;
    req.str = utf8_decode(std::string(extra));
    gSpriteMan.InstantiateSprite(&req, false);
}

void LuaProxy::placeSprite(int type, int imgResource, int xPos, int yPos, const char *extra)
{
    placeSprite(type, imgResource, xPos, yPos, extra, 0);
}


void LuaProxy::placeSprite(int type, int imgResource, int xPos, int yPos)
{
    placeSprite(type, imgResource, xPos, yPos, "");
}


/* Generated by code! [START]*/
short LuaProxy::Player::toadDoubleJReady()
{
    return ::Player::Get(m_index)->ToadDoubleJReady;
}

void LuaProxy::Player::setToadDoubleJReady(short var_toadDoubleJReady)
{
    ::Player::Get(m_index)->ToadDoubleJReady = var_toadDoubleJReady;
}

short LuaProxy::Player::sparklingEffect()
{
    return ::Player::Get(m_index)->SparklingEffect;
}

void LuaProxy::Player::setSparklingEffect(short var_sparklingEffect)
{
    ::Player::Get(m_index)->SparklingEffect = var_sparklingEffect;
}

short LuaProxy::Player::unknownCTRLLock1()
{
    return ::Player::Get(m_index)->UnknownCTRLLock1;
}

void LuaProxy::Player::setUnknownCTRLLock1(short var_unknownCTRLLock1)
{
    ::Player::Get(m_index)->UnknownCTRLLock1 = var_unknownCTRLLock1;
}

short LuaProxy::Player::unknownCTRLLock2()
{
    return ::Player::Get(m_index)->UnknownCTRLLock2;
}

void LuaProxy::Player::setUnknownCTRLLock2(short var_unknownCTRLLock2)
{
    ::Player::Get(m_index)->UnknownCTRLLock2 = var_unknownCTRLLock2;
}

short LuaProxy::Player::quicksandEffectTimer()
{
    return ::Player::Get(m_index)->QuicksandEffectTimer;
}

void LuaProxy::Player::setQuicksandEffectTimer(short var_quicksandEffectTimer)
{
    ::Player::Get(m_index)->QuicksandEffectTimer = var_quicksandEffectTimer;
}

short LuaProxy::Player::onSlipperyGround()
{
    return ::Player::Get(m_index)->OnSlipperyGround;
}

void LuaProxy::Player::setOnSlipperyGround(short var_onSlipperyGround)
{
    ::Player::Get(m_index)->OnSlipperyGround = var_onSlipperyGround;
}

short LuaProxy::Player::isAFairy()
{
    return ::Player::Get(m_index)->IsAFairy;
}

void LuaProxy::Player::setIsAFairy(short var_isAFairy)
{
    ::Player::Get(m_index)->IsAFairy = var_isAFairy;
}

short LuaProxy::Player::fairyAlreadyInvoked()
{
    return ::Player::Get(m_index)->FairyAlreadyInvoked;
}

void LuaProxy::Player::setFairyAlreadyInvoked(short var_fairyAlreadyInvoked)
{
    ::Player::Get(m_index)->FairyAlreadyInvoked = var_fairyAlreadyInvoked;
}

short LuaProxy::Player::fairyFramesLeft()
{
    return ::Player::Get(m_index)->FairyFramesLeft;
}

void LuaProxy::Player::setFairyFramesLeft(short var_fairyFramesLeft)
{
    ::Player::Get(m_index)->FairyFramesLeft = var_fairyFramesLeft;
}

short LuaProxy::Player::sheathHasKey()
{
    return ::Player::Get(m_index)->SheathHasKey;
}

void LuaProxy::Player::setSheathHasKey(short var_sheathHasKey)
{
    ::Player::Get(m_index)->SheathHasKey = var_sheathHasKey;
}

short LuaProxy::Player::sheathAttackCooldown()
{
    return ::Player::Get(m_index)->SheathAttackCooldown;
}

void LuaProxy::Player::setSheathAttackCooldown(short var_sheathAttackCooldown)
{
    ::Player::Get(m_index)->SheathAttackCooldown = var_sheathAttackCooldown;
}

short LuaProxy::Player::hearts()
{
    return ::Player::Get(m_index)->Hearts;
}

void LuaProxy::Player::setHearts(short var_hearts)
{
    ::Player::Get(m_index)->Hearts = var_hearts;
}

short LuaProxy::Player::peachHoverAvailable()
{
    return ::Player::Get(m_index)->PeachHoverAvailable;
}

void LuaProxy::Player::setPeachHoverAvailable(short var_peachHoverAvailable)
{
    ::Player::Get(m_index)->PeachHoverAvailable = var_peachHoverAvailable;
}

short LuaProxy::Player::pressingHoverButton()
{
    return ::Player::Get(m_index)->PressingHoverButton;
}

void LuaProxy::Player::setPressingHoverButton(short var_pressingHoverButton)
{
    ::Player::Get(m_index)->PressingHoverButton = var_pressingHoverButton;
}

short LuaProxy::Player::peachHoverTimer()
{
    return ::Player::Get(m_index)->PeachHoverTimer;
}

void LuaProxy::Player::setPeachHoverTimer(short var_peachHoverTimer)
{
    ::Player::Get(m_index)->PeachHoverTimer = var_peachHoverTimer;
}

short LuaProxy::Player::unused1()
{
    return ::Player::Get(m_index)->Unused1;
}

void LuaProxy::Player::setUnused1(short var_unused1)
{
    ::Player::Get(m_index)->Unused1 = var_unused1;
}

short LuaProxy::Player::peachHoverTrembleSpeed()
{
    return ::Player::Get(m_index)->PeachHoverTrembleSpeed;
}

void LuaProxy::Player::setPeachHoverTrembleSpeed(short var_peachHoverTrembleSpeed)
{
    ::Player::Get(m_index)->PeachHoverTrembleSpeed = var_peachHoverTrembleSpeed;
}

short LuaProxy::Player::peachHoverTrembleDir()
{
    return ::Player::Get(m_index)->PeachHoverTrembleDir;
}

void LuaProxy::Player::setPeachHoverTrembleDir(short var_peachHoverTrembleDir)
{
    ::Player::Get(m_index)->PeachHoverTrembleDir = var_peachHoverTrembleDir;
}

short LuaProxy::Player::itemPullupTimer()
{
    return ::Player::Get(m_index)->ItemPullupTimer;
}

void LuaProxy::Player::setItemPullupTimer(short var_itemPullupTimer)
{
    ::Player::Get(m_index)->ItemPullupTimer = var_itemPullupTimer;
}

float LuaProxy::Player::itemPullupMomentumSave()
{
    return ::Player::Get(m_index)->ItemPullupMomentumSave;
}

void LuaProxy::Player::setItemPullupMomentumSave(float var_itemPullupMomentumSave)
{
    ::Player::Get(m_index)->ItemPullupMomentumSave = var_itemPullupMomentumSave;
}

short LuaProxy::Player::unused2()
{
    return ::Player::Get(m_index)->Unused2;
}

void LuaProxy::Player::setUnused2(short var_unused2)
{
    ::Player::Get(m_index)->Unused2 = var_unused2;
}

short LuaProxy::Player::unkClimbing1()
{
    return ::Player::Get(m_index)->UnkClimbing1;
}

void LuaProxy::Player::setUnkClimbing1(short var_unkClimbing1)
{
    ::Player::Get(m_index)->UnkClimbing1 = var_unkClimbing1;
}

short LuaProxy::Player::unkClimbing2()
{
    return ::Player::Get(m_index)->UnkClimbing2;
}

void LuaProxy::Player::setUnkClimbing2(short var_unkClimbing2)
{
    ::Player::Get(m_index)->UnkClimbing2 = var_unkClimbing2;
}

short LuaProxy::Player::unkClimbing3()
{
    return ::Player::Get(m_index)->UnkClimbing3;
}

void LuaProxy::Player::setUnkClimbing3(short var_unkClimbing3)
{
    ::Player::Get(m_index)->UnkClimbing3 = var_unkClimbing3;
}

short LuaProxy::Player::waterState()
{
    return ::Player::Get(m_index)->WaterState;
}

void LuaProxy::Player::setWaterState(short var_waterState)
{
    ::Player::Get(m_index)->WaterState = var_waterState;
}

short LuaProxy::Player::isInWater()
{
    return ::Player::Get(m_index)->IsInWater;
}

void LuaProxy::Player::setIsInWater(short var_isInWater)
{
    ::Player::Get(m_index)->IsInWater = var_isInWater;
}

short LuaProxy::Player::waterStrokeTimer()
{
    return ::Player::Get(m_index)->WaterStrokeTimer;
}

void LuaProxy::Player::setWaterStrokeTimer(short var_waterStrokeTimer)
{
    ::Player::Get(m_index)->WaterStrokeTimer = var_waterStrokeTimer;
}

short LuaProxy::Player::unknownHoverTimer()
{
    return ::Player::Get(m_index)->UnknownHoverTimer;
}

void LuaProxy::Player::setUnknownHoverTimer(short var_unknownHoverTimer)
{
    ::Player::Get(m_index)->UnknownHoverTimer = var_unknownHoverTimer;
}

short LuaProxy::Player::slidingState()
{
    return ::Player::Get(m_index)->SlidingState;
}

void LuaProxy::Player::setSlidingState(short var_slidingState)
{
    ::Player::Get(m_index)->SlidingState = var_slidingState;
}

short LuaProxy::Player::slidingGroundPuffs()
{
    return ::Player::Get(m_index)->SlidingGroundPuffs;
}

void LuaProxy::Player::setSlidingGroundPuffs(short var_slidingGroundPuffs)
{
    ::Player::Get(m_index)->SlidingGroundPuffs = var_slidingGroundPuffs;
}

short LuaProxy::Player::climbingState()
{
    return ::Player::Get(m_index)->ClimbingState;
}

void LuaProxy::Player::setClimbingState(short var_climbingState)
{
    ::Player::Get(m_index)->ClimbingState = var_climbingState;
}

short LuaProxy::Player::unknownTimer()
{
    return ::Player::Get(m_index)->UnknownTimer;
}

void LuaProxy::Player::setUnknownTimer(short var_unknownTimer)
{
    ::Player::Get(m_index)->UnknownTimer = var_unknownTimer;
}

short LuaProxy::Player::unknownFlag()
{
    return ::Player::Get(m_index)->UnknownFlag;
}

void LuaProxy::Player::setUnknownFlag(short var_unknownFlag)
{
    ::Player::Get(m_index)->UnknownFlag = var_unknownFlag;
}

short LuaProxy::Player::unknownPowerupState()
{
    return ::Player::Get(m_index)->UnknownPowerupState;
}

void LuaProxy::Player::setUnknownPowerupState(short var_unknownPowerupState)
{
    ::Player::Get(m_index)->UnknownPowerupState = var_unknownPowerupState;
}

short LuaProxy::Player::slopeRelated()
{
    return ::Player::Get(m_index)->SlopeRelated;
}

void LuaProxy::Player::setSlopeRelated(short var_slopeRelated)
{
    ::Player::Get(m_index)->SlopeRelated = var_slopeRelated;
}

short LuaProxy::Player::tanookiStatueActive()
{
    return ::Player::Get(m_index)->TanookiStatueActive;
}

void LuaProxy::Player::setTanookiStatueActive(short var_tanookiStatueActive)
{
    ::Player::Get(m_index)->TanookiStatueActive = var_tanookiStatueActive;
}

short LuaProxy::Player::tanookiMorphCooldown()
{
    return ::Player::Get(m_index)->TanookiMorphCooldown;
}

void LuaProxy::Player::setTanookiMorphCooldown(short var_tanookiMorphCooldown)
{
    ::Player::Get(m_index)->TanookiMorphCooldown = var_tanookiMorphCooldown;
}

short LuaProxy::Player::tanookiActiveFrameCount()
{
    return ::Player::Get(m_index)->TanookiActiveFrameCount;
}

void LuaProxy::Player::setTanookiActiveFrameCount(short var_tanookiActiveFrameCount)
{
    ::Player::Get(m_index)->TanookiActiveFrameCount = var_tanookiActiveFrameCount;
}

short LuaProxy::Player::isSpinjumping()
{
    return ::Player::Get(m_index)->IsSpinjumping;
}

void LuaProxy::Player::setIsSpinjumping(short var_isSpinjumping)
{
    ::Player::Get(m_index)->IsSpinjumping = var_isSpinjumping;
}

short LuaProxy::Player::spinjumpStateCounter()
{
    return ::Player::Get(m_index)->SpinjumpStateCounter;
}

void LuaProxy::Player::setSpinjumpStateCounter(short var_spinjumpStateCounter)
{
    ::Player::Get(m_index)->SpinjumpStateCounter = var_spinjumpStateCounter;
}

short LuaProxy::Player::spinjumpLandDirection()
{
    return ::Player::Get(m_index)->SpinjumpLandDirection;
}

void LuaProxy::Player::setSpinjumpLandDirection(short var_spinjumpLandDirection)
{
    ::Player::Get(m_index)->SpinjumpLandDirection = var_spinjumpLandDirection;
}

short LuaProxy::Player::currentKillCombo()
{
    return ::Player::Get(m_index)->CurrentKillCombo;
}

void LuaProxy::Player::setCurrentKillCombo(short var_currentKillCombo)
{
    ::Player::Get(m_index)->CurrentKillCombo = var_currentKillCombo;
}

short LuaProxy::Player::groundSlidingPuffsState()
{
    return ::Player::Get(m_index)->GroundSlidingPuffsState;
}

void LuaProxy::Player::setGroundSlidingPuffsState(short var_groundSlidingPuffsState)
{
    ::Player::Get(m_index)->GroundSlidingPuffsState = var_groundSlidingPuffsState;
}

short LuaProxy::Player::warpNearby()
{
    return ::Player::Get(m_index)->WarpNearby;
}

void LuaProxy::Player::setWarpNearby(short var_warpNearby)
{
    ::Player::Get(m_index)->WarpNearby = var_warpNearby;
}

short LuaProxy::Player::unknown5C()
{
    return ::Player::Get(m_index)->Unknown5C;
}

void LuaProxy::Player::setUnknown5C(short var_unknown5C)
{
    ::Player::Get(m_index)->Unknown5C = var_unknown5C;
}

short LuaProxy::Player::unknown5E()
{
    return ::Player::Get(m_index)->Unknown5E;
}

void LuaProxy::Player::setUnknown5E(short var_unknown5E)
{
    ::Player::Get(m_index)->Unknown5E = var_unknown5E;
}

short LuaProxy::Player::hasJumped()
{
    return ::Player::Get(m_index)->HasJumped;
}

void LuaProxy::Player::setHasJumped(short var_hasJumped)
{
    ::Player::Get(m_index)->HasJumped = var_hasJumped;
}

double LuaProxy::Player::curXPos()
{
    return ::Player::Get(m_index)->CurXPos;
}

void LuaProxy::Player::setCurXPos(double var_curXPos)
{
    ::Player::Get(m_index)->CurXPos = var_curXPos;
}

double LuaProxy::Player::curYPos()
{
    return ::Player::Get(m_index)->CurYPos;
}

void LuaProxy::Player::setCurYPos(double var_curYPos)
{
    ::Player::Get(m_index)->CurYPos = var_curYPos;
}

double LuaProxy::Player::height()
{
    return ::Player::Get(m_index)->Height;
}

void LuaProxy::Player::setHeight(double var_height)
{
    ::Player::Get(m_index)->Height = var_height;
}

double LuaProxy::Player::width()
{
    return ::Player::Get(m_index)->Width;
}

void LuaProxy::Player::setWidth(double var_width)
{
    ::Player::Get(m_index)->Width = var_width;
}

double LuaProxy::Player::curXSpeed()
{
    return ::Player::Get(m_index)->CurXSpeed;
}

void LuaProxy::Player::setCurXSpeed(double var_curXSpeed)
{
    ::Player::Get(m_index)->CurXSpeed = var_curXSpeed;
}

double LuaProxy::Player::curYSpeed()
{
    return ::Player::Get(m_index)->CurYSpeed;
}

void LuaProxy::Player::setCurYSpeed(double var_curYSpeed)
{
    ::Player::Get(m_index)->CurYSpeed = var_curYSpeed;
}

short LuaProxy::Player::identity()
{
    return ::Player::Get(m_index)->Identity;
}

void LuaProxy::Player::setIdentity(short var_identity)
{
    ::Player::Get(m_index)->Identity = var_identity;
}

short LuaProxy::Player::uKeyState()
{
    return ::Player::Get(m_index)->UKeyState;
}

void LuaProxy::Player::setUKeyState(short var_uKeyState)
{
    ::Player::Get(m_index)->UKeyState = var_uKeyState;
}

short LuaProxy::Player::dKeyState()
{
    return ::Player::Get(m_index)->DKeyState;
}

void LuaProxy::Player::setDKeyState(short var_dKeyState)
{
    ::Player::Get(m_index)->DKeyState = var_dKeyState;
}

short LuaProxy::Player::lKeyState()
{
    return ::Player::Get(m_index)->LKeyState;
}

void LuaProxy::Player::setLKeyState(short var_lKeyState)
{
    ::Player::Get(m_index)->LKeyState = var_lKeyState;
}

short LuaProxy::Player::rKeyState()
{
    return ::Player::Get(m_index)->RKeyState;
}

void LuaProxy::Player::setRKeyState(short var_rKeyState)
{
    ::Player::Get(m_index)->RKeyState = var_rKeyState;
}

short LuaProxy::Player::jKeyState()
{
    return ::Player::Get(m_index)->JKeyState;
}

void LuaProxy::Player::setJKeyState(short var_jKeyState)
{
    ::Player::Get(m_index)->JKeyState = var_jKeyState;
}

short LuaProxy::Player::sJKeyState()
{
    return ::Player::Get(m_index)->SJKeyState;
}

void LuaProxy::Player::setSJKeyState(short var_sJKeyState)
{
    ::Player::Get(m_index)->SJKeyState = var_sJKeyState;
}

short LuaProxy::Player::xKeyState()
{
    return ::Player::Get(m_index)->XKeyState;
}

void LuaProxy::Player::setXKeyState(short var_xKeyState)
{
    ::Player::Get(m_index)->XKeyState = var_xKeyState;
}

short LuaProxy::Player::rNKeyState()
{
    return ::Player::Get(m_index)->RNKeyState;
}

void LuaProxy::Player::setRNKeyState(short var_rNKeyState)
{
    ::Player::Get(m_index)->RNKeyState = var_rNKeyState;
}

short LuaProxy::Player::sELKeyState()
{
    return ::Player::Get(m_index)->SELKeyState;
}

void LuaProxy::Player::setSELKeyState(short var_sELKeyState)
{
    ::Player::Get(m_index)->SELKeyState = var_sELKeyState;
}

short LuaProxy::Player::sTRKeyState()
{
    return ::Player::Get(m_index)->STRKeyState;
}

void LuaProxy::Player::setSTRKeyState(short var_sTRKeyState)
{
    ::Player::Get(m_index)->STRKeyState = var_sTRKeyState;
}

short LuaProxy::Player::facingDirection()
{
    return ::Player::Get(m_index)->FacingDirection;
}

void LuaProxy::Player::setFacingDirection(short var_facingDirection)
{
    ::Player::Get(m_index)->FacingDirection = var_facingDirection;
}

short LuaProxy::Player::mountType()
{
    return ::Player::Get(m_index)->MountType;
}

void LuaProxy::Player::setMountType(short var_mountType)
{
    ::Player::Get(m_index)->MountType = var_mountType;
}

short LuaProxy::Player::mountColor()
{
    return ::Player::Get(m_index)->MountColor;
}

void LuaProxy::Player::setMountColor(short var_mountColor)
{
    ::Player::Get(m_index)->MountColor = var_mountColor;
}

short LuaProxy::Player::mountState()
{
    return ::Player::Get(m_index)->MountState;
}

void LuaProxy::Player::setMountState(short var_mountState)
{
    ::Player::Get(m_index)->MountState = var_mountState;
}

short LuaProxy::Player::mountHeightOffset()
{
    return ::Player::Get(m_index)->MountHeightOffset;
}

void LuaProxy::Player::setMountHeightOffset(short var_mountHeightOffset)
{
    ::Player::Get(m_index)->MountHeightOffset = var_mountHeightOffset;
}

short LuaProxy::Player::mountGfxIndex()
{
    return ::Player::Get(m_index)->MountGfxIndex;
}

void LuaProxy::Player::setMountGfxIndex(short var_mountGfxIndex)
{
    ::Player::Get(m_index)->MountGfxIndex = var_mountGfxIndex;
}

short LuaProxy::Player::currentPowerup()
{
    return ::Player::Get(m_index)->CurrentPowerup;
}

void LuaProxy::Player::setCurrentPowerup(short var_currentPowerup)
{
    ::Player::Get(m_index)->CurrentPowerup = var_currentPowerup;
}

short LuaProxy::Player::currentPlayerSprite()
{
    return ::Player::Get(m_index)->CurrentPlayerSprite;
}

void LuaProxy::Player::setCurrentPlayerSprite(short var_currentPlayerSprite)
{
    ::Player::Get(m_index)->CurrentPlayerSprite = var_currentPlayerSprite;
}

short LuaProxy::Player::unused116()
{
    return ::Player::Get(m_index)->Unused116;
}

void LuaProxy::Player::setUnused116(short var_unused116)
{
    ::Player::Get(m_index)->Unused116 = var_unused116;
}

float LuaProxy::Player::gfxMirrorX()
{
    return ::Player::Get(m_index)->GfxMirrorX;
}

void LuaProxy::Player::setGfxMirrorX(float var_gfxMirrorX)
{
    ::Player::Get(m_index)->GfxMirrorX = var_gfxMirrorX;
}

short LuaProxy::Player::upwardJumpingForce()
{
    return ::Player::Get(m_index)->UpwardJumpingForce;
}

void LuaProxy::Player::setUpwardJumpingForce(short var_upwardJumpingForce)
{
    ::Player::Get(m_index)->UpwardJumpingForce = var_upwardJumpingForce;
}

short LuaProxy::Player::jumpButtonHeld()
{
    return ::Player::Get(m_index)->JumpButtonHeld;
}

void LuaProxy::Player::setJumpButtonHeld(short var_jumpButtonHeld)
{
    ::Player::Get(m_index)->JumpButtonHeld = var_jumpButtonHeld;
}

short LuaProxy::Player::spinjumpButtonHeld()
{
    return ::Player::Get(m_index)->SpinjumpButtonHeld;
}

void LuaProxy::Player::setSpinjumpButtonHeld(short var_spinjumpButtonHeld)
{
    ::Player::Get(m_index)->SpinjumpButtonHeld = var_spinjumpButtonHeld;
}

short LuaProxy::Player::forcedAnimationState()
{
    return ::Player::Get(m_index)->ForcedAnimationState;
}

void LuaProxy::Player::setForcedAnimationState(short var_forcedAnimationState)
{
    ::Player::Get(m_index)->ForcedAnimationState = var_forcedAnimationState;
}

float LuaProxy::Player::unknown124()
{
    return ::Player::Get(m_index)->Unknown124;
}

void LuaProxy::Player::setUnknown124(float var_unknown124)
{
    ::Player::Get(m_index)->Unknown124 = var_unknown124;
}

float LuaProxy::Player::unknown128()
{
    return ::Player::Get(m_index)->Unknown128;
}

void LuaProxy::Player::setUnknown128(float var_unknown128)
{
    ::Player::Get(m_index)->Unknown128 = var_unknown128;
}

short LuaProxy::Player::downButtonMirror()
{
    return ::Player::Get(m_index)->DownButtonMirror;
}

void LuaProxy::Player::setDownButtonMirror(short var_downButtonMirror)
{
    ::Player::Get(m_index)->DownButtonMirror = var_downButtonMirror;
}

short LuaProxy::Player::inDuckingPosition()
{
    return ::Player::Get(m_index)->InDuckingPosition;
}

void LuaProxy::Player::setInDuckingPosition(short var_inDuckingPosition)
{
    ::Player::Get(m_index)->InDuckingPosition = var_inDuckingPosition;
}

short LuaProxy::Player::selectButtonMirror()
{
    return ::Player::Get(m_index)->SelectButtonMirror;
}

void LuaProxy::Player::setSelectButtonMirror(short var_selectButtonMirror)
{
    ::Player::Get(m_index)->SelectButtonMirror = var_selectButtonMirror;
}

short LuaProxy::Player::unknown132()
{
    return ::Player::Get(m_index)->Unknown132;
}

void LuaProxy::Player::setUnknown132(short var_unknown132)
{
    ::Player::Get(m_index)->Unknown132 = var_unknown132;
}

short LuaProxy::Player::downButtonTapped()
{
    return ::Player::Get(m_index)->DownButtonTapped;
}

void LuaProxy::Player::setDownButtonTapped(short var_downButtonTapped)
{
    ::Player::Get(m_index)->DownButtonTapped = var_downButtonTapped;
}

short LuaProxy::Player::unknown136()
{
    return ::Player::Get(m_index)->Unknown136;
}

void LuaProxy::Player::setUnknown136(short var_unknown136)
{
    ::Player::Get(m_index)->Unknown136 = var_unknown136;
}

float LuaProxy::Player::xMomentumPush()
{
    return ::Player::Get(m_index)->XMomentumPush;
}

void LuaProxy::Player::setXMomentumPush(float var_xMomentumPush)
{
    ::Player::Get(m_index)->XMomentumPush = var_xMomentumPush;
}

short LuaProxy::Player::deathState()
{
    return ::Player::Get(m_index)->DeathState;
}

void LuaProxy::Player::setDeathState(short var_deathState)
{
    ::Player::Get(m_index)->DeathState = var_deathState;
}

short LuaProxy::Player::deathTimer()
{
    return ::Player::Get(m_index)->DeathTimer;
}

void LuaProxy::Player::setDeathTimer(short var_deathTimer)
{
    ::Player::Get(m_index)->DeathTimer = var_deathTimer;
}

short LuaProxy::Player::blinkTimer()
{
    return ::Player::Get(m_index)->BlinkTimer;
}

void LuaProxy::Player::setBlinkTimer(short var_blinkTimer)
{
    ::Player::Get(m_index)->BlinkTimer = var_blinkTimer;
}

short LuaProxy::Player::blinkState()
{
    return ::Player::Get(m_index)->BlinkState;
}

void LuaProxy::Player::setBlinkState(short var_blinkState)
{
    ::Player::Get(m_index)->BlinkState = var_blinkState;
}

short LuaProxy::Player::unknown144()
{
    return ::Player::Get(m_index)->Unknown144;
}

void LuaProxy::Player::setUnknown144(short var_unknown144)
{
    ::Player::Get(m_index)->Unknown144 = var_unknown144;
}

short LuaProxy::Player::layerStateStanding()
{
    return ::Player::Get(m_index)->LayerStateStanding;
}

void LuaProxy::Player::setLayerStateStanding(short var_layerStateStanding)
{
    ::Player::Get(m_index)->LayerStateStanding = var_layerStateStanding;
}

short LuaProxy::Player::layerStateLeftContact()
{
    return ::Player::Get(m_index)->LayerStateLeftContact;
}

void LuaProxy::Player::setLayerStateLeftContact(short var_layerStateLeftContact)
{
    ::Player::Get(m_index)->LayerStateLeftContact = var_layerStateLeftContact;
}

short LuaProxy::Player::layerStateTopContact()
{
    return ::Player::Get(m_index)->LayerStateTopContact;
}

void LuaProxy::Player::setLayerStateTopContact(short var_layerStateTopContact)
{
    ::Player::Get(m_index)->LayerStateTopContact = var_layerStateTopContact;
}

short LuaProxy::Player::layerStateRightContact()
{
    return ::Player::Get(m_index)->LayerStateRightContact;
}

void LuaProxy::Player::setLayerStateRightContact(short var_layerStateRightContact)
{
    ::Player::Get(m_index)->LayerStateRightContact = var_layerStateRightContact;
}

short LuaProxy::Player::pushedByMovingLayer()
{
    return ::Player::Get(m_index)->PushedByMovingLayer;
}

void LuaProxy::Player::setPushedByMovingLayer(short var_pushedByMovingLayer)
{
    ::Player::Get(m_index)->PushedByMovingLayer = var_pushedByMovingLayer;
}

short LuaProxy::Player::unused150()
{
    return ::Player::Get(m_index)->Unused150;
}

void LuaProxy::Player::setUnused150(short var_unused150)
{
    ::Player::Get(m_index)->Unused150 = var_unused150;
}

short LuaProxy::Player::unused152()
{
    return ::Player::Get(m_index)->Unused152;
}

void LuaProxy::Player::setUnused152(short var_unused152)
{
    ::Player::Get(m_index)->Unused152 = var_unused152;
}

short LuaProxy::Player::heldNPCIndex()
{
    return ::Player::Get(m_index)->HeldNPCIndex;
}

void LuaProxy::Player::setHeldNPCIndex(short var_heldNPCIndex)
{
    ::Player::Get(m_index)->HeldNPCIndex = var_heldNPCIndex;
}

short LuaProxy::Player::unknown156()
{
    return ::Player::Get(m_index)->Unknown156;
}

void LuaProxy::Player::setUnknown156(short var_unknown156)
{
    ::Player::Get(m_index)->Unknown156 = var_unknown156;
}

short LuaProxy::Player::powerupBoxContents()
{
    return ::Player::Get(m_index)->PowerupBoxContents;
}

void LuaProxy::Player::setPowerupBoxContents(short var_powerupBoxContents)
{
    ::Player::Get(m_index)->PowerupBoxContents = var_powerupBoxContents;
}

short LuaProxy::Player::currentSection()
{
    return ::Player::Get(m_index)->CurrentSection;
}

void LuaProxy::Player::setCurrentSection(short var_currentSection)
{
    ::Player::Get(m_index)->CurrentSection = var_currentSection;
}

short LuaProxy::Player::warpTimer()
{
    return ::Player::Get(m_index)->WarpTimer;
}

void LuaProxy::Player::setWarpTimer(short var_warpTimer)
{
    ::Player::Get(m_index)->WarpTimer = var_warpTimer;
}

short LuaProxy::Player::unknown15E()
{
    return ::Player::Get(m_index)->Unknown15E;
}

void LuaProxy::Player::setUnknown15E(short var_unknown15E)
{
    ::Player::Get(m_index)->Unknown15E = var_unknown15E;
}

short LuaProxy::Player::projectileTimer1()
{
    return ::Player::Get(m_index)->ProjectileTimer1;
}

void LuaProxy::Player::setProjectileTimer1(short var_projectileTimer1)
{
    ::Player::Get(m_index)->ProjectileTimer1 = var_projectileTimer1;
}

short LuaProxy::Player::projectileTimer2()
{
    return ::Player::Get(m_index)->ProjectileTimer2;
}

void LuaProxy::Player::setProjectileTimer2(short var_projectileTimer2)
{
    ::Player::Get(m_index)->ProjectileTimer2 = var_projectileTimer2;
}

short LuaProxy::Player::tailswipeTimer()
{
    return ::Player::Get(m_index)->TailswipeTimer;
}

void LuaProxy::Player::setTailswipeTimer(short var_tailswipeTimer)
{
    ::Player::Get(m_index)->TailswipeTimer = var_tailswipeTimer;
}

short LuaProxy::Player::unknown166()
{
    return ::Player::Get(m_index)->Unknown166;
}

void LuaProxy::Player::setUnknown166(short var_unknown166)
{
    ::Player::Get(m_index)->Unknown166 = var_unknown166;
}

float LuaProxy::Player::takeoffSpeed()
{
    return ::Player::Get(m_index)->TakeoffSpeed;
}

void LuaProxy::Player::setTakeoffSpeed(float var_takeoffSpeed)
{
    ::Player::Get(m_index)->TakeoffSpeed = var_takeoffSpeed;
}

short LuaProxy::Player::canFly()
{
    return ::Player::Get(m_index)->CanFly;
}

void LuaProxy::Player::setCanFly(short var_canFly)
{
    ::Player::Get(m_index)->CanFly = var_canFly;
}

short LuaProxy::Player::isFlying()
{
    return ::Player::Get(m_index)->IsFlying;
}

void LuaProxy::Player::setIsFlying(short var_isFlying)
{
    ::Player::Get(m_index)->IsFlying = var_isFlying;
}

short LuaProxy::Player::flightTimeRemaining()
{
    return ::Player::Get(m_index)->FlightTimeRemaining;
}

void LuaProxy::Player::setFlightTimeRemaining(short var_flightTimeRemaining)
{
    ::Player::Get(m_index)->FlightTimeRemaining = var_flightTimeRemaining;
}

short LuaProxy::Player::holdingFlightRunButton()
{
    return ::Player::Get(m_index)->HoldingFlightRunButton;
}

void LuaProxy::Player::setHoldingFlightRunButton(short var_holdingFlightRunButton)
{
    ::Player::Get(m_index)->HoldingFlightRunButton = var_holdingFlightRunButton;
}

short LuaProxy::Player::holdingFlightButton()
{
    return ::Player::Get(m_index)->HoldingFlightButton;
}

void LuaProxy::Player::setHoldingFlightButton(short var_holdingFlightButton)
{
    ::Player::Get(m_index)->HoldingFlightButton = var_holdingFlightButton;
}

short LuaProxy::Player::nPCBeingStoodOnIndex()
{
    return ::Player::Get(m_index)->NPCBeingStoodOnIndex;
}

void LuaProxy::Player::setNPCBeingStoodOnIndex(short var_nPCBeingStoodOnIndex)
{
    ::Player::Get(m_index)->NPCBeingStoodOnIndex = var_nPCBeingStoodOnIndex;
}

short LuaProxy::Player::unknown178()
{
    return ::Player::Get(m_index)->Unknown178;
}

void LuaProxy::Player::setUnknown178(short var_unknown178)
{
    ::Player::Get(m_index)->Unknown178 = var_unknown178;
}

short LuaProxy::Player::unknown17A()
{
    return ::Player::Get(m_index)->Unknown17A;
}

void LuaProxy::Player::setUnknown17A(short var_unknown17A)
{
    ::Player::Get(m_index)->Unknown17A = var_unknown17A;
}

short LuaProxy::Player::unused17C()
{
    return ::Player::Get(m_index)->Unused17C;
}

void LuaProxy::Player::setUnused17C(short var_unused17C)
{
    ::Player::Get(m_index)->Unused17C = var_unused17C;
}

short LuaProxy::Player::unused17E()
{
    return ::Player::Get(m_index)->Unused17E;
}

void LuaProxy::Player::setUnused17E(short var_unused17E)
{
    ::Player::Get(m_index)->Unused17E = var_unused17E;
}

short LuaProxy::Player::unused180()
{
    return ::Player::Get(m_index)->Unused180;
}

void LuaProxy::Player::setUnused180(short var_unused180)
{
    ::Player::Get(m_index)->Unused180 = var_unused180;
}

short LuaProxy::Player::unused182()
{
    return ::Player::Get(m_index)->Unused182;
}

void LuaProxy::Player::setUnused182(short var_unused182)
{
    ::Player::Get(m_index)->Unused182 = var_unused182;
}

short LuaProxy::Player::unused184()
{
    return ::Player::Get(m_index)->Unused184;
}

void LuaProxy::Player::setUnused184(short var_unused184)
{
    ::Player::Get(m_index)->Unused184 = var_unused184;
}

int LuaProxy::Player::getIndex()
{
    return m_index;
}

/* Generated by code! [END]*/


unsigned short LuaProxy::gravity()
{
    return GM_GRAVITY;
}


void LuaProxy::gravity(unsigned short value)
{
    GM_GRAVITY = value;
}


unsigned short LuaProxy::earthquake()
{
    return GM_EARTHQUAKE;
}


void LuaProxy::earthquake(unsigned short value)
{
    GM_EARTHQUAKE = value;
}


unsigned short LuaProxy::jumpheight()
{
    return GM_JUMPHIGHT;
}


void LuaProxy::jumpheight(unsigned short value)
{
    GM_JUMPHIGHT = value;
}


unsigned short LuaProxy::jumpheightBounce()
{
    return GM_JUMPHIGHT_BOUNCE;
}


void LuaProxy::jumpheightBounce(unsigned short value)
{
    GM_JUMPHIGHT_BOUNCE = value;
}


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
    if(iftype >= 1 && iftype <= 5){
        ::Block* pBlock = &::Blocks::GetBase()[m_index];
        void* ptr = ((&(*(byte*)pBlock)) + offset);
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
        return VBStr(0);

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


void LuaProxy::runAnimation(int id, double x, double y, double height, double width, double speedX, double speedY, int extraData)
{
    typedef int animationFunc(int, int, int, int, int);
    animationFunc* f = (animationFunc*)GF_RUN_ANIM;

    coorStruct tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.Height = height;
    tmp.Width = width;
    tmp.XSpeed = speedX;
    tmp.YSpeed = speedY;
    int a4 = 0;
    int a5 = 0;
    f((int)&id, (int)&tmp, (int)&extraData, (int)&a4, (int)&a5);
}


void LuaProxy::npcToCoins()
{
    typedef void npcToCoinsFunc();
    npcToCoinsFunc* f = (npcToCoinsFunc*)GF_NPC_TO_COINS;
    f();
}


luabind::object LuaProxy::blocks(lua_State *L)
{
    luabind::object vblocks = luabind::newtable(L);
    for(int i = 0; i < GM_BLOCK_COUNT; i++) {
        vblocks[i] = new LuaProxy::Block(i);
    }
    return vblocks;
}


luabind::object LuaProxy::findblocks(int ID, lua_State *L)
{
    luabind::object vblocks = luabind::newtable(L);
    int r = 0;

    bool anyID = (ID == -1 ? true : false);
    ::Block* thisblock = NULL;

    for(int i = 0; i < GM_BLOCK_COUNT; i++) {
        thisblock = ::Blocks::Get(i);
        if(thisblock->BlockType == ID || anyID) {
            vblocks[r] = LuaProxy::Block(i);
            ++r;
        }
    }
    return vblocks;
}


LuaProxy::VBStr::VBStr(wchar_t *ptr)
{
    m_wcharptr = ptr;
}

std::string LuaProxy::VBStr::str()
{
    return utf8_encode(std::wstring(m_wcharptr));
}

void LuaProxy::VBStr::setStr(std::string str)
{
    size_t len = wcslen(m_wcharptr);
    std::wstring trimmedStr = utf8_decode(str.substr(0, len));
    const wchar_t* newWStr = trimmedStr.c_str();
    wcscpy(m_wcharptr, newWStr);
}

int LuaProxy::VBStr::length()
{
    if(!m_wcharptr)
        return 0;
    return (*(((short*)m_wcharptr) - 2)/2);
}

void LuaProxy::VBStr::setLength(int len)
{
    if(!m_wcharptr)
        return;
    *(((short*)m_wcharptr) - 2) = len*2;
}

void LuaProxy::VBStr::clear()
{
    *(((short*)m_wcharptr) - 2) = (int)0;
    *(short*)m_wcharptr = (short)0;
}


LuaProxy::Layer::Layer(int layerIndex)
{
    m_layerIndex = layerIndex;
}

LuaProxy::VBStr LuaProxy::Layer::layerName()
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    return VBStr(thislayer->ptLayerName);
}

float LuaProxy::Layer::speedX()
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    return (thislayer->xSpeed == 0.0001f ? 0 : thislayer->xSpeed);
}

void LuaProxy::Layer::setSpeedX(float speedX)
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    ::Layer::SetXSpeed(thislayer, speedX);
}

float LuaProxy::Layer::speedY()
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    return (thislayer->ySpeed == 0.0001f ? 0 : thislayer->ySpeed);
}

void LuaProxy::Layer::setSpeedY(float speedY)
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    ::Layer::SetYSpeed(thislayer, speedY);
}

void LuaProxy::Layer::stop()
{
    LayerControl* thislayer = ::Layer::Get(m_layerIndex);
    ::Layer::Stop(thislayer);
}



luabind::object LuaProxy::findlayer(const char *layername, lua_State *L)
{
    for(int i = 1; i < 100; ++i){
        LayerControl* ctrl = ::Layer::Get(i);
        if(ctrl){
            std::wstring tarLayerName = utf8_decode(std::string(layername));
            if(!ctrl->ptLayerName)
				continue;
			std::wstring sourceLayerName(ctrl->ptLayerName);
            if(tarLayerName == sourceLayerName){
                return luabind::object(L, new Layer(i));
            }
        }
    }
    return luabind::object();
}


void LuaProxy::exitLevel()
{
    GM_LEAVE_LEVEL = 0xFFFF;
}





unsigned short LuaProxy::winState()
{
    return GM_WINNING;
}


void LuaProxy::winState(unsigned short value)
{
    GM_WINNING = value;
}


LuaProxy::Animation::Animation(int animationIndex)
{
    m_animationIndex = animationIndex;
}

void LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, luabind::object value)
{
    int iftype = (int)ftype;
    if(iftype >= 1 && iftype <= 5){
        SMBXAnimation* manimation = ::Animations::Get(m_animationIndex);
        void* ptr = ((&(*(byte*)manimation)) + offset);
        MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
    }
}

luabind::object LuaProxy::Animation::mem(int offset, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
    int iftype = (int)ftype;
    double val = 0;
    if(iftype >= 1 && iftype <= 5){
        SMBXAnimation* manimation = ::Animations::Get(m_animationIndex);
        void* ptr = ((&(*(byte*)manimation)) + offset);
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

short LuaProxy::Animation::id()
{
    if(!isValid())
        return 0;
    return ::Animations::Get(m_animationIndex)->AnimationID;
}

void LuaProxy::Animation::setId(short id)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->AnimationID = id;
}

double LuaProxy::Animation::x()
{
    if(!isValid())
        return 0;
    return ::Animations::Get(m_animationIndex)->XPos;
}

void LuaProxy::Animation::setX(double x)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->XPos = x;
}

double LuaProxy::Animation::y()
{
    if(!isValid())
        return 0;
    return ::Animations::Get(m_animationIndex)->YPos;
}

void LuaProxy::Animation::setY(double y)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->YPos = y;
}

double LuaProxy::Animation::speedX()
{
    if(!isValid())
        return 0;
    return ::Animations::Get(m_animationIndex)->XSpeed;
}

void LuaProxy::Animation::setSpeedX(double speedX)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->XSpeed = speedX;
}

double LuaProxy::Animation::speedY()
{
    if(!isValid())
        return 0;
    return ::Animations::Get(m_animationIndex)->YSpeed;
}

void LuaProxy::Animation::setSpeedY(double speedY)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->YSpeed = speedY;
}

double LuaProxy::Animation::height()
{
    if(!isValid())
        return 0;
    return ::Animations::Get(m_animationIndex)->Height;
}

void LuaProxy::Animation::setHeight(double height)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->Height = height;
}

double LuaProxy::Animation::width()
{
    if(!isValid())
        return 0;
    return ::Animations::Get(m_animationIndex)->Width;
}

void LuaProxy::Animation::setWidth(double width)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->Width = width;
}

short LuaProxy::Animation::timer()
{
    return ::Animations::Get(m_animationIndex)->Timer;
}

void LuaProxy::Animation::setTimer(short timer)
{
    if(!isValid())
        return;
    ::Animations::Get(m_animationIndex)->Timer = timer;

}

bool LuaProxy::Animation::isValid()
{
    return !(m_animationIndex < 0 || m_animationIndex > GM_NPCS_COUNT);
}


luabind::object LuaProxy::animations(lua_State *L)
{
    luabind::object vanimations = luabind::newtable(L);
    for(int i = 0; i < GM_ANIM_COUNT; i++) {
        vanimations[i] = new LuaProxy::Animation(i);
    }
    return vanimations;
}


void LuaProxy::runAnimation(int id, double x, double y, double height, double width, int extraData)
{
    typedef int animationFunc(int, int, int, int, int);
    animationFunc* f = (animationFunc*)GF_RUN_ANIM;

    coorStruct tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.Height = height;
    tmp.Width = width;
    tmp.XSpeed = 0;
    tmp.YSpeed = 0;
    int a4 = 0;
    int a5 = 0;
    f((int)&id, (int)&tmp, (int)&extraData, (int)&a4, (int)&a5);
}


void LuaProxy::runAnimation(int id, double x, double y, int extraData)
{
    typedef int animationFunc(int, int, int, int, int);
    animationFunc* f = (animationFunc*)GF_RUN_ANIM;

    coorStruct tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.Height = 0;
    tmp.Width = 0;
    tmp.XSpeed = 0;
    tmp.YSpeed = 0;
    int a4 = 0;
    int a5 = 0;
    f((int)&id, (int)&tmp, (int)&extraData, (int)&a4, (int)&a5);
}


LuaProxy::VBStr LuaProxy::getInput()
{
    return VBStr((wchar_t*)GM_INPUTSTR_BUF_PTR);
}
