#include "RenderOverrideManager.h"
#include "../GlobalFuncs.h"

void RenderOverrideManager::ResetOverrides()
{
    gfxOverrideMap.clear();
    gfxOverrideMaskSet.clear();
}

void RenderOverrideManager::loadOverrides(const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask /*= 0*/)
{
    std::wstring customLevelPath = getCustomFolderPath();
    for (int i = 1; i < numElements; i++){
        HDC nextHdcPtr = graphicsArray[i - 1];
        HDC nextHdcMaskPtr = 0;
        if (graphicsArray_Mask)
            nextHdcMaskPtr = graphicsArray_Mask[i - 1];
        
        std::wstring nextFilename = customLevelPath + prefix + L"-" + std::to_wstring(i) + L".png";
        
        DWORD fAttrib = GetFileAttributesW(nextFilename.c_str());
        if (fAttrib == INVALID_FILE_ATTRIBUTES)
            continue;
        if (fAttrib & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        int nextCode = gLunaRender.GetAutoImageResourceCode();
        if (!gLunaRender.LoadBitmapResource(nextFilename, nextCode))
            continue;

        auto possibleImg = gLunaRender.LoadedImages.find(nextCode);
        if (possibleImg == gLunaRender.LoadedImages.end())
            continue;

        gfxOverrideMap[nextHdcPtr] = possibleImg->second;
        if (nextHdcMaskPtr) {
            gfxOverrideMaskSet.insert(nextHdcMaskPtr);
        }

    }
}
