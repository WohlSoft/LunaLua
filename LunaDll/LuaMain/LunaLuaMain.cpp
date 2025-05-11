#include <string>
#include <memory>
#include <luabind/adopt_policy.hpp>
#include <luabind/out_value_policy.hpp>

#include "LunaLuaMain.h"
#include "../version.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../SMBXInternal/Level.h"
#include "../Misc/MiscFuncs.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/NPCs.h"
#include "../SMBXInternal/BGOs.h"
#include "../SMBXInternal/Animation.h"

#include "../Rendering/FrameCapture.h"

#include "LuaHelper.h"
#include "LuaProxy.h"
#include "LuaProxyComponent/LuaProxyAudio.h"
#include "../libs/luasocket/luasocket.h"
#include "../libs/luasocket/mime.h"
#include "../SdlMusic/MusicManager.h"
#include "../Rendering/LunaImage.h"
#include "../Rendering/ImageLoader.h"
#include "../Misc/RuntimeHook.h"
#include "../Input/MouseHandler.h"

#include "../Misc/LoadScreen.h"

#include "LunaPathValidator.h"
#include "../Misc/CollisionMatrix.h"
#include "../SMBXInternal/Ports.h"

/*static*/ DWORD CLunaFFILock::currentLockTlsIdx = TlsAlloc();

extern bool luaDidGameOverFlag;

const std::wstring CLunaLua::LuaLibsPath = L"\\scripts\\base\\engine\\main.lua";
using namespace luabind;

std::wstring CLunaLua::getLuaLibsPath()
{
    std::wstring lapi = gAppPathWCHAR;
    lapi = lapi.append(L"\\scripts\\base\\engine\\main.lua");
    return lapi;
}


CLunaLua::CLunaLua() :
        m_type(CLunaLua::LUNALUA_LEVEL),
        m_luaEventTableName(""),
        m_warningList(),
        L(0),
        m_ready(false),
        m_onStartRan(false),
        m_disableSectionChangeEvent(false),
        m_executeSectionChangeFlag(false),
        m_playerSectionChangeList(),
        m_luaCallDepth(0)
{}

CLunaLua::~CLunaLua()
{
    //Just to be safe
    shutdown();
}

void CLunaLua::exitContext()
{
    if (isValid())
    {
        {
            const char* shutdownSpecificName = nullptr;
            if (m_type == LUNALUA_LEVEL)
            {
                shutdownSpecificName = "onExitLevel";
            }
            else if (m_type == LUNALUA_WORLD)
            {
                shutdownSpecificName = "onExitMap";
            }

            if (shutdownSpecificName != nullptr)
            {
                std::shared_ptr<Event> shutdownSpecificEvent = std::make_shared<Event>(shutdownSpecificName, false);
                shutdownSpecificEvent->setDirectEventName(shutdownSpecificName);
                shutdownSpecificEvent->setLoopable(false);
                if (m_type == LUNALUA_LEVEL)
                {
                    callEvent(shutdownSpecificEvent, GM_LEVEL_EXIT_TYPE);
                }
                else
                {
                    callEvent(shutdownSpecificEvent);
                }
            }

            std::shared_ptr<Event> shutdownEvent = std::make_shared<Event>("onExit", false);
            shutdownEvent->setDirectEventName("onExit");
            shutdownEvent->setLoopable(false);
            callEvent(shutdownEvent);
        }
        shutdown();

        g_PerfTracker.disable();

        // Clean & stop all user started sounds and musics
        PGE_MusPlayer::MUS_stopMusic();
        PGE_Sounds::clearSoundBuffer();
        // Reset music volume overrider
        MusicManager::setVolumeOverride(-1);
    }
}

bool CLunaLua::shutdown()
{
    //Shutdown the lua module if possible
    if(!isValid())
        return false;

    // Make sure the loadscreen is gone by the time we kill the main Lua instance
    LunaLoadScreenKill();

    // Unset flags of things Lua code was processing
    SetSMBXFrameTimingDefault();
    gRenderBGOFlag = true;
    gRenderSizableFlag = true;
    gRenderBackgroundFlag = true;
    gDisablePlayerFilterBounceFix = false;
    gDisableNPCRespawnBugFix = false;
    gLavaIsWeak = false;
    gCamerasInitialised = false;
    gDisablePlayerDownwardClipFix.Apply();
    gDisableNPCDownwardClipFix.Apply();
    gDisableNPCDownwardClipFixSlope.Apply();
    gNPCCeilingBugFix.Apply();
    gNPCSectionFix.Apply();
    gMovingFenceFixIsEnabled = true;
    gInvisibleFenceFix.Apply();
    gMovingVineFixIsEnabled = true;
    gDroppedItemFix.Apply();
    gLinkFairyClowncarFixes.Apply();
    gSlideJumpFixIsEnabled = true;
    SMBX13::Ports::_enablePowerupPowerdownPositionFixes = true;
    gCollisionMatrix.clear();


    // Request cached images/sounds/files be held onto for now
    LunaImage::holdCachedImages(m_type == LUNALUA_WORLD);
    PGE_Sounds::holdCached(m_type == LUNALUA_WORLD);
    CachedReadFile::holdCached(m_type == LUNALUA_WORLD);

    // Clear image override map
    ImageLoader::ClearOverrides();

    // Clear lua-based extra gfx
    ImageLoader::LuaUnregisterAllExtraGfx();

    // Don't be paused by Lua
    g_EventHandler.requestUnpause();

    checkWarnings();

    m_ready = false;
    m_onStartRan = false;
    m_luaCallDepth = 0;
    LuaProxy::Audio::resetMciSections();
    lua_close(L);
    L = NULL;

    CachedReadFile::clearData();

    return true;
}

void CLunaLua::init(LuaLunaType type, std::wstring codePath, std::wstring levelPath /*= std::wstring()*/)
{
    SafeFPUControl noFPUExecptions;

    //Just to be safe
    shutdown();
    m_luaCallDepth = 0;

    gLunaPathValidator.SetPaths();

    //Open up a new Lua State
    L = luaL_newstate();
    //Open all luabind functions
    luabind::open(L);
    //Set the new type
    m_type = type;

    //Open up "safe" standard lua libraries
    lua_pushcfunction(L, luaopen_base);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_math);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_string);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_table);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_debug);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_os);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_package);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_bit);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_ffi);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_jit);
    lua_call(L, 0, 0);

    //SOCKET TESTING STUFF
    lua_pushcfunction(L, luaopen_io);
    lua_call(L,0,0);

    lua_getfield(L, LUA_GLOBALSINDEX, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, luaopen_socket_core);
    lua_setfield(L, -2, "socket.core");

    lua_pushcfunction(L, luaopen_mime_core);
    lua_setfield(L, -2, "mime.core");

    //SOCKET TESTING STUFF

    //Remove unsafe apis
    {
        object _G = globals(L);
        object osTable = _G["os"];
        osTable["execute"] = object();
        osTable["exit"] = object();
        //osTable["getenv"] = object();
        //osTable["remove"] = object();
        //osTable["rename"] = object();
        osTable["setlocal"] = object();
        osTable["tmpname"] = object();
    }

    //Read the API-File
    std::wstring wLuaCode;
    if(!readFile(wLuaCode, getLuaLibsPath(), L"\"scripts\\base\\engine\\main.lua\" is required.\nBe sure you installed everything correctly!")){
        shutdown();
        return;
    }
    //Convert to ASCII, as lua doesn't support unicode
    std::string LuaCode = WStr2Str(wLuaCode);

    //Bind all functions, propeties ect...
    bindAll();
    bindAllDeprecated();

    // Store flags for stuff lua will read
    luaDidGameOverFlag = gDidGameOver;
    gDidGameOver = false;

    //Setup default contants
    setupDefaults();

    //Load the Lua API
    bool errLapi = false;
    {
        CCallDepthCounter callDepth(*this);
        int lapierrcode = luaL_loadbuffer(L, LuaCode.c_str(), LuaCode.length(), "=main.lua") || lua_pcall(L, 0, LUA_MULTRET, 0);
        if (!(lapierrcode == 0)) {
            object error_msg(from_stack(L, -1));
            LunaMsgBox::ShowA(0, object_cast<const char*>(error_msg), "Error", MB_ICONWARNING);
            errLapi = true;
        }
        {
            errLapi = errLapi || luabind::object_cast<bool>(luabind::globals(L)["__isLuaError"]);
        }
    }


    //Shutdown if an error happend.
    if(errLapi){
        shutdown();
        return;
    }

    //Call the lua api init funtion.
    const char* initName = "__onInit";
    if (LuaHelper::is_function(L, initName)) {
        callLuaFunction(L, initName, WStr2Str(codePath), WStr2Str(levelPath));
    }

    // Request cached images be held onto for now
    LunaImage::releaseCachedImages(m_type == LUNALUA_WORLD);
    PGE_Sounds::releaseCached(m_type == LUNALUA_WORLD);
    CachedReadFile::releaseCached(m_type == LUNALUA_WORLD);
}

//Setup default constants
void CLunaLua::setupDefaults()
{
    object _G = globals(L);
    LUAHELPER_DEF_CONST(_G, GAME_ENGINE);
    LUAHELPER_DEF_CONST(_G, LUNALUA_VERSION);
    _G["LUNALUA_VER"] = LUNALUA_VERSION; // ALIAS

    object verTable = newtable(L);
    verTable[1] = LUNA_VERNUM1;
    verTable[2] = LUNA_VERNUM2;
    verTable[3] = LUNA_VERNUM3;
    verTable[4] = LUNA_VERNUM4;
    _G["__LUNA_VERSION_TABLE"] = verTable;

    LUAHELPER_DEF_CONST(_G, PLAYER_SMALL);
    LUAHELPER_DEF_CONST(_G, PLAYER_BIG);
    LUAHELPER_DEF_CONST(_G, PLAYER_FIREFLOWER);
    LUAHELPER_DEF_CONST(_G, PLAYER_LEAF);
    LUAHELPER_DEF_CONST(_G, PLAYER_TANOOKIE);
    LUAHELPER_DEF_CONST(_G, PLAYER_HAMMER);
    LUAHELPER_DEF_CONST(_G, PLAYER_ICE);

    LUAHELPER_DEF_CONST(_G, CHARACTER_MARIO);
    LUAHELPER_DEF_CONST(_G, CHARACTER_LUIGI);
    LUAHELPER_DEF_CONST(_G, CHARACTER_PEACH);
    LUAHELPER_DEF_CONST(_G, CHARACTER_TOAD);
    LUAHELPER_DEF_CONST(_G, CHARACTER_LINK);

    _G["FIND_ANY"] = -1;

    _G["DIR_RIGHT"] = 1;
    _G["DIR_RANDOM"] = 0;
    _G["DIR_LEFT"] = -1;

    _G["FIELD_BYTE"] = LuaProxy::LFT_BYTE;
    _G["FIELD_WORD"] = LuaProxy::LFT_WORD;
    _G["FIELD_DWORD"] = LuaProxy::LFT_DWORD;
    _G["FIELD_FLOAT"] = LuaProxy::LFT_FLOAT;
    _G["FIELD_DFLOAT"] = LuaProxy::LFT_DFLOAT;
    _G["FIELD_STRING"] = LuaProxy::LFT_STRING;
    _G["FIELD_BOOL"] = LuaProxy::LFT_BOOL;

    _G["KEY_UP"] = GM_PLAYER_KEY_UP;
    _G["KEY_DOWN"] = GM_PLAYER_KEY_DOWN;
    _G["KEY_LEFT"] = GM_PLAYER_KEY_LEFT;
    _G["KEY_RIGHT"] = GM_PLAYER_KEY_RIGHT;
    _G["KEY_JUMP"] = GM_PLAYER_KEY_JUMP;
    _G["KEY_SPINJUMP"] = GM_PLAYER_KEY_SJUMP;
    _G["KEY_X"] = GM_PLAYER_KEY_X;
    _G["KEY_RUN"] = GM_PLAYER_KEY_RUN;
    _G["KEY_SEL"] = GM_PLAYER_KEY_SEL;
    _G["KEY_STR"] = GM_PLAYER_KEY_STR;

    _G["NPC_MAX_ID"] = NPC::MAX_ID;
    _G["BLOCK_MAX_ID"] = Block::MAX_ID;
    _G["BGO_MAX_ID"] = SMBX_BGO::MAX_ID;
    _G["ANIMATION_MAX_ID"] = SMBXAnimation::MAX_ID;

    LUAHELPER_DEF_CONST(_G, VK_LBUTTON);
    LUAHELPER_DEF_CONST(_G, VK_RBUTTON);
    LUAHELPER_DEF_CONST(_G, VK_CANCEL);
    LUAHELPER_DEF_CONST(_G, VK_MBUTTON);
    LUAHELPER_DEF_CONST(_G, VK_XBUTTON1);
    LUAHELPER_DEF_CONST(_G, VK_XBUTTON2);
    LUAHELPER_DEF_CONST(_G, VK_BACK);
    LUAHELPER_DEF_CONST(_G, VK_TAB);
    LUAHELPER_DEF_CONST(_G, VK_CLEAR);
    LUAHELPER_DEF_CONST(_G, VK_RETURN);
    LUAHELPER_DEF_CONST(_G, VK_SHIFT);
    LUAHELPER_DEF_CONST(_G, VK_CONTROL);
    LUAHELPER_DEF_CONST(_G, VK_MENU);
    LUAHELPER_DEF_CONST(_G, VK_PAUSE);
    LUAHELPER_DEF_CONST(_G, VK_CAPITAL);
    LUAHELPER_DEF_CONST(_G, VK_KANA);
    LUAHELPER_DEF_CONST(_G, VK_HANGEUL);
    LUAHELPER_DEF_CONST(_G, VK_HANGUL);
    LUAHELPER_DEF_CONST(_G, VK_JUNJA);
    LUAHELPER_DEF_CONST(_G, VK_FINAL);
    LUAHELPER_DEF_CONST(_G, VK_HANJA);
    LUAHELPER_DEF_CONST(_G, VK_KANJI);
    LUAHELPER_DEF_CONST(_G, VK_ESCAPE);
    LUAHELPER_DEF_CONST(_G, VK_CONVERT);
    LUAHELPER_DEF_CONST(_G, VK_NONCONVERT);
    LUAHELPER_DEF_CONST(_G, VK_ACCEPT);
    LUAHELPER_DEF_CONST(_G, VK_MODECHANGE);
    LUAHELPER_DEF_CONST(_G, VK_SPACE);
    LUAHELPER_DEF_CONST(_G, VK_PRIOR);
    LUAHELPER_DEF_CONST(_G, VK_NEXT);
    LUAHELPER_DEF_CONST(_G, VK_END);
    LUAHELPER_DEF_CONST(_G, VK_HOME);
    LUAHELPER_DEF_CONST(_G, VK_LEFT);
    LUAHELPER_DEF_CONST(_G, VK_UP);
    LUAHELPER_DEF_CONST(_G, VK_RIGHT);
    LUAHELPER_DEF_CONST(_G, VK_DOWN);
    LUAHELPER_DEF_CONST(_G, VK_SELECT);
    LUAHELPER_DEF_CONST(_G, VK_PRINT);
    LUAHELPER_DEF_CONST(_G, VK_EXECUTE);
    LUAHELPER_DEF_CONST(_G, VK_SNAPSHOT);
    LUAHELPER_DEF_CONST(_G, VK_INSERT);
    LUAHELPER_DEF_CONST(_G, VK_DELETE);
    LUAHELPER_DEF_CONST(_G, VK_HELP);

    // VK_# (# from 0 to 9)
    std::string vkConstantName = "VK_";
    for (char i = '0'; i <= '9'; i++) {
        std::string nextName = vkConstantName + i;
        _G[nextName] = i;
    }

    // VK_# (# from A to Z)
    for (char i = 'A'; i <= 'Z'; i++) {
        std::string nextName = vkConstantName + i;
        _G[nextName] = i;
    }

    LUAHELPER_DEF_CONST(_G, VK_LWIN);
    LUAHELPER_DEF_CONST(_G, VK_RWIN);
    LUAHELPER_DEF_CONST(_G, VK_APPS);
    LUAHELPER_DEF_CONST(_G, VK_SLEEP);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD0);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD1);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD2);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD3);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD4);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD5);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD6);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD7);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD8);
    LUAHELPER_DEF_CONST(_G, VK_NUMPAD9);
    LUAHELPER_DEF_CONST(_G, VK_MULTIPLY);
    LUAHELPER_DEF_CONST(_G, VK_ADD);
    LUAHELPER_DEF_CONST(_G, VK_SEPARATOR);
    LUAHELPER_DEF_CONST(_G, VK_SUBTRACT);
    LUAHELPER_DEF_CONST(_G, VK_DECIMAL);
    LUAHELPER_DEF_CONST(_G, VK_DIVIDE);
    LUAHELPER_DEF_CONST(_G, VK_F1);
    LUAHELPER_DEF_CONST(_G, VK_F2);
    LUAHELPER_DEF_CONST(_G, VK_F3);
    LUAHELPER_DEF_CONST(_G, VK_F4);
    LUAHELPER_DEF_CONST(_G, VK_F5);
    LUAHELPER_DEF_CONST(_G, VK_F6);
    LUAHELPER_DEF_CONST(_G, VK_F7);
    LUAHELPER_DEF_CONST(_G, VK_F8);
    LUAHELPER_DEF_CONST(_G, VK_F9);
    LUAHELPER_DEF_CONST(_G, VK_F10);
    LUAHELPER_DEF_CONST(_G, VK_F11);
    LUAHELPER_DEF_CONST(_G, VK_F12);
    LUAHELPER_DEF_CONST(_G, VK_F13);
    LUAHELPER_DEF_CONST(_G, VK_F14);
    LUAHELPER_DEF_CONST(_G, VK_F15);
    LUAHELPER_DEF_CONST(_G, VK_F16);
    LUAHELPER_DEF_CONST(_G, VK_F17);
    LUAHELPER_DEF_CONST(_G, VK_F18);
    LUAHELPER_DEF_CONST(_G, VK_F19);
    LUAHELPER_DEF_CONST(_G, VK_F20);
    LUAHELPER_DEF_CONST(_G, VK_F21);
    LUAHELPER_DEF_CONST(_G, VK_F22);
    LUAHELPER_DEF_CONST(_G, VK_F23);
    LUAHELPER_DEF_CONST(_G, VK_F24);
    LUAHELPER_DEF_CONST(_G, VK_NUMLOCK);
    LUAHELPER_DEF_CONST(_G, VK_SCROLL);
    LUAHELPER_DEF_CONST(_G, VK_OEM_NEC_EQUAL);
    LUAHELPER_DEF_CONST(_G, VK_OEM_FJ_JISHO);
    LUAHELPER_DEF_CONST(_G, VK_OEM_FJ_MASSHOU);
    LUAHELPER_DEF_CONST(_G, VK_OEM_FJ_TOUROKU);
    LUAHELPER_DEF_CONST(_G, VK_OEM_FJ_LOYA);
    LUAHELPER_DEF_CONST(_G, VK_OEM_FJ_ROYA);
    LUAHELPER_DEF_CONST(_G, VK_LSHIFT);
    LUAHELPER_DEF_CONST(_G, VK_RSHIFT);
    LUAHELPER_DEF_CONST(_G, VK_LCONTROL);
    LUAHELPER_DEF_CONST(_G, VK_RCONTROL);
    LUAHELPER_DEF_CONST(_G, VK_LMENU);
    LUAHELPER_DEF_CONST(_G, VK_RMENU);
    LUAHELPER_DEF_CONST(_G, VK_BROWSER_BACK);
    LUAHELPER_DEF_CONST(_G, VK_BROWSER_FORWARD);
    LUAHELPER_DEF_CONST(_G, VK_BROWSER_REFRESH);
    LUAHELPER_DEF_CONST(_G, VK_BROWSER_STOP);
    LUAHELPER_DEF_CONST(_G, VK_BROWSER_SEARCH);
    LUAHELPER_DEF_CONST(_G, VK_BROWSER_FAVORITES);
    LUAHELPER_DEF_CONST(_G, VK_BROWSER_HOME);
    LUAHELPER_DEF_CONST(_G, VK_VOLUME_MUTE);
    LUAHELPER_DEF_CONST(_G, VK_VOLUME_DOWN);
    LUAHELPER_DEF_CONST(_G, VK_VOLUME_UP);
    LUAHELPER_DEF_CONST(_G, VK_MEDIA_NEXT_TRACK);
    LUAHELPER_DEF_CONST(_G, VK_MEDIA_PREV_TRACK);
    LUAHELPER_DEF_CONST(_G, VK_MEDIA_STOP);
    LUAHELPER_DEF_CONST(_G, VK_MEDIA_PLAY_PAUSE);
    LUAHELPER_DEF_CONST(_G, VK_LAUNCH_MAIL);
    LUAHELPER_DEF_CONST(_G, VK_LAUNCH_MEDIA_SELECT);
    LUAHELPER_DEF_CONST(_G, VK_LAUNCH_APP1);
    LUAHELPER_DEF_CONST(_G, VK_LAUNCH_APP2);

    LUAHELPER_DEF_CONST(_G, WHUD_ALL);
    LUAHELPER_DEF_CONST(_G, WHUD_ONLY_OVERLAY);
    LUAHELPER_DEF_CONST(_G, WHUD_NONE);

    LUAHELPER_DEF_CONST(_G, EXITTYPE_ANY);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_BOSS);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_CARD_ROULETTE);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_CRYSTAL);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_NONE);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_OFFSCREEN);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_SECRET);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_STAR);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_TAPE);
    LUAHELPER_DEF_CONST(_G, EXITTYPE_WARP);

    LUAHELPER_DEF_CONST(_G, HARM_TYPE_JUMP);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_FROMBELOW);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_NPC);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_PROJECTILE_USED);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_LAVA);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_HELD);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_TAIL);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_SPINJUMP);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_OFFSCREEN);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_SWORD);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_EXT_FIRE);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_EXT_ICE);
    LUAHELPER_DEF_CONST(_G, HARM_TYPE_EXT_HAMMER);

    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_UNKNOWN);
    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_HIT);
    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_DESPAWN);
    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_CONTAINER);
    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_AI);
    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_EATEN);
    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_LINK);
    LUAHELPER_DEF_CONST(_G, NPC_TFCAUSE_SWITCH);

    LUAHELPER_DEF_CONST(_G, GL_FLOAT);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_VEC2);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_VEC3);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_VEC4);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT2);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT3);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT4);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT2x3);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT2x4);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT3x2);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT3x4);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT4x2);
    LUAHELPER_DEF_CONST(_G, GL_FLOAT_MAT4x3);
    LUAHELPER_DEF_CONST(_G, GL_INT);
    LUAHELPER_DEF_CONST(_G, GL_INT_VEC2);
    LUAHELPER_DEF_CONST(_G, GL_INT_VEC3);
    LUAHELPER_DEF_CONST(_G, GL_INT_VEC4);
    LUAHELPER_DEF_CONST(_G, GL_UNSIGNED_INT);
    LUAHELPER_DEF_CONST(_G, GL_UNSIGNED_INT_VEC2);
    LUAHELPER_DEF_CONST(_G, GL_UNSIGNED_INT_VEC3);
    LUAHELPER_DEF_CONST(_G, GL_UNSIGNED_INT_VEC4);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_VEC2);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_VEC3);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_VEC4);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT2);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT3);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT4);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT2x3);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT2x4);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT3x2);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT3x4);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT4x2);
    LUAHELPER_DEF_CONST(_G, GL_DOUBLE_MAT4x3);
    LUAHELPER_DEF_CONST(_G, GL_SAMPLER_2D);

    {
        using namespace LuaProxy::Graphics;
        LUAHELPER_DEF_CONST(_G, RTYPE_IMAGE);
        LUAHELPER_DEF_CONST(_G, RTYPE_TEXT);
    }

    _G["MOUSE_BUTTON_L"] = MouseHandler::BUTTON_L;
    _G["MOUSE_BUTTON_R"] = MouseHandler::BUTTON_R;
    _G["MOUSE_BUTTON_M"] = MouseHandler::BUTTON_M;
    _G["MOUSE_EVT_UP"]   = MouseHandler::EVT_UP;
    _G["MOUSE_EVT_DOWN"] = MouseHandler::EVT_DOWN;
    _G["MOUSE_EVT_DBL"]  = MouseHandler::EVT_DBL;
    _G["MOUSE_WHEEL_V"]  = MouseHandler::WHEEL_V;
    _G["MOUSE_WHEEL_H"]  = MouseHandler::WHEEL_H;

    _G["ODIR_UP"] = 1;
    _G["ODIR_LEFT"] = 2;
    _G["ODIR_DOWN"] = 3;
    _G["ODIR_RIGHT"] = 4;
    if(m_type == LUNALUA_WORLD){
        _G["isOverworld"] = true;
        _G["world"] = LuaProxy::World();
    }

    _G["inputConfig1"] = LuaProxy::InputConfig(1);
    _G["inputConfig2"] = LuaProxy::InputConfig(2);

    _G["console"] = LuaProxy::Console();

    _G["print"] = object();
}

LUAHELPER_DEF_CLASS_HELPER(LunaImage, LuaImageResource);
LUAHELPER_DEF_CLASS_HELPER(CaptureBuffer, CaptureBuffer);
LUAHELPER_DEF_CLASS_HELPER(Mix_Chunk, Mix_Chunk);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::InputConfig, NativeInputConfig);
LUAHELPER_DEF_CLASS_HELPER(RECT, RECT);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::RECTd, RECTd);
LUAHELPER_DEF_CLASS_HELPER(Event, Event);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Logger, Logger);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Shader, Shader);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Data, Data);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::AsyncHTTPRequest, AsyncHTTPRequest);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::PlayerSettings, PlayerSettings);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::World, World);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Tile, Tile);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Scenery, Scenery);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Path, Path);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Musicbox, Musicbox);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::LevelObject, Level);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Warp, Warp);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Animation, Animation);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Layer, Layer);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Section, Section);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::NPC, NPC);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Block, Block);
LUAHELPER_DEF_CLASS_HELPER(LuaProxy::Audio::PlayingSfxInstance, PlayingSfxInstance);


// LUAHELPER_DEF_CLASS(LuaImageResource)


void CLunaLua::bindAll()
{
    //Bind stuff for world and level
    module(L)
        [
            def("mem", (void(*)(int, LuaProxy::L_FIELDTYPE, const luabind::object &, lua_State*)) &LuaProxy::mem),
            def("mem", (luabind::object(*)(int, LuaProxy::L_FIELDTYPE, lua_State*)) &LuaProxy::mem),

            namespace_("Native")[
                def("getSMBXPath", &LuaProxy::Native::getSMBXPath),
                def("getEpisodePath", &LuaProxy::Native::getEpisodePath),
                def("simulateError", &LuaProxy::Native::simulateError)
            ],

            namespace_("Text")[
                def("windowDebug", &LuaProxy::Text::windowDebug),
                def("windowDebugSimple", &LuaProxy::Text::windowDebugSimple),
                def("print", (void(*)(const luabind::object&, int, int)) &LuaProxy::Text::print),
                def("print", (void(*)(const luabind::object&, int, int, int)) &LuaProxy::Text::print),
                def("printWP", (void(*)(const luabind::object&, int, int, double)) &LuaProxy::Text::printWP),
                def("printWP", (void(*)(const luabind::object&, int, int, int, double)) &LuaProxy::Text::printWP)
            ],

            namespace_("Graphics")[
                LUAHELPER_DEF_CLASS_SMART_PTR_SHARED(LuaImageResource, std::shared_ptr)
                    .def("__eq", &LuaProxy::luaUserdataCompare<LunaImage>)
                    .property("width", &LunaImage::getW)
                    .property("height", &LunaImage::getH)
                    .property("__rawDataPtr", &LunaImage::getDataPtrAsInt),
                LUAHELPER_DEF_CLASS_SMART_PTR_SHARED(CaptureBuffer, std::shared_ptr)
                    .def(constructor<int, int, bool>())
                    .def("__eq", &LuaProxy::luaUserdataCompare<CaptureBuffer>)
                    .def("captureAt", &CaptureBuffer::CaptureAt),
                def("loadImage", (bool(*)(const std::string&, int, int))&LuaProxy::Graphics::loadImage),
                def("loadImage", (std::shared_ptr<LunaImage>(*)(const std::string&, lua_State*))&LuaProxy::Graphics::loadImage),
                def("loadAnimatedImage", &LuaProxy::Graphics::loadAnimatedImage, pure_out_value<2>()),
                def("placeSprite", (void(*)(int, int, int, int, const std::string&, int))&LuaProxy::Graphics::placeSprite),
                def("placeSprite", (void(*)(int, int, int, int, const std::string&))&LuaProxy::Graphics::placeSprite),
                def("placeSprite", (void(*)(int, int, int, int))&LuaProxy::Graphics::placeSprite),
                def("placeSprite", (void(*)(int, const std::shared_ptr<LunaImage>&  img, int, int, const std::string&, int))&LuaProxy::Graphics::placeSprite),
                def("placeSprite", (void(*)(int, const std::shared_ptr<LunaImage>&  img, int, int, const std::string&))&LuaProxy::Graphics::placeSprite),
                def("placeSprite", (void(*)(int, const std::shared_ptr<LunaImage>&  img, int, int))&LuaProxy::Graphics::placeSprite),
                def("unplaceSprites", (void(*)(const std::shared_ptr<LunaImage>&  img))&LuaProxy::Graphics::unplaceSprites),
                def("unplaceSprites", (void(*)(const std::shared_ptr<LunaImage>&  img, int, int))&LuaProxy::Graphics::unplaceSprites),
                def("getPixelData", &LuaProxy::Graphics::getPixelData, meta::join<pure_out_value<2>, pure_out_value<3>>::type()),
                def("drawImage", (void(*)(const std::shared_ptr<LunaImage>& , double, double, lua_State*))&LuaProxy::Graphics::drawImage),
                def("drawImage", (void(*)(const std::shared_ptr<LunaImage>& , double, double, float, lua_State*))&LuaProxy::Graphics::drawImage),
                def("drawImage", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, lua_State*))&LuaProxy::Graphics::drawImage),
                def("drawImage", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, float, lua_State*))&LuaProxy::Graphics::drawImage),
                def("drawImageWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, lua_State*))&LuaProxy::Graphics::drawImageWP),
                def("drawImageWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, float, double, lua_State*))&LuaProxy::Graphics::drawImageWP),
                def("drawImageWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, double, lua_State*))&LuaProxy::Graphics::drawImageWP),
                def("drawImageWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, float, double, lua_State*))&LuaProxy::Graphics::drawImageWP),
                def("drawImageToScene", (void(*)(const std::shared_ptr<LunaImage>& , double, double, lua_State*))&LuaProxy::Graphics::drawImageToScene),
                def("drawImageToScene", (void(*)(const std::shared_ptr<LunaImage>& , double, double, float, lua_State*))&LuaProxy::Graphics::drawImageToScene),
                def("drawImageToScene", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, lua_State*))&LuaProxy::Graphics::drawImageToScene),
                def("drawImageToScene", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, float, lua_State*))&LuaProxy::Graphics::drawImageToScene),
                def("drawImageToSceneWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, lua_State*))&LuaProxy::Graphics::drawImageToSceneWP),
                def("drawImageToSceneWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, float, double, lua_State*))&LuaProxy::Graphics::drawImageToSceneWP),
                def("drawImageToSceneWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, double, lua_State*))&LuaProxy::Graphics::drawImageToSceneWP),
                def("drawImageToSceneWP", (void(*)(const std::shared_ptr<LunaImage>& , double, double, double, double, double, double, float, double, lua_State*))&LuaProxy::Graphics::drawImageToSceneWP),
                def("draw", &LuaProxy::Graphics::draw),
                def("isOpenGLEnabled", &LuaProxy::Graphics::isOpenGLEnabled),
                def("glSetTexture", &LuaProxy::Graphics::glSetTexture),
                def("glSetTextureRGBA", &LuaProxy::Graphics::glSetTextureRGBA),
                // glDrawTriangles will be defined at runtime using FFI
                def("__glInternalDraw", &LuaProxy::Graphics::__glInternalDraw),
                def("__setSpriteOverride", &LuaProxy::Graphics::__setSpriteOverride),
                def("__setHardcodedSpriteOverride", &LuaProxy::Graphics::__setHardcodedSpriteOverride),
                def("__getHardcodedSpriteOverride", &LuaProxy::Graphics::__getHardcodedSpriteOverride),
                def("__getSpriteOverride", &LuaProxy::Graphics::__getSpriteOverride)
            ],

            namespace_("__Effects_EXPERIMENTAL")[
                def("screenGlow", &LuaProxy::Effects::screenGlow),
                def("screenGlowNegative", &LuaProxy::Effects::screenGlowNegative),
                def("flipX", &LuaProxy::Effects::flipX),
                def("flipY", &LuaProxy::Effects::flipY),
                def("flipXY", &LuaProxy::Effects::flipXY)
            ],

            namespace_("Misc")[
                def("cheatBuffer", (std::string(*)())&LuaProxy::Misc::cheatBuffer),
                def("cheatBuffer", (void(*)(const luabind::object&, lua_State*))&LuaProxy::Misc::cheatBuffer),
                def("listFiles", &LuaProxy::Misc::listFiles),
                def("listDirectories", &LuaProxy::Misc::listDirectories),
                def("listLocalFiles", &LuaProxy::Misc::listLocalFiles),
                def("listLocalDirectories", &LuaProxy::Misc::listLocalDirectories),
                def("resolveFile", &LuaProxy::Misc::resolveFile),
                def("resolveDirectory", &LuaProxy::Misc::resolveDirectory),
                def("resolveGraphicsFile", &LuaProxy::Misc::resolveGraphicsFile),
                def("isSamePath", &LuaProxy::Misc::isSamePath),
                def("openPauseMenu", &LuaProxy::Misc::openPauseMenu),
                def("saveGame", &LuaProxy::Misc::saveGame),
                def("exitGame", &LuaProxy::Misc::exitGame),
                def("exitEngine", &LuaProxy::Misc::exitEngine),
                def("didGameOver", &LuaProxy::Misc::didGameOver),
                def("loadEpisode", &LuaProxy::Misc::loadEpisode),
                def("pause", (void(*)(void))&LuaProxy::Misc::pause),
                def("pause", (void(*)(bool))&LuaProxy::Misc::pause),
                def("unpause", &LuaProxy::Misc::unpause),
                def("isPausedByLua", &LuaProxy::Misc::isPausedByLua),
                def("warning", &LuaProxy::Misc::warning),
                def("registerCharacterId", &LuaProxy::Misc::registerCharacterId),
                // This used to be Level.loadPlayerHitBoxes, but it needs to be in a namespace that's usable from the overworld.
                def("loadCharacterHitBoxes", (void(*)(int, int, const std::string&))&LuaProxy::loadHitboxes),
                def("showRichDialog", &LuaProxy::Misc::showRichDialog),
                def("__enablePerfTracker", &LuaProxy::Misc::__enablePerfTracker),
                def("__disablePerfTracker", &LuaProxy::Misc::__disablePerfTracker),
                def("__getPerfTrackerData", &LuaProxy::Misc::__getPerfTrackerData),
                def("__getNPCPropertyTableAddress", &NPC::GetPropertyTableAddress),
                def("__getBlockPropertyTableAddress", &Blocks::GetPropertyTableAddress),
                def("getEditorPlacedItem",(std::string(*)())&GetEditorPlacedItem)
            ],

            namespace_("FileFormats")[
                LUARAW_DEF_CLASS(LevelData)
                    .enum_("Format")
                    [
                        value("PGEX",    LevelData::PGEX),
                        value("SMBX64",  LevelData::SMBX64),
                        value("SMBX38A", LevelData::SMBX38A)
                    ],
                LUARAW_DEF_CLASS(WorldData)
                    .enum_("Format")
                    [
                        value("PGEX",    WorldData::PGEX),
                        value("SMBX64",  WorldData::SMBX64),
                        value("SMBX38A", WorldData::SMBX38A)
                    ],
                LUARAW_DEF_CLASS(LevelSection)
                    .enum_("LightingValue")
                    [
                        value("LIGHTING_NOT_SET",       LevelSection::LIGHTING_NOT_SET),
                        value("LIGHTING_DISABLED",      LevelSection::LIGHTING_DISABLED),
                        value("LIGHTING_PIXELS_BEGIN",  LevelSection::LIGHTING_PIXELS_BEGIN)
                    ],
                LUARAW_DEF_CLASS(LevelBGO)
                    .enum_("zmodes")
                    [
                        value("Background2", LevelBGO::Background2),
                        value("Background1", LevelBGO::Background1),
                        value("ZDefault",    LevelBGO::ZDefault),
                        value("Foreground1", LevelBGO::Foreground1),
                        value("Foreground2", LevelBGO::Foreground2)
                    ],
                LUARAW_DEF_CLASS(LevelNPC)
                    .enum_("GeneratorDirection")
                    [
                        value("NPC_GEN_CENTER", LevelNPC::NPC_GEN_CENTER),
                        value("NPC_GEN_UP", LevelNPC::NPC_GEN_UP),
                        value("NPC_GEN_LEFT", LevelNPC::NPC_GEN_LEFT),
                        value("NPC_GEN_DOWN", LevelNPC::NPC_GEN_DOWN),
                        value("NPC_GEN_RIGHT", LevelNPC::NPC_GEN_RIGHT),
                        value("NPC_GEN_UP_LEFT", LevelNPC::NPC_GEN_UP_LEFT),
                        value("NPC_GEN_LEFT_DOWN", LevelNPC::NPC_GEN_LEFT_DOWN),
                        value("NPC_GEN_DOWN_RIGHT", LevelNPC::NPC_GEN_DOWN_RIGHT),
                        value("NPC_GEN_RIGHT_UP", LevelNPC::NPC_GEN_RIGHT_UP)
                    ]
                    .enum_("GeneratorTypes")
                    [
                        value("NPC_GENERATOR_APPEAR", LevelNPC::NPC_GENERATOR_APPEAR),
                        value("NPC_GENERATOR_WARP", LevelNPC::NPC_GENERATOR_WARP),
                        value("NPC_GENERATPR_PROJECTILE", LevelNPC::NPC_GENERATPR_PROJECTILE),
                        value("NPC_GENERATPR_CUSTOM1", LevelNPC::NPC_GENERATPR_CUSTOM1),
                        value("NPC_GENERATPR_CUSTOM2", LevelNPC::NPC_GENERATPR_CUSTOM2)
                    ],
                LUARAW_DEF_CLASS(LevelDoor)
                    .enum_("EntranceDirectopn")
                    [
                        value("ENTRANCE_UP",    LevelDoor::ENTRANCE_UP),
                        value("ENTRANCE_LEFT",  LevelDoor::ENTRANCE_LEFT),
                        value("ENTRANCE_DOWN",  LevelDoor::ENTRANCE_DOWN),
                        value("ENTRANCE_RIGHT", LevelDoor::ENTRANCE_RIGHT)
                    ]
                    .enum_("ExitDirectopn")
                    [
                        value("EXIT_UP",    LevelDoor::EXIT_UP),
                        value("EXIT_LEFT",  LevelDoor::EXIT_LEFT),
                        value("EXIT_DOWN",  LevelDoor::EXIT_DOWN),
                        value("EXIT_RIGHT", LevelDoor::EXIT_RIGHT)
                    ]
                    .enum_("WarpType")
                    [
                        value("WARP_INSTANT",   LevelDoor::WARP_INSTANT),
                        value("WARP_PIPE",      LevelDoor::WARP_PIPE),
                        value("WARP_DOOR",      LevelDoor::WARP_DOOR),
                        value("WARP_PORTAL",    LevelDoor::WARP_PORTAL)
                    ]
                    .enum_("WarpTransitEffect")
                    [
                        value("TRANSIT_NONE",           LevelDoor::TRANSIT_NONE),
                        value("TRANSIT_SCROLL",         LevelDoor::TRANSIT_SCROLL),
                        value("TRANSIT_FADE",           LevelDoor::TRANSIT_FADE),
                        value("TRANSIT_CIRCLE_FADE",    LevelDoor::TRANSIT_CIRCLE_FADE),
                        value("TRANSIT_FLIP_H",         LevelDoor::TRANSIT_FLIP_H),
                        value("TRANSIT_FLIP_V",         LevelDoor::TRANSIT_FLIP_V)
                    ],
                LUARAW_DEF_CLASS(LevelPhysEnv)
                    .enum_("EnvTypes")
                    [
                        value("ENV_WATER",           LevelPhysEnv::ENV_WATER),
                        value("ENV_QUICKSAND",       LevelPhysEnv::ENV_QUICKSAND),
                        value("ENV_CUSTOM_LIQUID",   LevelPhysEnv::ENV_CUSTOM_LIQUID),
                        value("ENV_GRAVITATIONAL_FIELD",   LevelPhysEnv::ENV_GRAVITATIONAL_FIELD),
                        value("ENV_TOUCH_EVENT_ONCE_PLAYER",   LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_PLAYER),
                        value("ENV_TOUCH_EVENT_PLAYER",   LevelPhysEnv::ENV_TOUCH_EVENT_PLAYER),
                        value("ENV_TOUCH_EVENT_ONCE_NPC",   LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC),
                        value("ENV_TOUCH_EVENT_NPC",   LevelPhysEnv::ENV_TOUCH_EVENT_NPC),
                        value("ENV_CLICK_EVENT",   LevelPhysEnv::ENV_CLICK_EVENT),
                        value("ENV_COLLISION_SCRIPT",   LevelPhysEnv::ENV_COLLISION_SCRIPT),
                        value("ENV_CLICK_SCRIPT",   LevelPhysEnv::ENV_CLICK_SCRIPT),
                        value("ENV_COLLISION_EVENT",   LevelPhysEnv::ENV_COLLISION_EVENT),
                        value("ENV_AIR",   LevelPhysEnv::ENV_AIR),
                        value("ENV_TOUCH_EVENT_ONCE_NPC1",   LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC1),
                        value("ENV_TOUCH_EVENT_NPC1",   LevelPhysEnv::ENV_TOUCH_EVENT_NPC1),
                        value("ENV_NPC_HURTING_FIELD",   LevelPhysEnv::ENV_NPC_HURTING_FIELD),
                        value("ENV_SUBAREA",   LevelPhysEnv::ENV_SUBAREA)
                    ],
                LUARAW_DEF_CLASS(LevelEvent_Sets)
                    .enum_("SetActions")
                    [
                        value("LESet_Nothing",   LevelEvent_Sets::LESet_Nothing),
                        value("LESet_ResetDefault",   LevelEvent_Sets::LESet_ResetDefault)
                    ],
                def("openLevel", &LuaProxy::Formats::openLevel),
                def("openLevelHeader", &LuaProxy::Formats::openLevelHeader),
                def("getLevelData", &LuaProxy::Formats::getLevelData),
                def("openWorld", &LuaProxy::Formats::openWorld),
                def("openWorldHeader", &LuaProxy::Formats::openWorldHeader),
                def("openNpcConfig", &LuaProxy::Formats::openNpcConfig)
            ],
            /*************************FileFormats*end*************************/

            namespace_("Audio")[
                //SDL_Mixer's Mix_Chunk structure
                LUAHELPER_DEF_CLASS(Mix_Chunk)
                    .property("allocated", &Mix_Chunk::allocated)
                    .property("abuf", &Mix_Chunk::abuf)
                    .def_readwrite("alen", &Mix_Chunk::alen)
                    .def_readwrite("volume", &Mix_Chunk::volume),

                //Music
                def("MusicOpen", (void(*)(const std::string&))&LuaProxy::Audio::MusicOpen),
                def("MusicPlay", (void(*)())&LuaProxy::Audio::MusicPlay),
                def("MusicPlayFadeIn", (void(*)(int))&LuaProxy::Audio::MusicPlayFadeIn),
                def("MusicStop", (void(*)())&LuaProxy::Audio::MusicStop),
                def("MusicStopFadeOut", (void(*)(int))&LuaProxy::Audio::MusicStopFadeOut),
                def("MusicPause", (void(*)())&LuaProxy::Audio::MusicPause),
                def("MusicResume", (void(*)())&LuaProxy::Audio::MusicResume),
                def("MusicIsPlaying", (bool(*)())&LuaProxy::Audio::MusicIsPlaying),
                def("MusicIsPaused", (bool(*)())&LuaProxy::Audio::MusicIsPaused),
                def("MusicIsFading", (bool(*)())&LuaProxy::Audio::MusicIsFading),
                def("_GetMusicVolume", (int(*)(void))&LuaProxy::Audio::GetMusicVolume),
                def("MusicVolume", (void(*)(int))&LuaProxy::Audio::MusicVolume),
                def("MusicTitle", (std::string(*)())&LuaProxy::Audio::MusicTitle),
                def("MusicTitleTag", (std::string(*)())&LuaProxy::Audio::MusicTitleTag),
                def("MusicArtistTag", (std::string(*)())&LuaProxy::Audio::MusicArtistTag),
                def("MusicAlbumTag", (std::string(*)())&LuaProxy::Audio::MusicAlbumTag),
                def("MusicCopyrightTag", (std::string(*)())&LuaProxy::Audio::MusicCopyrightTag),
                def("MusicSetPos", (void(*)(double))&LuaProxy::Audio::MusicSetPosition),
                def("MusicGetPos", (double(*)())&LuaProxy::Audio::MusicGetPosition),
                def("MusicGetLoopStart", (double(*)())&LuaProxy::Audio::MusicGetLoopStart),
                def("MusicGetLoopEnd", (double(*)())&LuaProxy::Audio::MusicGetLoopEnd),
                def("MusicGetLoopLength", (double(*)())&LuaProxy::Audio::MusicGetLoopLength),
                //Seize music stream for LUA usage for section 0..20
                def("SeizeStream", (void(*)(int))&LuaProxy::Audio::seizeStream),
                //Return music stream access to SMBX engine back for section 0..20
                def("ReleaseStream", (void(*)(int))&LuaProxy::Audio::releaseStream),
                //Release music stream for ALL sections
                def("resetMciSections", (void(*)())&LuaProxy::Audio::resetMciSections),
                def("MusicChange", (void(*)(int, int))&LuaProxy::Audio::changeMusic),
                def("MusicChange", (void(*)(int, const std::string&))&LuaProxy::Audio::changeMusic),
                def("MusicChange", (void(*)(int, int, int))&LuaProxy::Audio::changeMusic),
                def("MusicChange", (void(*)(int, const std::string&, int))&LuaProxy::Audio::changeMusic),
                def("MusicFadeOut", (void(*)(int, int))&LuaProxy::Audio::musicFadeOut),
                def("MusicRewind", (void(*)())&LuaProxy::Audio::MusicRewind),
                def("MusicGetInstChannelCount", (double(*)())&LuaProxy::Audio::MusicGetInstChannelCount),
                def("MusicInstChannelMute", (void(*)(int))&LuaProxy::Audio::MusicInstChannelMute),
                def("MusicInstChannelUnmute", (void(*)(int))&LuaProxy::Audio::MusicInstChannelUnmute),
                def("MusicSetTempo", (void(*)(double))&LuaProxy::Audio::MusicSetTempo),
                def("MusicSetPitch", (void(*)(double))&LuaProxy::Audio::MusicSetPitch),
                def("MusicSetSpeed", (void(*)(double))&LuaProxy::Audio::MusicSetSpeed),
                def("MusicGetTempo", (double(*)())&LuaProxy::Audio::MusicGetTempo),
                def("MusicGetPitch", (double(*)())&LuaProxy::Audio::MusicGetPitch),
                def("MusicGetSpeed", (double(*)())&LuaProxy::Audio::MusicGetSpeed),

                //SFX
                def("newMix_Chunk", (Mix_Chunk*(*)())&LuaProxy::Audio::newMix_Chunk),
                def("clearSFXBuffer", (void(*)())&LuaProxy::Audio::clearSFXBuffer),
                def("playSFX", (void(*)(int))&LuaProxy::playSFX),
                def("playSFX", (void(*)(const std::string&, lua_State*))&LuaProxy::Audio::playSFX),
                def("SfxOpen", (Mix_Chunk*(*)(const std::string&, lua_State*))&LuaProxy::Audio::SfxOpen),
                def("SfxPlayCh", (int(*)(int, Mix_Chunk*,int))&LuaProxy::Audio::SfxPlayCh),
                def("SfxPlayChVol", (int(*)(int, Mix_Chunk*,int,int))&LuaProxy::Audio::SfxPlayChVol),
                def("SfxPlayChTimed", (int(*)(int, Mix_Chunk*, int, int))&LuaProxy::Audio::SfxPlayChTimed),
                def("SfxPlayChTimedVol", (int(*)(int, Mix_Chunk*, int, int,int))&LuaProxy::Audio::SfxPlayChTimedVol),
                def("SfxFadeInCh", (int(*)(int, Mix_Chunk*, int, int))&LuaProxy::Audio::SfxFadeInCh),
                def("SfxFadeInChVol", (int(*)(int, Mix_Chunk*, int, int, int))&LuaProxy::Audio::SfxFadeInChVol),
                def("SfxFadeInChTimed", (int(*)(int, Mix_Chunk*, int, int, int))&LuaProxy::Audio::SfxFadeInChTimed),
                def("SfxFadeInChTimedVol", (int(*)(int, Mix_Chunk*, int, int, int, int))&LuaProxy::Audio::SfxFadeInChTimedVol),
                def("SfxPause", (void(*)(int))&LuaProxy::Audio::SfxPause),
                def("SfxResume", (void(*)(int))&LuaProxy::Audio::SfxResume),
                def("SfxStop", (int(*)(int))&LuaProxy::Audio::SfxStop),
                def("SfxExpire", (int(*)(int, int))&LuaProxy::Audio::SfxExpire),
                def("SfxFadeOut", (int(*)(int, int))&LuaProxy::Audio::SfxFadeOut),
                def("SfxIsPlaying", (int(*)(int))&LuaProxy::Audio::SfxIsPlaying),
                def("SfxIsPaused", (int(*)(int))&LuaProxy::Audio::SfxIsPaused),
                def("SfxIsFading", (int(*)(int))&LuaProxy::Audio::SfxIsFading),
                def("SfxVolume", (int(*)(int, int))&LuaProxy::Audio::SfxVolume),

                def("SfxSetPanning", (int(*)(int, int, int))&LuaProxy::Audio::SfxSetPanning),
                def("SfxSetDistance", (int(*)(int, int))&LuaProxy::Audio::SfxSetDistance),
                def("SfxSet3DPosition", (int(*)(int, int, int))&LuaProxy::Audio::SfxSet3DPosition),
                def("SfxReverseStereo", (int(*)(int, int))&LuaProxy::Audio::SfxReverseStereo),

                def("MixedSfxVolume", &LuaProxy::Audio::GetMixedSfxVolume),
                def("MixedSfxVolume", &LuaProxy::Audio::SetMixedSfxVolume),

                LUAHELPER_DEF_CLASS_SMART_PTR_SHARED(PlayingSfxInstance, std::shared_ptr)
                    .def("Pause", &LuaProxy::Audio::PlayingSfxInstance::Pause)
                    .def("Resume", &LuaProxy::Audio::PlayingSfxInstance::Resume)
                    .def("Stop", &LuaProxy::Audio::PlayingSfxInstance::Stop)
                    .def("Expire", &LuaProxy::Audio::PlayingSfxInstance::Expire)
                    .def("FadeOut", &LuaProxy::Audio::PlayingSfxInstance::FadeOut)
                    .def("IsPlaying", &LuaProxy::Audio::PlayingSfxInstance::IsPlaying)
                    .def("IsPaused", &LuaProxy::Audio::PlayingSfxInstance::IsPaused)
                    .def("IsFading", &LuaProxy::Audio::PlayingSfxInstance::IsFading)
                    .def("Volume", &LuaProxy::Audio::PlayingSfxInstance::Volume)
                    .def("SetPanning", &LuaProxy::Audio::PlayingSfxInstance::SetPanning)
                    .def("SetDistance", &LuaProxy::Audio::PlayingSfxInstance::SetDistance)
                    .def("Set3DPosition", &LuaProxy::Audio::PlayingSfxInstance::Set3DPosition)
                    .def("SetReverseStereo", &LuaProxy::Audio::PlayingSfxInstance::SetReverseStereo),

                def("SfxPlayObj", &LuaProxy::Audio::SfxPlayObj),
                def("SfxPlayObjVol", &LuaProxy::Audio::SfxPlayObjVol),
                def("SfxPlayObjTimed", &LuaProxy::Audio::SfxPlayObjTimed),
                def("SfxPlayObjTimedVol", &LuaProxy::Audio::SfxPlayObjTimedVol),
                def("SfxFadeInObj", &LuaProxy::Audio::SfxFadeInObj),
                def("SfxFadeInObjVol", &LuaProxy::Audio::SfxFadeInObjVol),
                def("SfxFadeInObjTimed", &LuaProxy::Audio::SfxFadeInObjTimed),
                def("SfxFadeInObjTimedVol", &LuaProxy::Audio::SfxFadeInObjTimedVol),

                //Time
                def("AudioClock", (double(*)())&LuaProxy::Audio::AudioClock),
                def("MusicClock", (double(*)())&LuaProxy::Audio::MusicClock),

                def("__setOverrideForAlias", LuaProxy::Audio::__setOverrideForAlias),
                def("__getChunkForAlias", LuaProxy::Audio::__getChunkForAlias),
                def("__setMuteForAlias", LuaProxy::Audio::__setMuteForAlias),
                def("__getMuteForAlias", LuaProxy::Audio::__getMuteForAlias)
            ],
            /*************************Audio*end*************************/

            LUAHELPER_DEF_CLASS(NativeInputConfig)
            .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::InputConfig, m_index))
            .property("idx", &LuaProxy::InputConfig::idx)
            .property("inputType", &LuaProxy::InputConfig::inputType, &LuaProxy::InputConfig::setInputType)
            .property("up", &LuaProxy::InputConfig::up, &LuaProxy::InputConfig::setUp)
            .property("down", &LuaProxy::InputConfig::down, &LuaProxy::InputConfig::setDown)
            .property("left", &LuaProxy::InputConfig::left, &LuaProxy::InputConfig::setLeft)
            .property("right", &LuaProxy::InputConfig::right, &LuaProxy::InputConfig::setRight)
            .property("run", &LuaProxy::InputConfig::run, &LuaProxy::InputConfig::setRun)
            .property("altrun", &LuaProxy::InputConfig::altrun, &LuaProxy::InputConfig::setAltRun)
            .property("jump", &LuaProxy::InputConfig::jump, &LuaProxy::InputConfig::setJump)
            .property("altjump", &LuaProxy::InputConfig::altjump, &LuaProxy::InputConfig::setAltJump)
            .property("dropitem", &LuaProxy::InputConfig::dropitem, &LuaProxy::InputConfig::setDropItem)
            .property("pause", &LuaProxy::InputConfig::pause, &LuaProxy::InputConfig::setPause),

            LUAHELPER_DEF_CLASS(RECT)
            .def_readwrite("left", &RECT::left)
            .def_readwrite("top", &RECT::top)
            .def_readwrite("right", &RECT::right)
            .def_readwrite("bottom", &RECT::bottom),

            LUAHELPER_DEF_CLASS(RECTd)
            .def_readwrite("left", &LuaProxy::RECTd::left)
            .def_readwrite("top", &LuaProxy::RECTd::top)
            .def_readwrite("right", &LuaProxy::RECTd::right)
            .def_readwrite("bottom", &LuaProxy::RECTd::bottom),

            LUAHELPER_DEF_CLASS_SMART_PTR_SHARED(Event, std::shared_ptr)
            .property("eventName", &Event::eventName)
            .property("cancellable", &Event::isCancellable)
            .property("cancelled", &Event::cancelled, &Event::setCancelled)
            .property("loopable", &Event::getLoopable, &Event::setLoopable)
            .property("directEventName", &Event::getDirectEventName, &Event::setDirectEventName),

            LUAHELPER_DEF_CLASS(Logger)
            .def(constructor<std::string>())
            .def("write", &LuaProxy::Logger::write),

            LUAHELPER_DEF_CLASS(Shader)
            .def(constructor<>())
            .def("compileFromSource", &LuaProxy::Shader::compileFromSource)
            .def("compileFromFile", &LuaProxy::Shader::compileFromFile)
            .def("getAttributeInfo", &LuaProxy::Shader::getAttributeInfo)
            .def("getUniformInfo", &LuaProxy::Shader::getUniformInfo)
            .property("isCompiled", &LuaProxy::Shader::isCompiled),

            LUAHELPER_DEF_CLASS(Data)
                .enum_("DataTypes")
                [
                    value("DATA_LEVEL", LuaProxy::Data::DATA_LEVEL),
                    value("DATA_WORLD", LuaProxy::Data::DATA_WORLD),
                    value("DATA_GLOBAL", LuaProxy::Data::DATA_GLOBAL)
                ]
            .def(constructor<LuaProxy::Data::DataType>())
            .def(constructor<LuaProxy::Data::DataType, std::string>())
            .def(constructor<LuaProxy::Data::DataType, bool>())
            .def(constructor<LuaProxy::Data::DataType, std::string, bool>())
            .def("set", &LuaProxy::Data::set)
            .def("get", static_cast<std::string(LuaProxy::Data::*)(const std::string &) const>(&LuaProxy::Data::get))
            .def("get", static_cast<luabind::object(LuaProxy::Data::*)(lua_State*) const>(&LuaProxy::Data::get))
            .def("save", static_cast<void(LuaProxy::Data::*)()>(&LuaProxy::Data::save))
            .def("save", static_cast<void(LuaProxy::Data::*)(const std::string &)>(&LuaProxy::Data::save))
            .property("dataType", &LuaProxy::Data::dataType, &LuaProxy::Data::setDataType)
            .property("sectionName", &LuaProxy::Data::sectionName, &LuaProxy::Data::setSectionName)
            .property("useSaveSlot", &LuaProxy::Data::useSaveSlot, &LuaProxy::Data::setUseSaveSlot),

            LUAHELPER_DEF_CLASS(AsyncHTTPRequest)
            .enum_("HTTP_METHOD")[
                value("HTTP_POST", AsyncHTTPClient::HTTP_POST),
                value("HTTP_GET", AsyncHTTPClient::HTTP_GET)
            ]
            .def(constructor<>())
            .def("addArgument", &LuaProxy::AsyncHTTPRequest::addArgument)
            .def("send", &LuaProxy::AsyncHTTPRequest::send)
            .def("wait", &LuaProxy::AsyncHTTPRequest::wait)
            .property("url", &LuaProxy::AsyncHTTPRequest::getUrl, &LuaProxy::AsyncHTTPRequest::setUrl)
            .property("method", &LuaProxy::AsyncHTTPRequest::getMethod, &LuaProxy::AsyncHTTPRequest::setMethod)
            .property("ready", &LuaProxy::AsyncHTTPRequest::isReady)
            .property("processing", &LuaProxy::AsyncHTTPRequest::isProcessing)
            .property("finished", &LuaProxy::AsyncHTTPRequest::isFinished)
            .property("responseText", &LuaProxy::AsyncHTTPRequest::responseText)
            .property("statusCode", &LuaProxy::AsyncHTTPRequest::statusCode),

            LUAHELPER_DEF_CLASS(PlayerSettings)
            .scope[
                def("get", &LuaProxy::PlayerSettings::get)
            ]
            .property("hitboxWidth", &LuaProxy::PlayerSettings::getHitboxWidth, &LuaProxy::PlayerSettings::setHitboxWidth)
            .property("hitboxHeight", &LuaProxy::PlayerSettings::getHitboxHeight, &LuaProxy::PlayerSettings::setHitboxHeight)
            .property("hitboxDuckHeight", &LuaProxy::PlayerSettings::getHitboxDuckHeight, &LuaProxy::PlayerSettings::setHitboxDuckHeight)
            .property("grabOffsetX", &LuaProxy::PlayerSettings::getGrabOffsetX, &LuaProxy::PlayerSettings::setGrabOffsetX)
            .property("grabOffsetY", &LuaProxy::PlayerSettings::getGrabOffsetY, &LuaProxy::PlayerSettings::setGrabOffsetY)
            .def("getSpriteOffsetX", &LuaProxy::PlayerSettings::getSpriteOffsetX)
            .def("setSpriteOffsetX", &LuaProxy::PlayerSettings::setSpriteOffsetX)
            .def("getSpriteOffsetY", &LuaProxy::PlayerSettings::getSpriteOffsetY)
            .def("setSpriteOffsetY", &LuaProxy::PlayerSettings::setSpriteOffsetY)


            .property("character", &LuaProxy::PlayerSettings::getCharacter, &LuaProxy::PlayerSettings::setCharacter)
            .property("powerup", &LuaProxy::PlayerSettings::getPowerupID, &LuaProxy::PlayerSettings::setPowerupID),

            def("newRECT", &LuaProxy::newRECT),
            def("newRECTd", &LuaProxy::newRECTd),

            namespace_("UserData")[
                def("setValue", &LuaProxy::SaveBankProxy::setValue),
                def("getValue", &LuaProxy::SaveBankProxy::getValue),
                def("isValueSet", &LuaProxy::SaveBankProxy::isValueSet),
                def("values", &LuaProxy::SaveBankProxy::values),
                def("save", &LuaProxy::SaveBankProxy::save)
            ],

            class_<LuaProxy::Console>("Console")
            .def("print", &LuaProxy::Console::print)
            .def("println", &LuaProxy::Console::println)
            .def("clear", &LuaProxy::Console::clear)
        ];
    if(m_type == LUNALUA_WORLD){
        module(L)
            [
                namespace_("Graphics")[
                    def("activateOverworldHud", &LuaProxy::Graphics::activateOverworldHud),
                    def("getOverworldHudState", &LuaProxy::Graphics::getOverworldHudState)
                ],

                LUAHELPER_DEF_CLASS(World)
                .property("playerX", &LuaProxy::World::playerX, &LuaProxy::World::setPlayerX)
                .property("playerY", &LuaProxy::World::playerY, &LuaProxy::World::setPlayerY)
                .property("playerWalkingDirection", &LuaProxy::World::currentWalkingDirection, &LuaProxy::World::setCurrentWalkingDirection)
                .property("playerWalkingTimer", &LuaProxy::World::currentWalkingTimer, &LuaProxy::World::setCurrentWalkingTimer)
                .property("playerWalkingFrame", &LuaProxy::World::currentWalkingFrame, &LuaProxy::World::setCurrentWalkingFrame)
                .property("playerWalkingFrameTimer", &LuaProxy::World::currentWalkingFrameTimer, &LuaProxy::World::setCurrentWalkingFrameTimer)
                .property("playerIsCurrentWalking", &LuaProxy::World::playerIsCurrentWalking)
                .property("levelTitle", &LuaProxy::World::levelTitle)
                .property("levelObj", &LuaProxy::World::levelObj)
                .property("playerCurrentDirection", &LuaProxy::World::getCurrentDirection)
                .property("playerPowerup", &LuaProxy::World::playerPowerup, &LuaProxy::World::setPlayerPowerup)
                .def("mem", static_cast<void (LuaProxy::World::*)(int, LuaProxy::L_FIELDTYPE, const luabind::object &, lua_State*)>(&LuaProxy::World::mem))
                .def("mem", static_cast<luabind::object(LuaProxy::World::*)(int, LuaProxy::L_FIELDTYPE, lua_State*) const>(&LuaProxy::World::mem)),

                LUAHELPER_DEF_CLASS(Tile)
                .scope[ //static functions
                    def("count", &LuaProxy::Tile::count),
                    def("get", static_cast<luabind::object(*)(lua_State* L)>(&LuaProxy::Tile::get)),
                    def("get", static_cast<luabind::object(*)(luabind::object, lua_State* L)>(&LuaProxy::Tile::get)),
                    def("getIntersecting", &LuaProxy::Tile::getIntersecting)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Tile, m_index))
                .def(constructor<int>())
                .property("idx", &LuaProxy::Tile::idx)
                .property("id", &LuaProxy::Tile::id, &LuaProxy::Tile::setId)
                .property("x", &LuaProxy::Tile::x, &LuaProxy::Tile::setX)
                .property("y", &LuaProxy::Tile::y, &LuaProxy::Tile::setY)
                .property("width", &LuaProxy::Tile::width, &LuaProxy::Tile::setWidth)
                .property("height", &LuaProxy::Tile::height, &LuaProxy::Tile::setHeight)
                .property("isValid", &LuaProxy::Tile::isValid),

                LUAHELPER_DEF_CLASS(Scenery)
                .scope[ //static functions
                    def("count", &LuaProxy::Scenery::count),
                    def("get", static_cast<luabind::object(*)(lua_State* L)>(&LuaProxy::Scenery::get)),
                    def("get", static_cast<luabind::object(*)(luabind::object, lua_State* L)>(&LuaProxy::Scenery::get)),
                    def("getIntersecting", &LuaProxy::Scenery::getIntersecting)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Scenery, m_index))
                .def(constructor<int>())
                .property("idx", &LuaProxy::Scenery::idx)
                .property("id", &LuaProxy::Scenery::id, &LuaProxy::Scenery::setId)
                .property("x", &LuaProxy::Scenery::x, &LuaProxy::Scenery::setX)
                .property("y", &LuaProxy::Scenery::y, &LuaProxy::Scenery::setY)
                .property("width", &LuaProxy::Scenery::width, &LuaProxy::Scenery::setWidth)
                .property("height", &LuaProxy::Scenery::height, &LuaProxy::Scenery::setHeight)
                .property("isValid", &LuaProxy::Scenery::isValid),

                LUAHELPER_DEF_CLASS(Path)
                .scope[ //static functions
                    def("count", &LuaProxy::Path::count),
                    def("get", static_cast<luabind::object(*)(lua_State* L)>(&LuaProxy::Path::get)),
                    def("get", static_cast<luabind::object(*)(luabind::object, lua_State* L)>(&LuaProxy::Path::get)),
                    def("getIntersecting", &LuaProxy::Path::getIntersecting)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Path, m_index))
                .def(constructor<int>())
                .property("idx", &LuaProxy::Path::idx)
                .property("id", &LuaProxy::Path::id, &LuaProxy::Path::setId)
                .property("x", &LuaProxy::Path::x, &LuaProxy::Path::setX)
                .property("y", &LuaProxy::Path::y, &LuaProxy::Path::setY)
                .property("width", &LuaProxy::Path::width, &LuaProxy::Path::setWidth)
                .property("height", &LuaProxy::Path::height, &LuaProxy::Path::setHeight)
                .property("visible", &LuaProxy::Path::visible, &LuaProxy::Path::setVisible)
                .property("isValid", &LuaProxy::Path::isValid),

                LUAHELPER_DEF_CLASS(Musicbox)
                .scope[ //static functions
                    def("count", &LuaProxy::Musicbox::count),
                    def("get", static_cast<luabind::object(*)(lua_State* L)>(&LuaProxy::Musicbox::get)),
                    def("get", static_cast<luabind::object(*)(luabind::object, lua_State* L)>(&LuaProxy::Musicbox::get)),
                    def("getIntersecting", &LuaProxy::Musicbox::getIntersecting)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Musicbox, m_index))
                .def(constructor<int>())
                .property("idx", &LuaProxy::Musicbox::idx)
                .property("id", &LuaProxy::Musicbox::id, &LuaProxy::Musicbox::setId)
                .property("x", &LuaProxy::Musicbox::x, &LuaProxy::Musicbox::setX)
                .property("y", &LuaProxy::Musicbox::y, &LuaProxy::Musicbox::setY)
                .property("width", &LuaProxy::Musicbox::width, &LuaProxy::Musicbox::setWidth)
                .property("height", &LuaProxy::Musicbox::height, &LuaProxy::Musicbox::setHeight)
                .property("isValid", &LuaProxy::Musicbox::isValid),

                LUAHELPER_DEF_CLASS(Level)
                .scope[ //static functions
                        def("count", &LuaProxy::LevelObject::count),
                        def("get", (luabind::object(*)(lua_State* L))&LuaProxy::LevelObject::get),
                        def("get", (luabind::object(*)(luabind::object, lua_State* L))&LuaProxy::LevelObject::get),
                        def("getByName", &LuaProxy::LevelObject::getByName),
                        def("getByFilename", &LuaProxy::LevelObject::getByFilename),
                        def("findByName", &LuaProxy::LevelObject::findByName),
                        def("findByFilename", &LuaProxy::LevelObject::findByFilename)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::LevelObject, m_index))
                .property("idx", &LuaProxy::LevelObject::idx)
                .property("x", &LuaProxy::LevelObject::x, &LuaProxy::LevelObject::setX)
                .property("y", &LuaProxy::LevelObject::y, &LuaProxy::LevelObject::setY)
                .property("goToX", &LuaProxy::LevelObject::goToX, &LuaProxy::LevelObject::setGoToX)
                .property("goToY", &LuaProxy::LevelObject::goToY, &LuaProxy::LevelObject::setGoToY)
                .property("topExitType", &LuaProxy::LevelObject::topExitType, &LuaProxy::LevelObject::setTopExitType)
                .property("leftExitType", &LuaProxy::LevelObject::leftExitType, &LuaProxy::LevelObject::setLeftExitType)
                .property("bottomExitType", &LuaProxy::LevelObject::bottomExitType, &LuaProxy::LevelObject::setBottomExitType)
                .property("rightExitType", &LuaProxy::LevelObject::rightExitType, &LuaProxy::LevelObject::setRightExitType)
                .property("visible", &LuaProxy::LevelObject::visible, &LuaProxy::LevelObject::setVisible)
                .property("levelWarpNumber", &LuaProxy::LevelObject::levelWarpNumber, &LuaProxy::LevelObject::setLevelWarpNumber)
                .property("isPathBackground", &LuaProxy::LevelObject::isPathBackground, &LuaProxy::LevelObject::setIsPathBackground)
                .property("isBigBackground", &LuaProxy::LevelObject::isBigBackground, &LuaProxy::LevelObject::setIsBigBackground)
                .property("isGameStartPoint", &LuaProxy::LevelObject::isGameStartPoint, &LuaProxy::LevelObject::setIsGameStartPoint)
                .property("isAlwaysVisible", &LuaProxy::LevelObject::isAlwaysVisible, &LuaProxy::LevelObject::setIsAlwaysVisible)
                .property("title", &LuaProxy::LevelObject::title)
                .property("filename", &LuaProxy::LevelObject::filename)
                .def("mem", static_cast<void (LuaProxy::LevelObject::*)(int, LuaProxy::L_FIELDTYPE, const luabind::object &, lua_State*)>(&LuaProxy::LevelObject::mem))
                .def("mem", static_cast<luabind::object(LuaProxy::LevelObject::*)(int, LuaProxy::L_FIELDTYPE, lua_State*) const>(&LuaProxy::LevelObject::mem))

            ];
    }

    if(m_type == LUNALUA_LEVEL){
        module(L)
            [
                namespace_("Text")[
                    def("showMessageBox", &LuaProxy::Text::showMessageBox)
                ],

                namespace_("Misc")[
                    def("npcToCoins", &LuaProxy::Misc::npcToCoins),
                    def("doPOW", &LuaProxy::Misc::doPOW),
                    def("doPSwitchRaw", &LuaProxy::Misc::doPSwitchRaw),
                    def("doPSwitch", (void(*)())&LuaProxy::Misc::doPSwitch),
                    def("doPSwitch", (void(*)(bool))&LuaProxy::Misc::doPSwitch),
                    def("doBombExplosion", (void(*)(double, double, short))&LuaProxy::Misc::doBombExplosion),
                    def("_setSemisolidCollidingFlyType", &NPC::SetSemisolidCollidingFlyType),
                    def("_npcHarmCombo", NPC::HarmCombo),
                    def("_npcHarmComboWithDamage", NPC::HarmComboWithDamage),
                    def("_npcCollect", NPC::Collect),
                    def("_playerHarm", Player::Harm),
                    def("_playerKill", Player::Kill)
                    //def("doBombExplosion", (void(*)(double, double, short, const LuaProxy::Player&))&LuaProxy::Misc::doBombExplosion)
                ],

                namespace_("Level")[
                    def("filename", &LuaProxy::Level::filename),
                    def("name", &LuaProxy::Level::name),
                    // This isn't just useful in level situation... it is useful for overworld too, so, there's a copy in Misc too
                    def("loadPlayerHitBoxes", (void(*)(int, int, const std::string&))&LuaProxy::loadHitboxes)
                ],

                namespace_("Graphics")[
                    def("activateHud", &LuaProxy::Graphics::activateHud),
                    def("isHudActivated", &LuaProxy::Graphics::isHudActivated)
                ],

                LUAHELPER_DEF_CLASS(Warp)
                .scope[
                        def("count", &LuaProxy::Warp::count),
                        def("get", &LuaProxy::Warp::get),
                        def("getIntersectingEntrance", &LuaProxy::Warp::getIntersectingEntrance),
                        def("getIntersectingExit", &LuaProxy::Warp::getIntersectingExit)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Warp, m_index))
                .def(constructor<int>())
                .def("mem", static_cast<void (LuaProxy::Warp::*)(int, LuaProxy::L_FIELDTYPE, const luabind::object &, lua_State*)>(&LuaProxy::Warp::mem))
                .def("mem", static_cast<luabind::object(LuaProxy::Warp::*)(int, LuaProxy::L_FIELDTYPE, lua_State*) const>(&LuaProxy::Warp::mem))
                .property("idx", &LuaProxy::Warp::idx)
                .property("isHidden", LUAPROXY_REG_RW_CUSTOM(LuaProxy::Warp, isHidden, bool))
                .property("exitX", LUAPROXY_REG_RW_MOMENTUM(LuaProxy::Warp, exit, x))
                .property("exitY", LUAPROXY_REG_RW_MOMENTUM(LuaProxy::Warp, exit, y))
                .property("entranceX", LUAPROXY_REG_RW_MOMENTUM(LuaProxy::Warp, entrance, x))
                .property("entranceY", LUAPROXY_REG_RW_MOMENTUM(LuaProxy::Warp, entrance, y))
                .property("levelFilename", &LuaProxy::Warp::levelFilename, &LuaProxy::Warp::setLevelFilename),



                LUAHELPER_DEF_CLASS(Animation)
                .scope[ //static functions
                        def("count", &LuaProxy::Animation::count),
                        def("get", static_cast<luabind::object(*)(lua_State* L)>(&LuaProxy::Animation::get)),
                        def("get", static_cast<luabind::object(*)(luabind::object, lua_State* L)>(&LuaProxy::Animation::get)),
                        def("getIntersecting", &LuaProxy::Animation::getIntersecting),
                        def("spawn", (LuaProxy::Animation(*)(short, double, double, lua_State*))&LuaProxy::Animation::spawnEffect), //DONE
                        def("spawn", (LuaProxy::Animation(*)(short, double, double, float, lua_State*))&LuaProxy::Animation::spawnEffect) //DONE
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Animation, m_animationIndex))
                .def(constructor<int>())
                .def("mem", static_cast<void (LuaProxy::Animation::*)(int, LuaProxy::L_FIELDTYPE, const luabind::object &, lua_State*)>(&LuaProxy::Animation::mem))
                .def("mem", static_cast<luabind::object (LuaProxy::Animation::*)(int, LuaProxy::L_FIELDTYPE, lua_State*) const>(&LuaProxy::Animation::mem))
                .property("idx", &LuaProxy::Animation::idx)
                .property("id", &LuaProxy::Animation::id, &LuaProxy::Animation::setId)
                .property("x", &LuaProxy::Animation::x, &LuaProxy::Animation::setX)
                .property("y", &LuaProxy::Animation::y, &LuaProxy::Animation::setY)
                .property("speedX", &LuaProxy::Animation::speedX, &LuaProxy::Animation::setSpeedX)
                .property("speedY", &LuaProxy::Animation::speedY, &LuaProxy::Animation::setSpeedY)
                .property("width", &LuaProxy::Animation::width, &LuaProxy::Animation::setWidth)
                .property("height", &LuaProxy::Animation::height, &LuaProxy::Animation::setHeight)
                .property("subTimer", &LuaProxy::Animation::subTimer, &LuaProxy::Animation::setSubTimer)
                .property("timer", &LuaProxy::Animation::timer, &LuaProxy::Animation::setTimer)
                .property("animationFrame", &LuaProxy::Animation::animationFrame, &LuaProxy::Animation::setAnimationFrame)
                .property("npcID", &LuaProxy::Animation::npcID, &LuaProxy::Animation::setNpcID)
                .property("drawOnlyMask", &LuaProxy::Animation::drawOnlyMask, &LuaProxy::Animation::setDrawOnlyMask)
                .property("isValid", &LuaProxy::Animation::isValid),

                LUAHELPER_DEF_CLASS(Layer)
                .scope[ //static functions
                        def("get", (luabind::object(*)(lua_State* L))&LuaProxy::Layer::get),
                        def("get", (luabind::object(*)(const std::string& , lua_State* L))&LuaProxy::Layer::get),
                        def("find", &LuaProxy::Layer::find)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Layer, m_layerIndex))
                .def(constructor<int>())
                .def("stop", &LuaProxy::Layer::stop)
                .def("show", &LuaProxy::Layer::show)
                .def("hide", &LuaProxy::Layer::hide)
                .def("toggle", &LuaProxy::Layer::toggle)
                .property("idx", &LuaProxy::Layer::idx)
                .property("isHidden", &LuaProxy::Layer::isHidden)
                .property("speedX", &LuaProxy::Layer::speedX, &LuaProxy::Layer::setSpeedX)
                .property("speedY", &LuaProxy::Layer::speedY, &LuaProxy::Layer::setSpeedY)
                .property("layerName", &LuaProxy::Layer::layerName),

                LUAHELPER_DEF_CLASS(Section)
                .scope[
                    def("get", (luabind::object(*)(lua_State* L))&LuaProxy::Section::get),
                    def("get", (LuaProxy::Section(*)(short, lua_State* L))&LuaProxy::Section::get)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Section, m_secNum))
                .def(constructor<int>())
                .property("idx", &LuaProxy::Section::idx)
                .property("boundary", &LuaProxy::Section::boundary, &LuaProxy::Section::setBoundary)
                .property("musicID", &LuaProxy::Section::musicID, &LuaProxy::Section::setMusicID)
                .property("isLevelWarp", &LuaProxy::Section::isLevelWarp, &LuaProxy::Section::setIsLevelWarp)
                .property("hasOffscreenExit", &LuaProxy::Section::hasOffscreenExit, &LuaProxy::Section::setHasOffscreenExit)
                .property("backgroundID", &LuaProxy::Section::backgroundID, &LuaProxy::Section::setBackgroundID)
                .property("noTurnBack", &LuaProxy::Section::noTurnBack, &LuaProxy::Section::setNoTurnBack)
                .property("isUnderwater", &LuaProxy::Section::isUnderwater, &LuaProxy::Section::setIsUnderwater),

                LUAHELPER_DEF_CLASS(NPC)
                .scope[ //static functions
                    def("count", &LuaProxy::NPC::count),
                    def("get", (luabind::object(*)(lua_State* L))&LuaProxy::NPC::get),
                    def("get", (luabind::object(*)(luabind::object, lua_State* L))&LuaProxy::NPC::get),
                    def("get", (luabind::object(*)(luabind::object, luabind::object, lua_State* L))&LuaProxy::NPC::get),
                    def("getIntersecting", &LuaProxy::NPC::getIntersecting),
                    def("spawn", static_cast<LuaProxy::NPC(*)(short, double, double, short, lua_State*)>(&LuaProxy::spawnNPC)),
                    def("spawn", static_cast<LuaProxy::NPC(*)(short, double, double, short, bool, lua_State*)>(&LuaProxy::spawnNPC)),
                    def("spawn", static_cast<LuaProxy::NPC(*)(short, double, double, short, bool, bool, lua_State*)>(&LuaProxy::spawnNPC))
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::NPC, m_index))
                .def(constructor<int>())
                .def("mem", static_cast<void (LuaProxy::NPC::*)(int, LuaProxy::L_FIELDTYPE, const luabind::object &, lua_State*)>(&LuaProxy::NPC::mem))
                .def("mem", static_cast<luabind::object(LuaProxy::NPC::*)(int, LuaProxy::L_FIELDTYPE, lua_State*) const>(&LuaProxy::NPC::mem))
                .def("kill", static_cast<void (LuaProxy::NPC::*)(lua_State*)>(&LuaProxy::NPC::kill))
                .def("kill", static_cast<void (LuaProxy::NPC::*)(int, lua_State*)>(&LuaProxy::NPC::kill))
                .def("toIce", &LuaProxy::NPC::toIce)
                .def("toCoin", &LuaProxy::NPC::toCoin)
                .def("harm", static_cast<void (LuaProxy::NPC::*)(lua_State*)>(&LuaProxy::NPC::harm))
                .def("harm", static_cast<void (LuaProxy::NPC::*)(short, lua_State*)>(&LuaProxy::NPC::harm))
                .def("harm", static_cast<void (LuaProxy::NPC::*)(short, float, lua_State*)>(&LuaProxy::NPC::harm))
                .def("harmCombo", static_cast<short (LuaProxy::NPC::*)(short, short, lua_State*)>(&LuaProxy::NPC::harmCombo))
                .def("harmCombo", static_cast<short (LuaProxy::NPC::*)(short, short, float, lua_State*)>(&LuaProxy::NPC::harmCombo))
                .property("idx", &LuaProxy::NPC::idx)
                .property("id", &LuaProxy::NPC::id, &LuaProxy::NPC::setId)
                .property("isHidden", &LuaProxy::NPC::isHidden, &LuaProxy::NPC::setIsHidden)
                .property("direction", &LuaProxy::NPC::direction, &LuaProxy::NPC::setDirection)
                .property("x", &LuaProxy::NPC::x, &LuaProxy::NPC::setX)
                .property("y", &LuaProxy::NPC::y, &LuaProxy::NPC::setY)
                .property("width", &LuaProxy::NPC::width, &LuaProxy::NPC::setWidth)
                .property("height", &LuaProxy::NPC::height, &LuaProxy::NPC::setHeight)
                .property("speedX", &LuaProxy::NPC::speedX, &LuaProxy::NPC::setSpeedX)
                .property("speedY", &LuaProxy::NPC::speedY, &LuaProxy::NPC::setSpeedY)
                .property("attachedLayerName", &LuaProxy::NPC::attachedLayerName, &LuaProxy::NPC::setAttachedLayerName)
                .property("activateEventName", &LuaProxy::NPC::activateEventName, &LuaProxy::NPC::setActivateEventName)
                .property("deathEventName", &LuaProxy::NPC::deathEventName, &LuaProxy::NPC::setDeathEventName)
                .property("noMoreObjInLayer", &LuaProxy::NPC::noMoreObjInLayer, &LuaProxy::NPC::setNoMoreObjInLayer)
                .property("talkEventName", &LuaProxy::NPC::talkEventName, &LuaProxy::NPC::setTalkEventName)
                .property("msg", &LuaProxy::NPC::msg, &LuaProxy::NPC::setMsg)
                .property("layerName", &LuaProxy::NPC::layerName, &LuaProxy::NPC::setLayerName)
                .property("attachedLayerObj", &LuaProxy::NPC::attachedLayerObj, &LuaProxy::NPC::setAttachedLayerObj)
                .property("layerObj", &LuaProxy::NPC::layerObj, &LuaProxy::NPC::setLayerObj)
                .property("ai1", &LuaProxy::NPC::ai1, &LuaProxy::NPC::setAi1)
                .property("ai2", &LuaProxy::NPC::ai2, &LuaProxy::NPC::setAi2)
                .property("ai3", &LuaProxy::NPC::ai3, &LuaProxy::NPC::setAi3)
                .property("ai4", &LuaProxy::NPC::ai4, &LuaProxy::NPC::setAi4)
                .property("ai5", &LuaProxy::NPC::ai5, &LuaProxy::NPC::setAi5)
                .property("drawOnlyMask", &LuaProxy::NPC::drawOnlyMask, &LuaProxy::NPC::setDrawOnlyMask)
                .property("invincibleToSword", &LuaProxy::NPC::isInvincibleToSword, &LuaProxy::NPC::setIsInvincibleToSword)
                .property("legacyBoss", &LuaProxy::NPC::legacyBoss, &LuaProxy::NPC::setLegacyBoss)
                .property("friendly", &LuaProxy::NPC::friendly, &LuaProxy::NPC::setFriendly)
                .property("dontMove", &LuaProxy::NPC::dontMove, &LuaProxy::NPC::setDontMove)
                .property("collidesBlockBottom", &LuaProxy::NPC::collidesBlockBottom, &LuaProxy::NPC::setCollidesBlockBottom)
                .property("collidesBlockLeft", &LuaProxy::NPC::collidesBlockLeft, &LuaProxy::NPC::setCollidesBlockLeft)
                .property("collidesBlockUp", &LuaProxy::NPC::collidesBlockUp, &LuaProxy::NPC::setCollidesBlockUp)
                .property("collidesBlockRight", &LuaProxy::NPC::collidesBlockRight, &LuaProxy::NPC::setCollidesBlockRight)
                .property("underwater", &LuaProxy::NPC::isUnderwater, &LuaProxy::NPC::setIsUnderwater)
                .property("animationFrame", &LuaProxy::NPC::animationFrame, &LuaProxy::NPC::setAnimationFrame)
                .property("animationTimer", &LuaProxy::NPC::animationTimer, &LuaProxy::NPC::setAnimationTimer)
                .property("killFlag", &LuaProxy::NPC::killFlag, &LuaProxy::NPC::setKillFlag)
                .property("isValid", &LuaProxy::NPC::isValid),


                LUAHELPER_DEF_CLASS(Block)
                .scope[ //static functions
                        def("count", &LuaProxy::Block::count),
                        def("get", (luabind::object(*)(lua_State* L))&LuaProxy::Block::get),
                        def("get", (luabind::object(*)(luabind::object, lua_State* L))&LuaProxy::Block::get),
                        def("getIntersecting", &LuaProxy::Block::getIntersecting),
                        def("spawn", &LuaProxy::Block::spawn),
                        def("_getBumpable", &LuaProxy::Block::_getBumpable),
                        def("_setBumpable", &LuaProxy::Block::_setBumpable),
                        def("_rawHitBlock", &LuaProxy::Block::_rawHitBlock)
                ]
                .def("__eq", LUAPROXY_DEFUSERDATAINEDXCOMPARE(LuaProxy::Block, m_index))
                .def(constructor<int>())
                .def("mem", static_cast<void (LuaProxy::Block::*)(int, LuaProxy::L_FIELDTYPE, const luabind::object&, lua_State*)>(&LuaProxy::Block::mem))
                .def("mem", static_cast<luabind::object (LuaProxy::Block::*)(int, LuaProxy::L_FIELDTYPE, lua_State*) const>(&LuaProxy::Block::mem))
                //.def("collidesWith", &LuaProxy::Block::collidesWith)
                .def("remove", static_cast<void (LuaProxy::Block::*)()>(&LuaProxy::Block::remove))
                .def("remove", static_cast<void (LuaProxy::Block::*)(bool)>(&LuaProxy::Block::remove))
                //.def("hit", static_cast<void (LuaProxy::Block::*)()>(&LuaProxy::Block::hit))
                //.def("hit", static_cast<void (LuaProxy::Block::*)(bool)>(&LuaProxy::Block::hit))
                //.def("hit", static_cast<void (LuaProxy::Block::*)(bool, LuaProxy::Player)>(&LuaProxy::Block::hit))
                //.def("hit", static_cast<void (LuaProxy::Block::*)(bool, LuaProxy::Player, int)>(&LuaProxy::Block::hit))
                .property("idx", &LuaProxy::Block::idx)
                .property("x", &LuaProxy::Block::x, &LuaProxy::Block::setX)
                .property("y", &LuaProxy::Block::y, &LuaProxy::Block::setY)
                .property("width", &LuaProxy::Block::width, &LuaProxy::Block::setWidth)
                .property("height", &LuaProxy::Block::height, &LuaProxy::Block::setHeight)
                .property("speedX", &LuaProxy::Block::speedX, &LuaProxy::Block::setSpeedX)
                .property("speedY", &LuaProxy::Block::speedY, &LuaProxy::Block::setSpeedY)
                .property("id", &LuaProxy::Block::id, &LuaProxy::Block::setId)
                .property("contentID", &LuaProxy::Block::contentID, &LuaProxy::Block::setContentID)
                .property("isHidden", &LuaProxy::Block::isHidden, &LuaProxy::Block::setIsHidden)
                .property("invisible", &LuaProxy::Block::isHidden, &LuaProxy::Block::setIsHidden)
                .property("slippery", &LuaProxy::Block::slippery, &LuaProxy::Block::setSlippery)
                .property("layerName", &LuaProxy::Block::layerName)
                .property("layerObj", &LuaProxy::Block::layerObj)
            ];
    }
}


void CLunaLua::bindAllDeprecated()
{
    module(L)
        [
            def("getSMBXPath", &LuaProxy::Native::getSMBXPath), //DONE
            def("simulateError", &LuaProxy::Native::simulateError), //DONE
            def("windowDebug", &LuaProxy::Text::windowDebug), //DONE
            def("printText", (void(*)(const luabind::object&, int, int)) &LuaProxy::Text::print), //DONE
            def("printText", (void(*)(const luabind::object&, int, int, int)) &LuaProxy::Text::print), //DONE
            def("loadImage", (bool(*)(const std::string&, int, int))&LuaProxy::Graphics::loadImage), //DONE
            def("placeSprite", (void(*)(int, int, int, int, const std::string&, int))&LuaProxy::Graphics::placeSprite), //DONE
            def("placeSprite", (void(*)(int, int, int, int, const std::string&))&LuaProxy::Graphics::placeSprite), //DONE
            def("placeSprite", (void(*)(int, int, int, int))&LuaProxy::Graphics::placeSprite), //DONE

            /*************************Audio*****************************/
            //Old Audio stuff
            def("playSFX", (void(*)(int))&LuaProxy::playSFX),
            def("playSFX", (void(*)(const std::string&, lua_State*))&LuaProxy::playSFX),
            def("playSFXSDL", (void(*)(const std::string&, lua_State*))&LuaProxy::playSFXSDL),
            def("clearSFXBuffer", (void(*)())&LuaProxy::clearSFXBuffer),
            def("MusicOpen", (void(*)(const std::string&))&LuaProxy::MusicOpen),
            def("MusicPlay", (void(*)())&LuaProxy::MusicPlay),
            def("MusicPlayFadeIn", (void(*)(int))&LuaProxy::MusicPlay),
            def("MusicStop", (void(*)())&LuaProxy::MusicStop),
            def("MusicStopFadeOut", (void(*)(int))&LuaProxy::MusicStopFadeOut),
            def("MusicVolume", (void(*)(int))&LuaProxy::MusicVolume)
        ];
    if (m_type == LUNALUA_WORLD){
        module(L)
            [
                def("levels", &LuaProxy::levels),
                def("findlevels", &LuaProxy::findlevels),
                def("findlevel", &LuaProxy::findlevel)
            ];
    }
    if (m_type == LUNALUA_LEVEL){
        module(L)
            [
                def("showMessageBox", &LuaProxy::Text::showMessageBox), //DONE
                def("totalNPC", &LuaProxy::totalNPCs), //DONE
                def("npcs", &LuaProxy::npcs), //DONE
                def("findnpcs", &LuaProxy::findNPCs), //New version working = DONE
                def("triggerEvent", &LuaProxy::triggerEvent), //In next version event namespace
                def("playMusic", &LuaProxy::playMusic), //DONE
                def("loadHitboxes", (void(*)(int, int, const std::string&))&LuaProxy::loadHitboxes),
                def("gravity", (unsigned short(*)())&LuaProxy::gravity), //DONE [DEPRECATED]
                def("gravity", (void(*)(unsigned short))&LuaProxy::gravity), //DONE [DEPRECATED]
                def("earthquake", (unsigned short(*)())&LuaProxy::earthquake), //DONE [DEPRECATED]
                def("earthquake", (void(*)(unsigned short))&LuaProxy::earthquake), //DONE [DEPRECATED]
                def("jumpheight", (unsigned short(*)())&LuaProxy::jumpheight), //DONE [DEPRECATED]
                def("jumpheight", (void(*)(unsigned short))&LuaProxy::jumpheight), //DONE [DEPRECATED]
                def("jumpheightBounce", (unsigned short(*)())&LuaProxy::jumpheightBounce), //DONE [DEPRECATED]
                def("jumpheightBounce", (void(*)(unsigned short))&LuaProxy::jumpheightBounce), //DONE [DEPRECATED]
                def("runAnimation", (void(*)(int, double, double, double, double, double, double, int))&LuaProxy::runAnimation), //DONE [DEPRECATED]
                def("runAnimation", (void(*)(int, double, double, double, double, int))&LuaProxy::runAnimation), //DONE [DEPRECATED]
                def("runAnimation", (void(*)(int, double, double, int))&LuaProxy::runAnimation), //DONE
                def("npcToCoins", &LuaProxy::Misc::npcToCoins), //DONE
                def("blocks", &LuaProxy::blocks),   //DONE
                def("findblocks", &LuaProxy::findblocks),   //DONE
                def("findlayer", &LuaProxy::findlayer),     //DONE
                def("animations", &LuaProxy::animations), //DONE
                def("getInput", &LuaProxy::getInput), //DONE
                def("hud", &LuaProxy::Graphics::activateHud), //DONE
                def("getLevelFilename", &LuaProxy::Level::filename), //DONE
                def("getLevelName", &LuaProxy::Level::name), //DONE
                def("spawnNPC", static_cast<LuaProxy::NPC(*)(short, double, double, short, lua_State*)>(&LuaProxy::spawnNPC)), //DONE
                def("spawnNPC", static_cast<LuaProxy::NPC(*)(short, double, double, short, bool, lua_State*)>(&LuaProxy::spawnNPC)), //DONE
                def("spawnNPC", static_cast<LuaProxy::NPC(*)(short, double, double, short, bool, bool, lua_State*)>(&LuaProxy::spawnNPC)), //DONE
                def("spawnEffect", (LuaProxy::Animation(*)(short, double, double, lua_State*))&LuaProxy::spawnEffect), //DONE
                def("spawnEffect", (LuaProxy::Animation(*)(short, double, double, float, lua_State*))&LuaProxy::spawnEffect) //DONE
            ];

    }
}

void CLunaLua::triggerOnStart()
{
    GLEngineProxy::CheckRendererInit();

    //If the lua module is not valid anyway, then just return
    if (!isValid())
    {
        return;
    }

    // If is not ready (SMBX not init), then skip event loop
    if (!m_ready)
    {
        return;
    }

    if (!m_onStartRan) {
        // manually flag all players to have onSectionChange called following onStart
        for (int i = 1; i <= GM_PLAYERS_COUNT; i++) {
            gLunaLua.queuePlayerSectionChangeEvent(i);
        }
        // execute onStart event
        std::shared_ptr<Event> onStartEvent = std::make_shared<Event>("onStart", false);
        onStartEvent->setLoopable(false);
        onStartEvent->setDirectEventName("onStart");
        m_onStartRan = true;
        callEvent(onStartEvent);
    }
}

void CLunaLua::doEvents()
{
    GLEngineProxy::CheckRendererInit();

    //If the lua module is not valid anyway, then just return
    if(!isValid())
        return;

    // If is not ready (SMBX not init), then skip event loop
    if (!m_ready)
        return;


    //If player is not valid then shutdown the lua module
    if(m_type == LUNALUA_LEVEL){
        if(!Player::Get(1)){
            shutdown();
            return;
        }
    }

    MusicManager::setCurrentSection(Player::Get(1)->CurrentSection);

    triggerOnStart();

    // If an error happened in onStart then return.
    if (!isValid())
        return;

    std::shared_ptr<Event> onLoopEvent = std::make_shared<Event>("onLoop", false);
    onLoopEvent->setLoopable(false);
    onLoopEvent->setDirectEventName("onLoop");
    callEvent(onLoopEvent);

    callLuaFunction(L, "__doEventQueue");
}

void CLunaLua::checkWarnings()
{
    if (m_warningList.size() > 0)
    {
        std::wostringstream message;
        message << L"Warnings occured during run:\r\n";
        for (auto iter = m_warningList.cbegin(), end = m_warningList.cend(); iter != end; ++iter)
        {
            message << L" - " << Str2WStr(*iter) << L"\r\n";
        }
        LunaMsgBox::ShowW(NULL, message.str().c_str(), L"LunaLua Warnings", MB_OK | MB_ICONWARNING);
    }

    m_warningList.clear();
}

void CLunaLua::setWarning(const std::string& str)
{
    // Only queue warnings if in editor
    if (GM_CUR_SAVE_SLOT == 0)
    {
        m_warningList.push_back(str);
    }
}


void CLunaLua::queuePlayerSectionChangeEvent(int playerIdx) {
    // queue up an onSectionChange event to be called for this player at the end of the current event
    // if we aren't currently accepting additional events being queued (currently in the process of executing them), exit
    if (m_disableSectionChangeEvent) return;
    // check if the specified player is already queued for the event to be executed
    for (size_t i = 0; i < m_playerSectionChangeList.size(); i++) {
        // if we found a match in the list, exit
        if (m_playerSectionChangeList[i] == playerIdx) return;
    }
    // push the player index and set flag to execute event
    m_playerSectionChangeList.push_back(playerIdx);
    m_executeSectionChangeFlag = true;
}
