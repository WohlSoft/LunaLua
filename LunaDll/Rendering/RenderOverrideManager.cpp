#include "RenderOverrideManager.h"
#include "../GlobalFuncs.h"
#include "RenderOps/RenderBitmapOp.h"
#include "../Globals.h"
#include "SMBXMaskedImage.h"
#include "../SMBXInternal/HardcodedGraphicsAccess.h"
#include "RenderUtils.h"

void RenderOverrideManager::loadOverrides(const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask /*= 0*/)
{
    std::wstring customLevelPath = getCustomFolderPath();
    if (customLevelPath != L"") {
        loadOverrides(customLevelPath, prefix, graphicsArray, numElements, graphicsArray_Mask);
    }
    
    std::wstring episodePath = GM_FULLDIR;
    if (customLevelPath != episodePath){        // So we don't have double loading
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

        std::shared_ptr<BMPBox> bmp = std::shared_ptr<BMPBox>(BMPBox::loadIfExist(nextFilename, gLunaRender.GetScreenDC()));
        if(!bmp)
            continue;
        if (!bmp->ImageLoaded())
            continue;

        SMBXMaskedImage* img = SMBXMaskedImage::Get(nextHdcMaskPtr, nextHdcPtr);
        if (img == nullptr)
            continue;

        if (!img->HasLoadedPng())
        {
            img->SetLoadedPng(bmp);
        }
    }
}

void RenderOverrideManager::loadLevelGFX()
{
    loadOverrides(L"block", GM_GFX_BLOCKS_PTR, 700, GM_GFX_BLOCKS_MASK_PTR);
    loadOverrides(L"background2", GM_GFX_BACKGROUND2_PTR, 58);
    loadOverrides(L"npc", GM_GFX_NPC_PTR, 300, GM_GFX_NPC_MASK_PTR);
    loadOverrides(L"effect", GM_GFX_EFFECTS_PTR, 200, GM_GFX_EFFECTS_MASK_PTR);
    loadOverrides(L"background", GM_GFX_BACKGROUND_PTR, 200, GM_GFX_BACKGROUND_MASK_PTR);
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
    std::vector<std::wstring> allPathsToSearch;
    if (!gIsOverworld)
        allPathsToSearch.push_back(getCustomFolderPath()); // Check in the custom folder
    allPathsToSearch.push_back(GM_FULLDIR); // Check the episode dir later if not found in custom folder

    std::wstring baseNameStr = L"hardcoded-";
    for (int i = 1; i <= HardcodedGraphicsItem::Size(); i++) {
        HardcodedGraphicsItem& nextItem = HardcodedGraphicsItem::Get(i);
        if(nextItem.state == HardcodedGraphicsItem::HITEMSTATE_INVALID)
            continue;
        if(nextItem.isMask())
            continue;

        if (nextItem.isArray()) {
            for (int j = nextItem.minItem; j <= nextItem.maxItem; j++) 
            {
                std::wstring nextPNGName = baseNameStr + std::to_wstring(i) + L"-" + std::to_wstring(j) + L".png";
                
                for (const auto& nextFilename : allPathsToSearch) {
                    std::shared_ptr<BMPBox> bmpObj = std::shared_ptr<BMPBox>(BMPBox::loadIfExist(nextFilename + nextPNGName, gLunaRender.GetScreenDC()));
                    if(!bmpObj)
                        continue;
                    if(!bmpObj->ImageLoaded())
                        continue;

                    HDC mainHDC = nullptr;
                    HDC maskHDC = nullptr;
                    nextItem.getHDC(j, &mainHDC, &maskHDC);
                    
                    if (mainHDC != nullptr || maskHDC != nullptr)
                    {
                        SMBXMaskedImage* img = SMBXMaskedImage::Get(maskHDC, mainHDC);

                        if (img != nullptr) {
                            img->SetLoadedPng(bmpObj);
                        }
                    }
                    break;
                }
            }
        }
        else 
        {
            std::wstring nextPNGName = baseNameStr + std::to_wstring(i) + L".png";
            for (const auto& nextFilename : allPathsToSearch) {
                std::shared_ptr<BMPBox> bmpObj = std::shared_ptr<BMPBox>(BMPBox::loadIfExist(nextFilename + nextPNGName, gLunaRender.GetScreenDC()));
                if (!bmpObj)
                    continue;
                if (!bmpObj->ImageLoaded())
                    continue;

                HDC mainHDC = nullptr;
                HDC maskHDC = nullptr;
                nextItem.getHDC(-1, &mainHDC, &maskHDC);

                if (mainHDC != nullptr || maskHDC != nullptr)
                {
                    SMBXMaskedImage* img = SMBXMaskedImage::Get(maskHDC, mainHDC);

                    if (img != nullptr) {
                        img->SetLoadedPng(bmpObj);
                    }
                }
                break;
            }
        }

    }
}


static void dumpHardcodedImages()
{

    std::wstring baseNameStr = L"hardcoded-";
    for (int i = 1; i <= HardcodedGraphicsItem::Size(); i++) {
        HardcodedGraphicsItem& nextItem = HardcodedGraphicsItem::Get(i);
        if (nextItem.state == HardcodedGraphicsItem::HITEMSTATE_INVALID)
            continue;
        if (nextItem.isMask())
            continue;

        HardcodedGraphicsItem* nextItemMask = nextItem.getMaskObj();
        if (nextItem.isArray()) {
            for (int j = nextItem.minItem; j <= nextItem.maxItem; j++)
            {
                std::wstring nextGIFName = baseNameStr + std::to_wstring(i) + L"-" + std::to_wstring(j) + L".png";

                HDC mainHDC = nullptr;
                HDC maskHDC = nullptr;
                nextItem.getHDC(j, &mainHDC, &maskHDC);
                if (mainHDC) SaveMaskedHDCToFile(nextGIFName, mainHDC, maskHDC);
            }
        }
        else
        {
            std::wstring nextGIFName = baseNameStr + std::to_wstring(i) + L".png";

            HDC mainHDC = nullptr;
            HDC maskHDC = nullptr;
            nextItem.getHDC(-1, &mainHDC, &maskHDC);
            if (mainHDC) SaveMaskedHDCToFile(nextGIFName, mainHDC, maskHDC);
        }

    }
}
