#include <thread>
#include <atomic>
#include <string>
#include "../Globals.h"
#include "../Defines.h"
#include "../GlobalFuncs.h"
#include "AsmPatch.h"
#include "../Rendering/GL/GLEngine.h"
#include "../Rendering/GL/GLContextManager.h"
#include "../Rendering/Rendering.h"
#include "../Rendering/WindowSizeHandler.h"
#include <lua.hpp>
#include "LoadScreen.h"
#include "../LuaMain/LunaPathValidator.h"

static void luasetconst(lua_State* L, const char* name, lua_Number number)
{
    lua_pushnumber(L, number);
    lua_setglobal(L, name);
}

#define DEF_CONST(L, C) luasetconst(L, #C, C)

extern void InitMinimalLuaState(lua_State* L);

static lua_State* gameoverGetLuaState()
{
    static std::string mainCode;
    if (mainCode.length() == 0)
    {
        std::wstring lapi = gAppPathWCHAR;
        lapi = lapi.append(L"\\scripts\\base\\engine\\gameover.lua");

        FILE* theFile = _wfopen(lapi.c_str(), L"rb");
        if (!theFile)
        {
            return nullptr;
        }
        fseek(theFile, 0, SEEK_END);
        size_t len = ftell(theFile);
        rewind(theFile);
        if (len > 0)
        {
            mainCode.resize(len);
            fread(&mainCode[0], 1, len, theFile);
        }
        fclose(theFile);
    }

    static lua_State* L = nullptr;
    if (L == nullptr)
    {
        L = luaL_newstate();

        InitMinimalLuaState(L);

        if (luaL_loadbuffer(L, mainCode.c_str(), mainCode.length(), "=gameover.lua"))
        {
            MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua Gameover Screen Syntax Error", MB_OK | MB_ICONWARNING);
            return nullptr;
        }
        if (lua_pcall(L, 0, 0, 0))
        {
            MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua Gameover Screen Critical Error", MB_OK | MB_ICONWARNING);
            return nullptr;
        }
    }
    if (L == nullptr) return nullptr;

    lua_pushboolean(L, false);
    lua_setglobal(L, "_gameoverComplete");

    lua_getglobal(L, "init");
    if (lua_pcall(L, 0, 0, 0))
    {
        MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua Gameover Screen Error", MB_OK | MB_ICONWARNING);
    }

    return L;
}

static void gameoverPlayAudio() {
    std::string path;
    bool found = false;
    
    // check paths for custom gameover sound
    static const char* extensionOptions[] = { "gameover.ogg", "gameover.mp3", "gameover.wav", "gameover.voc", "gameover.flac", "gameover.spc", 0 };
    for (int i = 0; extensionOptions[i] != 0; i++)
    {
        // check for this file
        auto next = std::string(extensionOptions[i]);
        auto result = resolveIfNotAbsolutePath(next);
        if (result.length() != next.length()) // return value will differ if the file exists
        {
            path = result;
            found = true;
            break;
        }
    }
    
    if (!found) { // no custom file exists
        // use basegame path to audio
        path = resolveIfNotAbsolutePath("sound/extended/gameover.ogg");
    }
    
    // load and play the sound
    Mix_Chunk* chunk = PGE_Sounds::SND_OpenSnd(path.c_str());
    if (chunk)
    {
        Mix_PlayChannelVol(-1, chunk, 0, MIX_MAX_VOLUME);
    }
}

void LunaLuaGameoverScreenRun() {
    auto L = gameoverGetLuaState();

    gameoverPlayAudio();

    bool done = false;
    do {
        LunaDllWaitFrame(false);

        Renderer::Get().StartFrameRender();
        Renderer::Get().StartCameraRender(1);

        // Set camera 0 (primary framebuffer)
        std::shared_ptr<GLEngineCmd_SetCamera> cmd = std::make_shared<GLEngineCmd_SetCamera>();
        cmd->mIdx = 0;
        cmd->mX = 0;
        cmd->mY = 0;
        g_GLEngine.QueueCmd(cmd);
        
        if (L != nullptr) {
            lua_getglobal(L, "onDraw");
            if (lua_pcall(L, 0, 0, 0))
            {
                MessageBoxA(NULL, lua_tostring(L, -1), "LunaLua Gameover Screen Error", MB_OK | MB_ICONWARNING);
                done = true;
            }
            lua_getglobal(L, "_gameoverComplete");
            if (lua_toboolean(L, -1)) {
                done = true;
            }
            lua_pop(L, 1);
        } else {
            done = true;
        }

        Renderer::Get().RenderBelowPriority(DBL_MAX);

        g_GLEngine.EndFrame(NULL, true);

        Renderer::Get().EndFrameRender();
    } while (!done);

}