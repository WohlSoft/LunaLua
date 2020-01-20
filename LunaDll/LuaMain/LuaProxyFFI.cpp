
#include <cstdlib>
#include <memory>
#include <vector>
#include <cstring>
#include <Windows.h>
#include <Psapi.h>

#include "../Globals.h"
#include "../Rendering/LunaImage.h"
#include "../Rendering/FrameCapture.h"
#include "../Rendering/GL/GLTextureStore.h"
#include "../SMBXInternal/Blocks.h"
#include "../SMBXInternal/Layer.h"
#include "../SdlMusic/SdlMusPlayer.h"
#include "../FileManager/CustomParamStore.h"
#include "../Misc/TestMode.h"
#include "../Misc/TestModeMenu.h"
#include "LunaLuaMain.h"

#define FFI_EXPORT(sig) __declspec(dllexport) sig __cdecl

// Prototypes from RuntimeHookCharacterId.cpp
short* getValidCharacterIDArray();
PlayerMOB* getTemplateForCharacter(int id);

extern "C" {
    FFI_EXPORT(void*) LunaLuaAlloc(size_t size) {
        CLunaFFILock ffiLock(__FUNCTION__);
        return malloc(size);
    }

    FFI_EXPORT(void) LunaLuaGlDrawTriangles(const float* vert, const float* tex, unsigned int count) {
        CLunaFFILock ffiLock(__FUNCTION__);
        auto obj = std::make_shared<GLEngineCmd_Draw2DArray>();
        obj->mType = GL_TRIANGLES;
        obj->mVert = vert;
        obj->mTex = tex;
        obj->mCount = count;
        Renderer::Get().GLCmd(obj);
    }

    FFI_EXPORT(void) LunaLuaKillPlayer(short playerIndex)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        native_killPlayer(&playerIndex);
    }

    FFI_EXPORT(void) LunaLuaHarmPlayer(short playerIndex)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        native_harmPlayer(&playerIndex);
    }

    FFI_EXPORT(short*) LunaLuaGetValidCharacterIDArray()
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return getValidCharacterIDArray();
    }

    FFI_EXPORT(unsigned int) LunaLuaGetTemplateAddressForCharacter(int id)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return (unsigned int)getTemplateForCharacter(id);
    }

    FFI_EXPORT(int) LunaLuaBlocksTestCollision(unsigned int plAddr, unsigned int blAddr)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return Blocks::TestCollision((PlayerMOB*)plAddr, (Block*)blAddr);
    }

    FFI_EXPORT(void) LunaLuaBlockRemove(unsigned int idx, short playSoundEffect)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        short doPlaySoundAndEffects = COMBOOL(playSoundEffect);
        native_removeBlock(&idx, &doPlaySoundAndEffects);
    }

    FFI_EXPORT(void) LunaLuaBlockHit(unsigned int blockIdx, short fromUpSide, unsigned short playerIdx, int hittingCount)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
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
        CLunaFFILock ffiLock(__FUNCTION__);
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
        CLunaFFILock ffiLock(__FUNCTION__);
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
        CLunaFFILock ffiLock(__FUNCTION__);
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
        CLunaFFILock ffiLock(__FUNCTION__);
        gRenderBGOFlag = val;
    }

    FFI_EXPORT(void) LunaLuaSetSizableRenderFlag(bool val)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        gRenderSizableFlag = val;
    }

    FFI_EXPORT(const char*) LunaLuaGetLevelCustomParams(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return g_LevelCustomParams.getCharPtrArray()[0];
    }

    FFI_EXPORT(const char**) LunaLuaGetSectionCustomParams(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return g_SectionCustomParams.getCharPtrArray();
    }

    FFI_EXPORT(const char**) LunaLuaGetNpcCustomParams(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return g_NpcCustomParams.getCharPtrArray();
    }

    FFI_EXPORT(const char**) LunaLuaGetBgoCustomParams(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return g_BgoCustomParams.getCharPtrArray();
    }

    FFI_EXPORT(const char**) LunaLuaGetBlockCustomParams(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return g_BlockCustomParams.getCharPtrArray();
    }

    FFI_EXPORT(const char*) LunaLuaGetDefaultLevelCustomParams(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        static std::string ret; // Static so it's temporarially stored after returning
        ret = g_configManager.mergeExtraSettings(ConfigPackMiniManager::X_LEVELFILE, 0, "{}");
        return ret.c_str();
    }

    FFI_EXPORT(const char*) LunaLuaGetDefaultSectionCustomParams(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        static std::string ret; // Static so it's temporarially stored after returning
        ret = g_configManager.mergeExtraSettings(ConfigPackMiniManager::X_SECTIONS, 0, "{}");
        return ret.c_str();
    }

    FFI_EXPORT(const char*) LunaLuaGetDefaultNpcCustomParams(unsigned int id)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        static std::string ret; // Static so it's temporarially stored after returning
        ret = g_configManager.mergeExtraSettings(ConfigPackMiniManager::NPC, id, "{}");
        return ret.c_str();
    }

    FFI_EXPORT(const char*) LunaLuaGetDefaultBgoCustomParams(unsigned int id)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        static std::string ret; // Static so it's temporarially stored after returning
        ret = g_configManager.mergeExtraSettings(ConfigPackMiniManager::BGO, id, "{}");
        return ret.c_str();
    }

    FFI_EXPORT(const char*) LunaLuaGetDefaultBlockCustomParams(unsigned int id)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        static std::string ret; // Static so it's temporarially stored after returning
        ret = g_configManager.mergeExtraSettings(ConfigPackMiniManager::BLOCKS, id, "{}");
        return ret.c_str();
    }

    struct FFITestModeSettings {
        int playerCount;
        bool showFPS;
        bool godMode;
        STestModePlayerSettings players[2];
        unsigned int entranceIndex;
    };
    FFITestModeSettings testModeSettings;

    FFI_EXPORT(FFITestModeSettings*) LunaLuaGetTestModeSettings(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        static FFITestModeSettings testModeSettings;
        static STestModeSettings settings;
        settings = getTestModeSettings();
        testModeSettings.playerCount = settings.playerCount;
        testModeSettings.showFPS = settings.showFPS;
        testModeSettings.godMode = settings.godMode;
        testModeSettings.players[0] = settings.players[0];
        testModeSettings.players[1] = settings.players[1];
        testModeSettings.entranceIndex = settings.entranceIndex;

        return &testModeSettings;
    }

    FFI_EXPORT(void) LunaLuaSetTestModeSettings(const FFITestModeSettings* pTestModeSettings)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        static STestModeSettings settings;
        settings = getTestModeSettings();

        settings.playerCount = pTestModeSettings->playerCount;
        settings.showFPS = pTestModeSettings->showFPS;
        settings.godMode = pTestModeSettings->godMode;
        settings.players[0] = pTestModeSettings->players[0];
        settings.players[1] = pTestModeSettings->players[1];
        settings.entranceIndex = pTestModeSettings->entranceIndex;

        setTestModeSettings(settings);
    }

    FFI_EXPORT(void) LunaLuaTestModeExit(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        testModeClosePauseMenu(false, true);
    }

    FFI_EXPORT(void) LunaLuaTestModeRestart(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        testModeClosePauseMenu(true, false);
    }

    FFI_EXPORT(void) LunaLuaTestModeContinue(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        testModeClosePauseMenu(false, false);
    }

    FFI_EXPORT(KeyMap*) LunaLuaGetRawKeymapArray(void)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return gRawKeymap;
    }

    FFI_EXPORT(void) LunaLuaShowLayer(short layerIndex, bool noSmoke)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        short noSmokeNative = COMBOOL(noSmoke);
        native_showLayer(&::Layer::Get(layerIndex)->ptLayerName, &noSmokeNative);
    }

    FFI_EXPORT(void) LunaLuaHideLayer(short layerIndex, bool noSmoke)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        short noSmokeNative = COMBOOL(noSmoke);
        native_hideLayer(&::Layer::Get(layerIndex)->ptLayerName, &noSmokeNative);
    }

    FFI_EXPORT(unsigned char*) LunaLuaGetKeyStateArray()
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        return gKeyState;
    }

    typedef struct
    {
        int len;
        char data[1];
    } GameDataStruct;

    static GameDataStruct* gameData;
    static std::mutex gameDataMutex;
    FFI_EXPORT(void) LunaLuaSetGameData(const char* dataPtr, int dataLen)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        std::unique_lock<std::mutex> lck(gameDataMutex);
        if (gameData != nullptr)
        {
            free(gameData);
            gameData = nullptr;
        }
        gameData = (GameDataStruct*)malloc(dataLen + sizeof(int));
        if (gameData != nullptr)
        {
            gameData->len = dataLen;
            ::memcpy(gameData->data, dataPtr, dataLen);
        }
    }

    FFI_EXPORT(GameDataStruct*) LunaLuaGetGameData()
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        std::unique_lock<std::mutex> lck(gameDataMutex);
        if (gameData == nullptr)
        {
            return nullptr;
        }
        GameDataStruct* cpy = (GameDataStruct*)malloc(gameData->len + sizeof(int));
        cpy->len = gameData->len;
        ::memcpy(cpy->data, gameData->data, gameData->len);
        return cpy;
    }

    FFI_EXPORT(void) LunaLuaFreeReturnedGameData(GameDataStruct* cpy)
    {
        CLunaFFILock ffiLock(__FUNCTION__);
        if (cpy == nullptr)
        {
            return;
        }
        free(cpy);
    }


    FFI_EXPORT(int) LunaLuaGetSelectedControllerPowerLevel(void)
    {
        return (int)gLunaGameControllerManager.getSelectedControllerPowerLevel();
    }
}
