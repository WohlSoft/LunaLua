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
struct SMBXImageCategory;

class RenderOverrideManager
{
public:
    RenderOverrideManager() = default;
    RenderOverrideManager(RenderOverrideManager&) = delete;
    ~RenderOverrideManager() = default;

    void loadOverrides(const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask = 0);
    void loadOverrides(const std::wstring& path, const std::wstring& prefix, HDC* graphicsArray, int numElements, HDC* graphicsArray_Mask = 0);
    void loadLevelGFX();
    void loadWorldGFX();
    void loadHardcodedOverrides();

    void loadDefaultGraphics(const SMBXImageCategory &imageCategory);
    void loadDefaultGraphics(void);
};

#endif