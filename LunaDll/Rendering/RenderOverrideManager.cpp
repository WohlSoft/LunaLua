#include "RenderOverrideManager.h"
#include "../GlobalFuncs.h"
#include "RenderOps/RenderBitmapOp.h"


void RenderOverrideManager::ResetOverrides()
{
    gfxOverrideMap.clear();
    gfxOverrideMaskSet.clear();
}

void RenderOverrideManager::loadOverrides(const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask /*= 0*/)
{
    std::wstring customLevelPath = getCustomFolderPath();
    if (customLevelPath != L"") {
        loadOverrides(customLevelPath, prefix, graphicsArray, numElements, graphicsArray_Mask);
    }
    
    std::wstring episodePath = GM_FULLDIR;
    if (customLevelPath != episodePath){
        loadOverrides(episodePath, prefix, graphicsArray, numElements, graphicsArray_Mask);
    }
}

void RenderOverrideManager::loadOverrides(const std::wstring& path, const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask /*= 0*/)
{
    for (int i = 1; i < numElements + 1; i++){        
        HDC nextHdcPtr = graphicsArray[i - 1];
        
        if (gfxOverrideMap.find(nextHdcPtr) != gfxOverrideMap.end())
            continue;

        HDC nextHdcMaskPtr = 0;
        if (graphicsArray_Mask)
            nextHdcMaskPtr = graphicsArray_Mask[i - 1];

        std::wstring nextFilename = path + prefix + L"-" + std::to_wstring(i) + L".png";

        DWORD fAttrib = GetFileAttributesW(nextFilename.c_str());
        if (fAttrib == INVALID_FILE_ATTRIBUTES)
            continue;
        if (fAttrib & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        

        gfxOverrideMap[nextHdcPtr] = std::make_shared<BMPBox>(nextFilename, gLunaRender.m_hScreenDC);
        if (nextHdcMaskPtr) {
            gfxOverrideMaskSet.insert(nextHdcMaskPtr);
        }

    }
}

bool RenderOverrideManager::renderOverrideBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc)
{
    if (gfxOverrideMaskSet.find(hdcSrc) != gfxOverrideMaskSet.end()) return true; //Skip mask

    auto it = gfxOverrideMap.find(hdcSrc);
    if (it != gfxOverrideMap.end()) {
        RenderBitmapOp overrideFunc;
        overrideFunc.direct_img = it->second.get();
        overrideFunc.x = nXDest;
        overrideFunc.y = nYDest;
        overrideFunc.sx = nXSrc;
        overrideFunc.sy = nYSrc;
        overrideFunc.sw = nWidth;
        overrideFunc.sh = nHeight;
        overrideFunc.Draw(&gLunaRender);
        return true;
    }

    return false;
}

void RenderOverrideManager::loadLevelGFX()
{
    //dbgboxA("Loading overrides!");
    
    /*
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    double pcFreq = double(li.QuadPart);
    QueryPerformanceCounter(&li);
    __int64 startNow = li.QuadPart;
    */

    loadOverrides(L"block", GM_GFX_BLOCKS_PTR, 638, GM_GFX_BLOCKS_MASK_PTR);
    /*
    QueryPerformanceCounter(&li);
    double diffTime = double(li.QuadPart-startNow) / pcFreq;
    dbgboxA((std::string("Duration: ") + std::to_string(diffTime) + std::string(" sec")).c_str());
    */
    loadOverrides(L"background2", GM_GFX_BACKGROUND2_PTR, 58);
    loadOverrides(L"npc", GM_GFX_NPC_PTR, 300, GM_GFX_NPC_MASK_PTR);
    loadOverrides(L"effect", GM_GFX_EFFECTS_PTR, 148, GM_GFX_EFFECTS_MASK_PTR);
    loadOverrides(L"background", GM_GFX_BACKGROUND_PTR, 190, GM_GFX_BACKGROUND_MASK_PTR);
    loadOverrides(L"mario", GM_GFX_MARIO_PTR, 7, GM_GFX_MARIO_MASK_PTR);
    loadOverrides(L"luigi", GM_GFX_LUIGI_PTR, 7, GM_GFX_LUIGI_MASK_PTR);
    loadOverrides(L"peach", GM_GFX_PEACH_PTR, 7, GM_GFX_PEACH_MASK_PTR);
    loadOverrides(L"toad", GM_GFX_TOAD_PTR, 7, GM_GFX_TOAD_MASK_PTR);
    loadOverrides(L"link", GM_GFX_LINK_PTR, 7, GM_GFX_LINK_MASK_PTR);
    loadOverrides(L"yoshib", GM_GFX_YOSHIB_PTR, 8, GM_GFX_YOSHIB_MASK_PTR);
    loadOverrides(L"yoshit", GM_GFX_YOSHIT_PTR, 8, GM_GFX_YOSHIT_MASK_PTR);
}

void RenderOverrideManager::loadWorldGFX()
{
    loadOverrides(L"tile", GM_GFX_TILES_PTR, 328);
    loadOverrides(L"level", GM_GFX_LEVEL_PTR, 32, GM_GFX_LEVEL_MASK_PTR);
    loadOverrides(L"scene", GM_GFX_SCENE_PTR, 65, GM_GFX_SCENE_MASK_PTR);
    loadOverrides(L"path", GM_GFX_PATH_PTR, 32, GM_GFX_PATH_MASK_PTR);
    loadOverrides(L"player", GM_GFX_PLAYER_PTR, 5, GM_GFX_PLAYER_MASK_PTR);
}
