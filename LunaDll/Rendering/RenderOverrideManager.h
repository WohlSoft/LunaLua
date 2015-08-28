#ifndef RenderOverrideManager_hhhhh
#define RenderOverrideManager_hhhhh

#include "../Globals.h"
#include <Windows.h>
#include <unordered_map>
#include <unordered_set>

class RenderOverrideManager
{
public:
    RenderOverrideManager() = default;
    RenderOverrideManager(RenderOverrideManager&) = delete;
    ~RenderOverrideManager() = default;

    void ResetOverrides();


private:
    std::unordered_map<HDC, BMPBox*> gfxOverrideMap;
    std::unordered_set<HDC> gfxOverrideMaskSet;
};

#endif