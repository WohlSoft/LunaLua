#include "../LuaProxy.h"
#include "../../Rendering/Rendering.h"
#include "../../libs/ini-reader/INIReader.h"
#include "../../SMBXInternal/PlayerMOB.h"
#include "../../SMBXInternal/NPCs.h"
#include "../../SMBXInternal/Animation.h"
#include "../../SMBXInternal/Blocks.h"
#include "../../SMBXInternal/Layer.h"
#include "../../SMBXInternal/Sound.h"
#include "../../SMBXInternal/SMBXEvents.h"
#include "../../SMBXInternal/Overworld.h"
#include "../../SMBXInternal/WorldLevel.h"
#include "../../SMBXInternal/Level.h"
#include "../../SMBXInternal/CustomGraphics.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/SdlMusPlayer.h"
#include "../../Misc/RuntimeHook.h"
#include "LuaProxyAudio.h"
#include <sstream>


//type - Player's state/powerup
//ini_file - path to INI-file which contains the hitbox redefinations
void LuaProxy::loadHitboxes(int _character, int _powerup, const std::string& ini_file, lua_State* L)
{
    if ((_powerup < 1) || (_powerup>7)) {
        luaL_error(L, "Powerup ID must be from 1 to 7.");
        return;
    }
    if ((_character < 1) || (_character > 5)) {
        luaL_error(L, "Character ID must be from 1 to 5.");
        return;
    }

    int powerup = _powerup - 1;
    int character = _character - 1;

    std::wstring full_path;
    if (isAbsolutePath(ini_file))
    {
        full_path = utf8_decode(ini_file);
    } else {
        std::wstring world_dir = (wstring)GM_FULLDIR;
        full_path = world_dir.append(::Level::GetName());
        full_path = removeExtension(full_path);
        full_path = full_path.append(L"\\"); // < path into level folder
        full_path = full_path + utf8_decode(ini_file);
    }

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


    short* hitbox_width = &GM_HITBOX_W_PTR;
    short* hitbox_height = &GM_HITBOX_H_PTR;
    short* hitbox_height_duck = &GM_HITBOX_H_D_PTR;
    short* hitbox_grab_offset_X = &GM_HITBOX_GRABOFF_X;
    short* hitbox_grab_offset_Y = &GM_HITBOX_GRABOFF_Y;


	//Parser of hitbox properties from PGE Calibrator INI File

	//Frames X and Y on playable character sprite from 0 to 9

	// SMBX FrameID <-> X-Y Conversion formulas:
	// TO:
	//FrameID = (Y+10*X)-49
	// FROM:
	//X = ((FrameID+49)-(FrameID+49)%10)/10
	//Y = (FrameID+49)%10


	std::string width = "";
	std::string height = "";
	std::string height_duck = "";
	std::string grab_offset_x = "";
	std::string grab_offset_y = "";
	std::string isUsed;
	std::string offsetX;
	std::string offsetY;

	switch(character)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		//normal
		width = hitBoxFile.Get("common", "width", "");
		height = hitBoxFile.Get("common", "height", "");
		//duck
		height_duck = hitBoxFile.Get("common", "height-duck", "");

		//grab offsets
		grab_offset_x = hitBoxFile.Get("common", "grab-offset-x", "");
		grab_offset_y = hitBoxFile.Get("common", "grab-offset-y", "");

		for (int x = 0; x<10; x++)
		{
			for (int y = 0; y<10; y++)
			{
				isUsed.clear();
				offsetX.clear();
				offsetY.clear();
				std::stringstream xx;
				xx << "frame-" << x << "-" << y;
				std::string tFrame = xx.str();
				isUsed = hitBoxFile.Get(tFrame, "used", "false");
				if (isUsed == "true") //--> skip this frame
				{
					//Offset relative to
					offsetX = hitBoxFile.Get(tFrame, "offsetX", "default value");
					offsetY = hitBoxFile.Get(tFrame, "offsetY", "default value");
					if (!offsetX.empty() && !offsetY.empty())
					{
						SMBX_CustomGraphics::setOffsetX((Characters)_character,
							SMBX_CustomGraphics::convIndexCoorToSpriteIndex(x, y),
							(PowerupID)_powerup, -(int)atoi(offsetX.c_str()));
						SMBX_CustomGraphics::setOffsetY((Characters)_character,
							SMBX_CustomGraphics::convIndexCoorToSpriteIndex(x, y),
							(PowerupID)_powerup, -(int)atoi(offsetY.c_str()));
					}
				}
			}
		}
		break;
	default:
		MessageBoxA(0, "Wrong character ID", "Error", 0);
		return;
	}

	if( !width.empty() )
		hitbox_width[powerup*5+character] = (short)atoi(width.c_str());
	if( !height.empty() )
		hitbox_height[powerup*5+character] = (short)atoi(height.c_str());
	if( !height_duck.empty() )
		hitbox_height_duck[powerup*5+character] = (short)atoi(height_duck.c_str());
	if( !grab_offset_x.empty() )
		hitbox_grab_offset_X[powerup*5+character] = (short)atoi(grab_offset_x.c_str());
	if( !grab_offset_y.empty() )
		hitbox_grab_offset_Y[powerup*5+character] = (short)atoi(grab_offset_y.c_str());
}


int LuaProxy::totalNPCs()
{
	return (int)GM_NPCS_COUNT;
}


luabind::object LuaProxy::npcs(lua_State *L)
{
	luabind::object vnpcs = luabind::newtable(L);
	for(int i = 0; i < GM_NPCS_COUNT; i++) {
		vnpcs[i] = LuaProxy::NPC(i);
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
		if(thisnpc->id == ID || anyID) {
			if(::NPC::GetSection(thisnpc) == section || anySec) {
				vnpcs[r] = LuaProxy::NPC(i);
				++r;
			}
		}
	}
	return vnpcs;
}

void LuaProxy::mem(int mem, LuaProxy::L_FIELDTYPE ftype, const luabind::object &value, lua_State *L)
{
	int iftype = (int)ftype;
	if(iftype >= 1 && iftype <= 5){
		void* ptr = ((&(*(byte*)mem)));
		MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
	}
	else if (ftype == LFT_STRING) {
		void* ptr = ((&(*(byte*)mem)));
		LuaHelper::assignVB6StrPtr((VB6StrPtr*)ptr, value, L);
	}
}


luabind::object LuaProxy::mem(int mem, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
	int iftype = (int)ftype;
	double val = 0;
	if(iftype >= 1 && iftype <= 6){
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
	case LFT_STRING:
		return luabind::object(L, VBStr((wchar_t*)(int)val));
	default:
		return luabind::object();
	}
}


void LuaProxy::triggerEvent(const std::string& evName)
{
	SMBXEvents::TriggerEvent(utf8_decode(evName), 0);
}


void LuaProxy::playSFX(int index)
{
	SMBXSound::PlaySFX(index);
}


void LuaProxy::playSFX(const std::string& filename)
{
#ifndef NO_SDL
	playSFXSDL(filename);
#else
	wstring full_path;
	if(!isAbsolutePath(filename)){
		wstring world_dir = (wstring)GM_FULLDIR;
		full_path = world_dir.append(Level::GetName());
		full_path = removeExtension(full_path);
		full_path = full_path.append(L"\\"); // < path into level folder
		full_path = full_path + utf8_decode(filename);
	}else{
		full_path = utf8_decode(filename);
	}
	
	PlaySound(full_path.c_str(), 0, SND_FILENAME | SND_ASYNC);
#endif
}

void LuaProxy::playSFXSDL(const std::string& filename)
{
#ifndef NO_SDL
    std::string full_paths = Audio::getSfxPath(filename);
	PGE_Sounds::SND_PlaySnd(full_paths.c_str());
#else
	playSFX(filename);
#endif
}

void LuaProxy::clearSFXBuffer()
{
#ifndef NO_SDL
	PGE_Sounds::clearSoundBuffer();
#endif
}

void LuaProxy::MusicOpen(const std::string& filename)
{
#ifndef NO_SDL
    std::string full_paths = Audio::getSfxPath(filename);
	PGE_MusPlayer::MUS_openFile(full_paths.c_str());
#endif
}

void LuaProxy::MusicPlay()
{
#ifndef NO_SDL
	PGE_MusPlayer::MUS_playMusic();
#endif
}

void LuaProxy::MusicPlayFadeIn(int ms)
{
#ifndef NO_SDL
	PGE_MusPlayer::MUS_playMusicFadeIn(ms);
#endif
}

void LuaProxy::MusicStop()
{
#ifndef NO_SDL
    PGE_MusPlayer::MUS_stopMusic();
#endif
}

void LuaProxy::MusicStopFadeOut(int ms)
{
#ifndef NO_SDL
	PGE_MusPlayer::MUS_stopMusicFadeOut(ms);
#endif
}

void LuaProxy::MusicVolume(int vlm)
{
#ifndef NO_SDL
	PGE_MusPlayer::MUS_changeVolume(vlm);
#endif
}

bool LuaProxy::MusicIsPlaying()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsPlaying();
#else
    return false;
#endif
}

bool LuaProxy::MusicIsPaused()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsPaused();
#else
    return false;
#endif
}

bool LuaProxy::MusicIsFading()
{
#ifndef NO_SDL
    return PGE_MusPlayer::MUS_IsFading();
#else
    return false;
#endif
}


void LuaProxy::playMusic(int section)
{
	SMBXSound::PlayMusic(section);
}

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

void LuaProxy::runAnimation(int id, double x, double y, double height, double width, double speedX, double speedY, int extraData)
{
	typedef int __stdcall animationFunc(int, int, int, int, int);
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


luabind::object LuaProxy::blocks(lua_State *L)
{
	luabind::object vblocks = luabind::newtable(L);
	for(int i = 0; i < GM_BLOCK_COUNT; i++) {
		vblocks[i] = LuaProxy::Block(i);
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

luabind::object LuaProxy::findlayer(const std::string& layername, lua_State *L)
{
    std::wstring tarLayerName = utf8_decode(layername);
	for(int i = 0; i < 100; ++i){
		LayerControl* ctrl = ::Layer::Get(i);
		if(ctrl){
			if(!ctrl->ptLayerName)
				continue;
			std::wstring sourceLayerName(ctrl->ptLayerName);
			if(tarLayerName == sourceLayerName){
				return luabind::object(L, Layer(i));
			}
		}
	}
	return luabind::object();
}

luabind::object LuaProxy::animations(lua_State *L)
{
	luabind::object vanimations = luabind::newtable(L);
	for(int i = 0; i < GM_ANIM_COUNT; i++) {
		vanimations[i] = LuaProxy::Animation(i);
	}
	return vanimations;
}


void LuaProxy::runAnimation(int id, double x, double y, double height, double width, int extraData)
{
	typedef int __stdcall animationFunc(int, int, int, int, int);
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
	typedef int __stdcall animationFunc(int, int, int, int, int);
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


luabind::object LuaProxy::levels(lua_State *L)
{
	luabind::object vlevels = luabind::newtable(L);
	for(int i = 0; i < (signed)GM_LEVEL_COUNT; i++) {
		vlevels[i+1] = LuaProxy::LevelObject(i);
	}
	return vlevels;
}

luabind::object LuaProxy::findlevels(const std::string &toFindName, lua_State* L)
{
	luabind::object obj = luabind::newtable(L);
	bool found = false;
	for(int i = 0, j = 0; i < (signed)GM_LEVEL_COUNT; ++i){
		WorldLevel* ctrl = ::SMBXLevel::get(i);
		if(ctrl){
			std::wstring tarLevelName = utf8_decode(std::string(toFindName));
			if(!ctrl->levelTitle)
				continue;
			std::wstring sourceLayerName(ctrl->levelTitle);
			if(sourceLayerName.find(tarLevelName) != std::wstring::npos){
				if(!found)
					found = true;

				obj[j++] = LevelObject(i);
			}
		}
	}
	if(!found){
		return luabind::object();
	}
	return obj;
}

luabind::object LuaProxy::findlevel(const std::string &toFindName, lua_State* L)
{
	for(int i = 0; i < (signed)GM_LEVEL_COUNT; ++i){
		WorldLevel* ctrl = ::SMBXLevel::get(i);
		if(ctrl){
			std::wstring tarLevelName = utf8_decode(std::string(toFindName));
			if(!ctrl->levelTitle)
				continue;
			std::wstring sourceLevelName(ctrl->levelTitle);
			if(tarLevelName == sourceLevelName){
				return luabind::object(L, LevelObject(i));
			}
		}
	}
	return luabind::object();
}

RECT LuaProxy::newRECT()
{
	RECT r;
	r.bottom = 0;
	r.left = 0;
	r.right = 0;
	r.top = 0;
	return r;
}

LuaProxy::RECTd LuaProxy::newRECTd()
{
	RECTd r;
	r.bottom = 0.0;
	r.left = 0.0;
	r.right = 0.0;
	r.top = 0.0;
	return r;
}





LuaProxy::NPC LuaProxy::spawnNPC(short npcid, double x, double y, short section, lua_State* L)
{
	return LuaProxy::spawnNPC(npcid, x, y, section, false, false, L);
}

LuaProxy::NPC LuaProxy::spawnNPC(short npcid, double x, double y, short section, bool respawn, lua_State* L)
{
	return LuaProxy::spawnNPC(npcid, x, y, section, respawn, false, L);
}

LuaProxy::NPC LuaProxy::spawnNPC(short npcid, double x, double y, short section, bool respawn, bool centered, lua_State* L)
{

	if(npcid < 1 || npcid > ::NPC::MAX_ID){
		luaL_error(L, "Invalid NPC-ID!\nNeed NPC-ID between 1-%d\nGot NPC-ID: %d", ::NPC::MAX_ID, npcid);
		return LuaProxy::NPC(-1);
	}
		
	if(section < 0 || section > 20){
		luaL_error(L, "Invalid Section!\nNeed Section-Index between 0-20\nGot Section-Index: %d", section);
		return LuaProxy::NPC(-1);
	}
		
	if(GM_NPCS_COUNT >= 5000){
		luaL_error(L, "Over 5000 NPCs, cannot spawn more!");
		return LuaProxy::NPC(-1);
	}

	LuaProxy::NPC theNewNPC(GM_NPCS_COUNT);
	void* nativeAddr = theNewNPC.getNativeAddr();


	memset(nativeAddr, 0, 0x158);
	WORD* widthArray = GM_CONF_WIDTH;
	WORD* heightArray = GM_CONF_HEIGHT;
	WORD* gfxWidthArray = GM_CONF_GFXWIDTH;
	WORD* gfxHeightArray = GM_CONF_GFXHEIGHT;

	short width = widthArray[npcid];
	short height = heightArray[npcid];
	short gfxWidth = gfxWidthArray[npcid];
	short gfxHeight = gfxHeightArray[npcid];

	gfxWidth = (gfxWidth ? gfxWidth : width);
	gfxHeight = (gfxHeight ? gfxHeight : height);

	if (centered) {
		x -= 0.5 * (double)width;
		y -= 0.5 * (double)height;
	}

    NPCMOB* npc = (NPCMOB*)nativeAddr;
    npc->momentum.x = x;
    npc->momentum.y = y;
    npc->momentum.height = height;
    npc->momentum.width = width;
    npc->momentum.speedX = 0.0;
    npc->momentum.speedY = 0.0;

    npc->spawnMomentum.x = x;
    npc->spawnMomentum.y = y;
    npc->spawnMomentum.height = gfxHeight;
    npc->spawnMomentum.width = gfxWidth;
    npc->spawnMomentum.speedX = 0.0;
    npc->spawnMomentum.speedY = 0.0;

    if (respawn) {
        npc->spawnID = npcid;
    }
    npc->id = npcid;

    npc->offscreenCountdownTimer = 180;
    npc->unknown_124 = -1;
    npc->currentSection = section;

	++(GM_NPCS_COUNT);

	return theNewNPC;
}



LuaProxy::Animation LuaProxy::spawnEffect(short effectID, double x, double y, lua_State* L)
{
    return spawnEffect(effectID, x, y, 1.0f, L);
} 


LuaProxy::Animation LuaProxy::spawnEffect(short effectID, double x, double y, float animationFrame, lua_State* L)
{
    typedef void __stdcall animationFunc(short*, Momentum*, float*, short*, short*);
    animationFunc* spawnEffectFunc = (animationFunc*)GF_RUN_ANIM;

    if (effectID < 1 || effectID > ::SMBXAnimation::MAX_ID){
        luaL_error(L, "Invalid Effect-ID!\nNeed Effect-ID between 1-%d\nGot Effect-ID: %d", ::SMBXAnimation::MAX_ID, effectID);
        return LuaProxy::Animation(-1);
    }

    if (GM_ANIM_COUNT >= 996){
        luaL_error(L, "Over 996 Effects, cannot spawn more!");
        return LuaProxy::Animation(-1);
    }

    Momentum coor;                          //Arg 2
    coor.x = x;
    coor.y = y;            //Arg 3
    short npcID = 0;                        //Arg 4
    short onlyDrawMask = COMBOOL(false);    //Arg 5

    spawnEffectFunc(&effectID, &coor, &animationFrame, &npcID, &onlyDrawMask);
    return LuaProxy::Animation(GM_ANIM_COUNT - 1);
}

LuaProxy::VBStr LuaProxy::getInput()
{
    return VBStr(*(wchar_t**)&GM_INPUTSTR_BUF_PTR);
}
