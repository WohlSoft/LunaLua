#include "RenderOverrideManager.h"
#include "../GlobalFuncs.h"
#include "RenderOps/RenderBitmapOp.h"
#include "../Globals.h"
#include "SMBXMaskedImage.h"
#include "../SMBXInternal/HardcodedGraphicsAccess.h"

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

        HDC nextHdcMaskPtr = nullptr;
        if (graphicsArray_Mask)
            nextHdcMaskPtr = graphicsArray_Mask[i - 1];

        std::wstring nextFilename = path + prefix + L"-" + std::to_wstring(i) + L".png";

        DWORD fAttrib = GetFileAttributesW(nextFilename.c_str());
        if (fAttrib == INVALID_FILE_ATTRIBUTES)
            continue;
        if (fAttrib & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        
        std::shared_ptr<BMPBox> bmp = std::make_shared<BMPBox>(nextFilename, gLunaRender.m_hScreenDC);
        if (!bmp->ImageLoaded())
            continue;

        SMBXMaskedImage* img = SMBXMaskedImage::get(nextHdcMaskPtr, nextHdcPtr);
        if (img == nullptr)
            continue;

        img->SetLoadedPng(bmp);
    }
}

void RenderOverrideManager::loadLevelGFX()
{
    loadOverrides(L"block", GM_GFX_BLOCKS_PTR, 638, GM_GFX_BLOCKS_MASK_PTR);
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



void RenderOverrideManager::loadHardcodedOverrides()
{
    std::wstring baseNameStr = L"hardcoded-";
    for (int i = 1; i <= LunaHardcodedGraphicsPatching::getHardcodedGraphicsItemSize(); i++) {
        HardcodedGraphicsItem& nextItem = LunaHardcodedGraphicsPatching::getHardcodedGraphicsItem(i);
        if(nextItem.state == HardcodedGraphicsItem::HITEMSTATE_INVALID)
            continue;
        if(nextItem.isMask())
            continue;

        HardcodedGraphicsItem* nextItemMask = nullptr;
        if (nextItem.hasMask())
            nextItemMask = &LunaHardcodedGraphicsPatching::getHardcodedGraphicsItem(nextItem.maskIndex);

        if (nextItem.isArray()) {
            for (int j = nextItem.minItem; j <= nextItem.maxItem; j++) 
            {
                std::wstring nextPNGName = baseNameStr + std::to_wstring(i) + L"-" + std::to_wstring(j) + L".png";
                // FIXME: Add loading code for array-based images

                HDC mainHDC = nullptr;
                HDC maskHDC = nullptr;
                LunaHardcodedGraphicsPatching::getHDCFromIndex(i, j, &mainHDC, &maskHDC);

            }
        }
        else 
        {
            // FIXME: Add loading code for images

            HDC mainHDC = nullptr;
            HDC maskHDC = nullptr;
            LunaHardcodedGraphicsPatching::getHDCFromIndex(i, -1, &mainHDC, &maskHDC);
        }

    }
}
