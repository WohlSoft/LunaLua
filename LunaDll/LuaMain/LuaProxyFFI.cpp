
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
        gLunaRender.GLCmd(obj);
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
		static LunaLuaMemUsageData out;
		static PROCESS_MEMORY_COUNTERS psmemCounters;

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
}
