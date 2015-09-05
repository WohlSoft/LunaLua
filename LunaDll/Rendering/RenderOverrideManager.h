#ifndef RenderOverrideManager_hhhhh
#define RenderOverrideManager_hhhhh

#include "../Globals.h"
#include <Windows.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory.h>

class RenderOverrideManager
{
public:
    RenderOverrideManager() = default;
    RenderOverrideManager(RenderOverrideManager&) = delete;
    ~RenderOverrideManager() = default;

    void ResetOverrides();
    void loadOverrides(const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask = 0);
    void loadOverrides(const std::wstring& path, const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask = 0);
    void loadLevelGFX();
    void loadWorldGFX();
    bool renderOverrideBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc);
    
private:
    std::unordered_map<HDC, std::shared_ptr<BMPBox>> gfxOverrideMap;
    std::unordered_set<HDC> gfxOverrideMaskSet;
};

#endif