#ifndef RenderOverrideManager_hhhhh
#define RenderOverrideManager_hhhhh

#include <Windows.h>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory.h>
class BMPBox;
class SMBXMaskedImage;

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
    bool renderOverrideBitBlt(SMBXMaskedImage* img, int x, int y, int sw, int sh, int sx, int sy, bool maskOnly=false);
    
private:
    std::unordered_map<SMBXMaskedImage*, std::shared_ptr<BMPBox>> gfxOverrideMap;
};

#endif