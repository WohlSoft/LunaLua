#include "../LuaProxy.h"
#include "../../Rendering/Rendering.h"
#include "../../libs/ini-reader/INIReader.h"
#include "../../MOBs/PlayerMOB.h"
#include "../../MOBs/NPCs.h"
#include "../../Blocks/Blocks.h"
#include "../../Layer/Layer.h"
#include "../../Sound/Sound.h"
#include "../../SMBX_Events/SMBXEvents.h"
#include "../../World/Overworld.h"
#include "../../World/WorldLevel.h"
#include "../../Level/Level.h"
#include "../../GlobalFuncs.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SdlMusic/SdlMusPlayer.h"
#include "../../Misc/RuntimeHook.h"


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
	short *hitbox_grab_offset_X = (short *)(0xB2C7CE);
	short *hitbox_grab_offset_Y = (short *)(0xB2C814);


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
	std::string width = "";
	std::string height = "";
	std::string height_duck = "";
	std::string grab_offset_x = "";
	std::string grab_offset_y = "";

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
		if(thisnpc->Identity == ID || anyID) {
			if(::NPC::GetSection(thisnpc) == section || anySec) {
				vnpcs[r] = LuaProxy::NPC(i);
				++r;
			}
		}
	}
	return vnpcs;
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
#ifndef NO_SDL
	playSFXSDL(filename);
#else
	wstring world_dir = wstring((wchar_t*)GM_FULLDIR);
	wstring full_path = world_dir.append(Level::GetName());
	full_path = removeExtension(full_path);
	full_path = full_path.append(L"\\"); // < path into level folder
	full_path = full_path + utf8_decode(filename);
	PlaySound(full_path.c_str(), 0, SND_FILENAME | SND_ASYNC);
#endif
}

void LuaProxy::playSFXSDL(const char* filename)
{
#ifndef NO_SDL
	wstring world_dir = wstring((wchar_t*)GM_FULLDIR);
	wstring full_path = world_dir.append(Level::GetName());
	full_path = removeExtension(full_path);
	full_path = full_path.append(L"\\"); // < path into level folder
	string full_paths = wstr2str(full_path) + filename;
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

void LuaProxy::MusicOpen(const char *filename)
{
#ifndef NO_SDL
	wstring world_dir = wstring((wchar_t*)GM_FULLDIR);
	wstring full_path = world_dir.append(Level::GetName());
	full_path = removeExtension(full_path);
	full_path = full_path.append(L"\\"); // < path into level folder
	string full_paths = wstr2str(full_path) + filename;
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
	PGE_MusPlayer::MUS_playMusic();
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
				return luabind::object(L, Layer(i));
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

std::string LuaProxy::getSMBXPath()
{
	return utf8_encode(wstring(getModulePath()));
}

void LuaProxy::hud(bool activate)
{
	gSkipSMBXHUD = !activate;
}

luabind::object LuaProxy::levels(lua_State *L)
{
	luabind::object vlevels = luabind::newtable(L);
	for(int i = 0; i < (signed)GM_LEVEL_COUNT; i++) {
		vlevels[i+1] = LuaProxy::LevelObject(i);
	}
	return vlevels;
}

luabind::object LuaProxy::findlevels(std::string toFindName, lua_State* L)
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

luabind::object LuaProxy::findlevel(std::string toFindName, lua_State* L)
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


std::string LuaProxy::getLevelFilename()
{
	return utf8_encode(std::wstring((wchar_t*)GM_LVLFILENAME_PTR));
}

std::string LuaProxy::getLevelName()
{
	return utf8_encode(std::wstring((wchar_t*)GM_LVLNAME_PTR));
}



LuaProxy::NPC LuaProxy::spawnNPC(short npcid, double x, double y, short section, lua_State* L)
{

	if(npcid < 1 || npcid > 292){
		luaL_error(L, "Invalid NPC-ID!\nNeed NPC-ID between 1-292\nGot NPC-ID: %d", npcid);
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
	WORD* widthArray = (WORD*)GM_CONF_WIDTH;
	WORD* heightArray = (WORD*)GM_CONF_HEIGHT;
	WORD* gfxWidthArray = (WORD*)GM_CONF_GFXWIDTH;
	WORD* gfxHeightArray = (WORD*)GM_CONF_GFXHEIGHT;

	short width = widthArray[npcid];
	short height = heightArray[npcid];
	short gfxWidth = gfxWidthArray[npcid];
	short gfxHeight = gfxHeightArray[npcid];

	gfxWidth = (gfxWidth ? gfxWidth : width);
	gfxHeight = (gfxHeight ? gfxHeight : height);

	PATCH_OFFSET(nativeAddr, 0x78, double, x);
	PATCH_OFFSET(nativeAddr, 0x80, double, y);
	PATCH_OFFSET(nativeAddr, 0x88, double, height);
	PATCH_OFFSET(nativeAddr, 0x90, double, width);
	PATCH_OFFSET(nativeAddr, 0x98, double, 0.0);
	PATCH_OFFSET(nativeAddr, 0xA0, double, 0.0);

	PATCH_OFFSET(nativeAddr, 0xA8, double, x);
	PATCH_OFFSET(nativeAddr, 0xB0, double, y);
	PATCH_OFFSET(nativeAddr, 0xB8, double, gfxHeight);
	PATCH_OFFSET(nativeAddr, 0xC0, double, gfxWidth);

	PATCH_OFFSET(nativeAddr, 0xDC, WORD, npcid);
	PATCH_OFFSET(nativeAddr, 0xE2, WORD, npcid);

	PATCH_OFFSET(nativeAddr, 0x12A, WORD, 180);
	PATCH_OFFSET(nativeAddr, 0x124, WORD, -1);
	PATCH_OFFSET(nativeAddr, 0x146, WORD, section);

	++(GM_NPCS_COUNT);

	return theNewNPC;
}
