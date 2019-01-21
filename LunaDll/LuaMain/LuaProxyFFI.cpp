
#include <cstdlib>
#include <memory>
#include <Windows.h>
#include <Psapi.h>

#include "../Globals.h"
#include "../Rendering/LunaImage.h"
#include "../Rendering/FrameCapture.h"
#include "../Rendering/GL/GLTextureStore.h"
#include "../SMBXInternal/Blocks.h"
#include "../SdlMusic/SdlMusPlayer.h"
#include "../FileManager/CustomParamStore.h"
#include "../Misc/TestMode.h"
#include "../Misc/TestModeMenu.h"

#define FFI_EXPORT(sig) __declspec(dllexport) sig __cdecl

// Prototypes from RuntimeHookCharacterId.cpp
short* getValidCharacterIDArray();
PlayerMOB* getTemplateForCharacter(int id);

extern "C" {
    FFI_EXPORT(void*) LunaLuaAlloc(size_t size) {
        return malloc(size);
    }

    FFI_EXPORT(void) LunaLuaGlDrawTriangles(const float* vert, const float* tex, unsigned int count) {
        auto obj = std::make_shared<GLEngineCmd_Draw2DArray>();
        obj->mType = GL_TRIANGLES;
        obj->mVert = vert;
        obj->mTex = tex;
        obj->mCount = count;
        Renderer::Get().GLCmd(obj);
    }

    FFI_EXPORT(void) LunaLuaKillPlayer(short playerIndex)
    {
        native_killPlayer(&playerIndex);
    }

    FFI_EXPORT(void) LunaLuaHarmPlayer(short playerIndex)
    {
        native_harmPlayer(&playerIndex);
    }

    FFI_EXPORT(short*) LunaLuaGetValidCharacterIDArray()
    {
        return getValidCharacterIDArray();
    }

    FFI_EXPORT(unsigned int) LunaLuaGetTemplateAddressForCharacter(int id)
    {
        return (unsigned int)getTemplateForCharacter(id);
    }

    FFI_EXPORT(int) LunaLuaBlocksTestCollision(unsigned int plAddr, unsigned int blAddr)
    {
        return Blocks::TestCollision((PlayerMOB*)plAddr, (Block*)blAddr);
    }

    FFI_EXPORT(void) LunaLuaBlockRemove(unsigned int idx, short playSoundEffect)
    {
        short doPlaySoundAndEffects = COMBOOL(playSoundEffect);
        native_removeBlock(&idx, &doPlaySoundAndEffects);
    }

    FFI_EXPORT(void) LunaLuaBlockHit(unsigned int blockIdx, short fromUpSide, unsigned short playerIdx, int hittingCount)
    {
        short unkFlag1VB = COMBOOL(fromUpSide);
        native_hitBlock(&blockIdx, &unkFlag1VB, &playerIdx);
        if (hittingCount != -1) {
            Blocks::Get(blockIdx)->RepeatingHits = hittingCount;
        }
    }

    #pragma comment(lib, "psapi.lib")
    struct LunaLuaMemUsageData
    {
        uint32_t totalWorking;
        uint32_t imgRawMem;
        uint32_t imgCompMem;
        uint32_t sndMem;
        double   imgGpuMem;
    };
    FFI_EXPORT(const LunaLuaMemUsageData*) LunaLuaGetMemUsage()
    {
        static thread_local LunaLuaMemUsageData out;
        static thread_local PROCESS_MEMORY_COUNTERS psmemCounters;

        if (GetProcessMemoryInfo(GetCurrentProcess(), &psmemCounters, sizeof(psmemCounters)))
        {
            out.totalWorking = (psmemCounters.WorkingSetSize + 512) / 1024;
        }
        else
        {
            out.totalWorking = 0;
        }

        out.imgRawMem = (LunaImage::totalRawMem + 512) / 1024;
        out.imgCompMem = (LunaImage::totalCompMem + 512) / 1024;
        out.sndMem = (PGE_Sounds::GetMemUsage() + 512) / 1024;
        out.imgGpuMem = ((double)GLTextureStore::GetTextureMemory()) / 1024.0;

        return &out;
    }


    FFI_EXPORT(const char*) LunaLuaMemReadString(unsigned int addr)
    {
        static thread_local std::string tmp;
        void* ptr = ((&(*(byte*)addr)));

        if (ptr == 0)
        {
            return nullptr;
        }

        tmp = *((VB6StrPtr*)ptr);
        return tmp.c_str();
    }

    FFI_EXPORT(void) LunaLuaMemWriteString(unsigned int addr, const char* str)
    {
        static thread_local VB6StrPtr nullStr;
        void* ptr = ((&(*(byte*)addr)));

        if (str != nullptr)
        {
            *((VB6StrPtr*)ptr) = std::string(str);
        }
        else
        {
            *((VB6StrPtr*)ptr) = nullStr;
        }
    }

    FFI_EXPORT(void) LunaLuaSetBGORenderFlag(bool val)
    {
        gRenderBGOFlag = val;
    }

    FFI_EXPORT(const char**) LunaLuaGetNpcCustomParams(void)
    {
        return g_NpcCustomParams.getCharPtrArray();
    }

    FFI_EXPORT(const char**) LunaLuaGetBlockCustomParams(void)
    {
        return g_BlockCustomParams.getCharPtrArray();
    }

    struct FFITestModeSettings {
        int playerCount;
        bool showFPS;
        bool godMode;
        STestModePlayerSettings players[2];
    };
    FFITestModeSettings testModeSettings;

    FFI_EXPORT(FFITestModeSettings*) LunaLuaGetTestModeSettings(void)
    {
        static FFITestModeSettings testModeSettings;
        static STestModeSettings settings;
        settings = getTestModeSettings();
        testModeSettings.playerCount = settings.playerCount;
        testModeSettings.showFPS = settings.showFPS;
        testModeSettings.godMode = settings.godMode;
        testModeSettings.players[0] = settings.players[0];
        testModeSettings.players[1] = settings.players[1];

        return &testModeSettings;
    }

    FFI_EXPORT(void) LunaLuaSetTestModeSettings(const FFITestModeSettings* pTestModeSettings)
    {
        static STestModeSettings settings;
        settings = getTestModeSettings();

        settings.playerCount = pTestModeSettings->playerCount;
        settings.showFPS = pTestModeSettings->showFPS;
        settings.godMode = pTestModeSettings->godMode;
        settings.players[0] = pTestModeSettings->players[0];
        settings.players[1] = pTestModeSettings->players[1];

        setTestModeSettings(settings);
    }

    FFI_EXPORT(void) LunaLuaTestModeExit(void)
    {
        testModeClosePauseMenu(false, true);
    }

    FFI_EXPORT(void) LunaLuaTestModeRestart(void)
    {
        testModeClosePauseMenu(true, false);
    }

    FFI_EXPORT(void) LunaLuaTestModeContinue(void)
    {
        testModeClosePauseMenu(false, false);
    }

    FFI_EXPORT(KeyMap*) LunaLuaGetRawKeymapArray(void)
    {
        return gRawKeymap;
    }
}
