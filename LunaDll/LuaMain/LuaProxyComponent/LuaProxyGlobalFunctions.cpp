#include "../LuaProxy.h"
#include "../../Rendering/Rendering.h"
#include <IniProcessor/ini_processing.h>
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
#include "../../Misc/VB6StrPtr.h"
#include "LuaProxyAudio.h"
#include <sstream>
#include "../../Misc/SafeFPUControl.h"

//type - Player's state/powerup
//ini_file - path to INI-file which contains the hitbox redefinations
void LuaProxy::loadHitboxes(int _character, int _powerup, const std::string& ini_file, lua_State* L)
{
    if((_powerup < 1) || (_powerup>10))
    {
        luaL_error(L, "Powerup ID must be from 1 to 10.");
        return;
    }
    if ((_character < 1) || (_character > 5))
    {
        if (runtimeHookGetExtCharacterHitBoxData(_character, _powerup) == nullptr)
        {
            luaL_error(L, "Invalid character ID.");
            return;
        }
    }

    std::string full_path = resolveIfNotAbsolutePath(ini_file);

    IniProcessing hitBoxFile(full_path);
    if(!hitBoxFile.isOpened())
    {
        luaL_error(L, ("Failed to read an INI file: " + full_path).c_str());
        return;
    }

    //Parser of hitbox properties from PGE Calibrator INI File

    //Frames X and Y on playable character sprite from 0 to 9

    // SMBX FrameID <-> X-Y Conversion formulas:
    // TO:
    //FrameID = (Y+10*X)-49
    // FROM:
    //X = ((FrameID+49)-(FrameID+49)%10)/10
    //Y = (FrameID+49)%10

    const short UNDEFINED = 0x7FFF;
    short width = UNDEFINED;
    short height = UNDEFINED;
    short height_duck = UNDEFINED;
    short grab_offset_x = UNDEFINED;
    short grab_offset_y = UNDEFINED;
    bool isUsed = false;
    short offsetX = UNDEFINED;
    short offsetY = UNDEFINED;

    hitBoxFile.beginGroup("common");
    //normal
    hitBoxFile.read("width", width, UNDEFINED);
    hitBoxFile.read("height", height, UNDEFINED);
    //duck
    hitBoxFile.read("height-duck", height_duck, UNDEFINED);

    //grab offsets
    hitBoxFile.read("grab-offset-x", grab_offset_x, UNDEFINED);
    hitBoxFile.read("grab-offset-y", grab_offset_y, UNDEFINED);

    hitBoxFile.endGroup();

    for (int x = 0; x < 10; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            isUsed = false;
            offsetX = UNDEFINED;
            offsetY = UNDEFINED;

            std::stringstream xx;
            xx << "frame-" << x << "-" << y;

            std::string tFrame = xx.str();
            hitBoxFile.beginGroup(tFrame);

            hitBoxFile.read("used", isUsed, false);
            if(isUsed) //--> skip this frame
            {
                //Offset relative to
                hitBoxFile.read("offsetX", offsetX, UNDEFINED);
                hitBoxFile.read("offsetY", offsetY, UNDEFINED);
                if(offsetX != UNDEFINED && offsetY != UNDEFINED)
                {
                    SMBX_CustomGraphics::setOffsetX((Characters)_character,
                        SMBX_CustomGraphics::convIndexCoorToSpriteIndex(x, y),
                        (PowerupID)_powerup, -offsetX);
                    SMBX_CustomGraphics::setOffsetY((Characters)_character,
                        SMBX_CustomGraphics::convIndexCoorToSpriteIndex(x, y),
                        (PowerupID)_powerup, -offsetY);
                }
            }
            hitBoxFile.endGroup();
        }
    }

    if(width != UNDEFINED)
        SMBX_CustomGraphics::setPlayerHitboxWidth((PowerupID)_powerup, (Characters)_character, width);
    if(height != UNDEFINED)
        SMBX_CustomGraphics::setPlayerHitboxHeight((PowerupID)_powerup, (Characters)_character, height);
    if(height_duck != UNDEFINED)
        SMBX_CustomGraphics::setPlayerHitboxDuckHeight((PowerupID)_powerup, (Characters)_character, height_duck);
    if(grab_offset_x != UNDEFINED)
        SMBX_CustomGraphics::setPlayerGrabOffsetX((PowerupID)_powerup, (Characters)_character, grab_offset_x);
    if(grab_offset_y != UNDEFINED)
        SMBX_CustomGraphics::setPlayerGrabOffsetY((PowerupID)_powerup, (Characters)_character, grab_offset_y);
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
    void* ptr = ((&(*(byte*)mem)));

    switch (ftype) {
    case LFT_BYTE:
    case LFT_WORD:
    case LFT_DWORD:
    case LFT_FLOAT:
    case LFT_DFLOAT:
    {
        try {
            MemAssign((int)ptr, luabind::object_cast<double>(value), OP_Assign, (FIELDTYPE)ftype);
        }
        catch (luabind::cast_failed) {
            luaL_error(L, "Cannot interpret field as number");
        }
        break;
    }
    case LFT_STRING:
    {
        LuaHelper::assignVB6StrPtr((VB6StrPtr*)ptr, value, L);
        break;
    }
    case LFT_BOOL:
    {
        bool obj;
        try {
            obj = luabind::object_cast<bool>(value);
        } catch (luabind::cast_failed) {
            luaL_error(L, "Cannot interpret field as boolean");
            break;
        }
        void* ptr = ((&(*(byte*)mem)));
        *((short*)ptr) = COMBOOL(obj);
        break;
    }
    default:
        break;
    }
}


luabind::object LuaProxy::mem(int mem, LuaProxy::L_FIELDTYPE ftype, lua_State *L)
{
    int iftype = (int)ftype;
    void* ptr = ((&(*(byte*)mem)));

    switch (ftype) {
    case LFT_BYTE:
    case LFT_WORD:
    case LFT_DWORD:
    case LFT_FLOAT:
    case LFT_DFLOAT:
    {
        double val = GetMem((int)ptr, (FIELDTYPE)ftype);
        return luabind::object(L, val);
    }
    case LFT_STRING:
    {
        return luabind::object(L, (std::string)*((VB6StrPtr*)ptr));
    }
    case LFT_BOOL:
    {
        return luabind::object(L, 0 != *((int16_t*)ptr));
    }
    default:
        return luabind::object();
    }
}


void LuaProxy::triggerEvent(const std::string& evName)
{
    SMBXEvents::TriggerEvent(Str2WStr(evName), 0);
}


void LuaProxy::playSFX(int index)
{
    SMBXSound::PlaySFX(index);
}


void LuaProxy::playSFX(const std::string& filename, lua_State* L)
{
#ifndef NO_SDL
    playSFXSDL(filename, L);
#else
    wstring full_path;
    if(!isAbsolutePath(filename)){
        full_path = getCustomFolderPath() + utf8_decode(filename);
    }else{
        full_path = utf8_decode(filename);
    }

    PlaySound(full_path.c_str(), 0, SND_FILENAME | SND_ASYNC);
#endif
}

void LuaProxy::playSFXSDL(const std::string& filename, lua_State* L)
{
#ifndef NO_SDL
    std::string full_paths = Audio::getSfxPath(filename);
    if (!PGE_Sounds::SND_PlaySnd(full_paths.c_str()))
    {
        // If error, pass to Lua
        luaL_error(L, "%s", PGE_Sounds::SND_getLastError());
    }
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
    SMBXSound::PlayMusic(section, false);
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
    std::wstring tarLayerName = Str2WStr(layername);
    for(int i = 0; i < 255; ++i){
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

void LuaProxy::runAnimation(int id, double x, double y, double height, double width, double speedX, double speedY, int extraData)
{
    typedef int __stdcall animationFunc(int, int, int, int, int);
    animationFunc* f = (animationFunc*)GF_RUN_ANIM;

    Momentum tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.height = height;
    tmp.width = width;
    tmp.speedX = speedX;
    tmp.speedY = speedY;
    int a4 = 0;
    int a5 = 0;
    SafeFPUControl::clear();
    f((int)&id, (int)&tmp, (int)&extraData, (int)&a4, (int)&a5);
}

void LuaProxy::runAnimation(int id, double x, double y, double height, double width, int extraData)
{
    runAnimation(id, x, y, height, width, 0.0, 0.0, extraData);
}

void LuaProxy::runAnimation(int id, double x, double y, int extraData)
{
    runAnimation(id, x, y, 0.0, 0.0, extraData);
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
            std::wstring tarLevelName = Str2WStr(std::string(toFindName));
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
            std::wstring tarLevelName = Str2WStr(std::string(toFindName));
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
    npc->spawnMomentum.height = height;
    npc->spawnMomentum.width = width;
    npc->spawnMomentum.speedX = 0.0;
    npc->spawnMomentum.speedY = 0.0;

    if (respawn) {
        npc->spawnID = npcid;
    }
    npc->id = npcid;

    npc->offscreenCountdownTimer = 180;
    npc->activeFlag = -1;
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

    SafeFPUControl::clear();
    spawnEffectFunc(&effectID, &coor, &animationFrame, &npcID, &onlyDrawMask);
    return LuaProxy::Animation(GM_ANIM_COUNT - 1);
}

std::string LuaProxy::getInput()
{
    return (*(VB6StrPtr*)&GM_INPUTSTR_BUF_PTR);
}
