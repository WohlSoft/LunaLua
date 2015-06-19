#include "RenderOverrides.h"
#include <unordered_map>
#include <unordered_set>
#include "../Defines.h"
#include "../SMBXInternal/Level.h"
#include "RenderUtils.h"

#include <wincodec.h>
#include <wincodecsdk.h>

static HBITMAP loadLevelNpcGfx(const std::wstring& path, unsigned short npcid);
static std::unordered_map<HDC, HBITMAP> gfxOverrideMap;
static std::unordered_set<HDC> gfxOverrideMaskSet;

void loadRenderOverrideGfx()
{
    // Get world and level filenames
    std::wstring worldPath = (std::wstring)GM_FULLDIR;
    std::wstring levelFile = Level::GetName();

    // Generate the custom resource dir
    size_t idx = levelFile.rfind(L'.');
    if (idx == std::wstring::npos) return;
    std::wstring levelFolder = worldPath + levelFile.substr(0, idx) + L'\\';

    // Clear existing npc graphics resources
    for (auto kv : gfxOverrideMap) {
        DeleteObject(kv.second);
    }
    gfxOverrideMap.clear();
    gfxOverrideMaskSet.clear();

    // Load new npc graphics resources
    for (unsigned short i = 1; i < 300; i++) {
        HDC npcHdcPtr = ((HDC*)GM_GFX_NPC_PTR)[i - 1];
        HDC npcHdcMaskPtr = ((HDC*)GM_GFX_NPC_MASK_PTR)[i - 1];
        if (npcHdcPtr == NULL) continue;

        HBITMAP npbBmp = loadLevelNpcGfx(levelFolder, i);
        if (npbBmp != NULL) {
            gfxOverrideMap[npcHdcPtr] = npbBmp;

            if (npcHdcMaskPtr != NULL) {
                gfxOverrideMaskSet.insert(npcHdcMaskPtr);
            }
        }
    }
}

bool renderOverrideBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    auto maskIt = gfxOverrideMaskSet.find(hdcSrc);
    if (maskIt != gfxOverrideMaskSet.end())
    {
        // Don't render mask if we're overriding
        return true;
    }

    auto gfxIt = gfxOverrideMap.find(hdcSrc);
    if (gfxIt != gfxOverrideMap.end()) {
        HDC hdc = CreateCompatibleDC(hdcDest);
        SelectObject(hdc, gfxIt->second);

        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = AC_SRC_ALPHA;

        AlphaBlend(hdcDest, nXDest, nYDest, nWidth, nHeight, hdc, nXSrc, nYSrc, nWidth, nHeight, bf);

        DeleteDC(hdc);

        // Don't run BitBlt if we've run AlphaBlend
        return true;
    }

    // Otherwise, run BitBlt as normal
    return false;
}

static HBITMAP loadLevelNpcGfx(const std::wstring& path, unsigned short npcid)
{
    std::wstring filename = path + L"npc-" + std::to_wstring(npcid) + L".png";
    return LoadGfxAsBitmap(filename);
}
