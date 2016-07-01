#include "../Defines.h"
#include "../Globals.h"
#include "../Rendering/GL/GLEngineProxy.h"
#include "../Rendering/RenderOps/RenderSpriteOp.h"
#include "../Rendering/Rendering.h"
#include "../SMBXInternal/HardcodedGraphicsAccess.h"
#include "SMBXMaskedImage.h"
#include "RenderOps/RenderBitmapOp.h"

// Lookup table instance
std::unordered_map<HDC, std::shared_ptr<SMBXMaskedImage>> SMBXMaskedImage::lookupTable;

// Custom overridables instance
std::unordered_map<std::string, std::shared_ptr<SMBXMaskedImage>> SMBXMaskedImage::customOverridable;

SMBXMaskedImage* SMBXMaskedImage::Get(HDC maskHdc, HDC mainHdc)
{
    if (maskHdc == nullptr && mainHdc == nullptr) {
        return nullptr;
    }

    if (mainHdc != nullptr)
    {
        auto it = lookupTable.find(mainHdc);
        if (it != lookupTable.end())
        {
            if (maskHdc != nullptr && it->second->maskHdc == nullptr)
            {
                it->second->maskHdc = maskHdc;
                lookupTable[maskHdc] = it->second;
            }
            return it->second.get();
        }
    }

    if (maskHdc != nullptr)
    {
        auto it = lookupTable.find(maskHdc);
        if (it != lookupTable.end())
        {
            if (mainHdc != nullptr && it->second->mainHdc == nullptr)
            {
                it->second->mainHdc = mainHdc;
                lookupTable[mainHdc] = it->second;
            }
            return it->second.get();
        }
    }

    std::shared_ptr<SMBXMaskedImage> img = std::make_shared<SMBXMaskedImage>();
    if (mainHdc != nullptr)
    {
        img->mainHdc = mainHdc;
        lookupTable[mainHdc] = img;
    }
    if (maskHdc != nullptr)
    {
        img->maskHdc = maskHdc;
        lookupTable[maskHdc] = img;
    }
    return img.get();
}

void SMBXMaskedImage::ClearLookupTable(void)
{
    lookupTable.clear();
}

SMBXMaskedImage::SMBXMaskedImage() :
    maskHdc(nullptr), mainHdc(nullptr),
    rgbaOverrideImage(nullptr),
    loadedPngImage(nullptr),
    maskedOverrideImage(nullptr)
{
}

void SMBXMaskedImage::Draw(int dx, int dy, int w, int h, int sx, int sy, bool drawMask, bool drawMain)
{
    if (maskHdc == nullptr && mainHdc == nullptr) return;
    if (drawMask == false && drawMain == false) return;
    
    if (drawMask && maskHdc == nullptr) {
        // If there's no mask hdc but we're supposed to draw a mask, assume a fully black mask
        if (drawMain && mainHdc != nullptr) {
            // Normal opaque rendering
            if (g_GLEngine.IsEnabled())
            {
                g_GLEngine.EmulatedBitBlt(dx, dy, w, h, mainHdc, sx, sy, SRCCOPY);
            }
            else
            {
                BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, mainHdc, sx, sy, SRCCOPY);
            }
        }
        else
        {
            // Mask-only rendering, when there's no mask? Best we can do is render a black rectangle
            if (g_GLEngine.IsEnabled())
            {
                g_GLEngine.EmulatedBitBlt(dx, dy, w, h, nullptr, 0, 0, 0x10);
            }
            else
            {
                BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, nullptr, 0, 0, 0x10);
            }
        }
    }
    else {
        // Normal mask rendering
        if (g_GLEngine.IsEnabled())
        {
            // TODO: Implement GLEngine masked image rendering as a single call of some sort
            if (drawMask)
            {
                g_GLEngine.EmulatedBitBlt(dx, dy, w, h, maskHdc, sx, sy, SRCAND);
            }
            if (mainHdc != nullptr && drawMain)
            {
                g_GLEngine.EmulatedBitBlt(dx, dy, w, h, mainHdc, sx, sy, SRCPAINT);
            }
        }
        else
        {
            if (drawMask)
            {
                BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, maskHdc, sx, sy, SRCAND);
            }
            if (mainHdc != nullptr && drawMain)
            {
                BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, mainHdc, sx, sy, SRCPAINT);
            }
        }
    }
}

void SMBXMaskedImage::DrawWithOverride(int dx, int dy, int w, int h, int sx, int sy, bool drawMask, bool drawMain)
{
    if (maskedOverrideImage != nullptr)
    {
        maskedOverrideImage->Draw(dx, dy, w, h, sx, sy, drawMask, drawMain);
    }
    else if (rgbaOverrideImage || loadedPngImage)
    {
        // TODO: Handle drawMask/drawMain parameters (fixes shadowstar for PNGs)
        RenderBitmapOp overrideFunc;
        overrideFunc.direct_img = rgbaOverrideImage ? rgbaOverrideImage : loadedPngImage;
        overrideFunc.x = dx;
        overrideFunc.y = dy;
        overrideFunc.sx = sx;
        overrideFunc.sy = sy;
        overrideFunc.sw = w;
        overrideFunc.sh = h;
        overrideFunc.Draw(&gLunaRender);
    }
    else
    {
        Draw(dx, dy, w, h, sx, sy, drawMask, drawMain);
    }
}

void SMBXMaskedImage::SetLoadedPng(const std::shared_ptr<BMPBox>& img)
{
    loadedPngImage = img;
}

void SMBXMaskedImage::SetOverride(const std::shared_ptr<BMPBox>& img)
{
    maskedOverrideImage = nullptr;
    if (loadedPngImage.get() != img.get())
    {
        rgbaOverrideImage = img;
    }
    else
    {
        rgbaOverrideImage = nullptr;
    }
}

void SMBXMaskedImage::SetOverride(SMBXMaskedImage* img)
{
    rgbaOverrideImage = nullptr;
    if (loadedPngImage || (img != this))
    {
        maskedOverrideImage = img;
    }
    else
    {
        maskedOverrideImage = nullptr;
    }
}

void SMBXMaskedImage::UnsetOverride()
{
    rgbaOverrideImage = nullptr;
    maskedOverrideImage = nullptr;
}

void SMBXMaskedImage::getSize(int& w, int& h, bool followOverride)
{
    if ((maskedOverrideImage != nullptr) && followOverride)
    {
        maskedOverrideImage->getSize(w, h, false);
    }
    else if (rgbaOverrideImage || loadedPngImage)
    {
        std::shared_ptr<BMPBox> img = rgbaOverrideImage ? rgbaOverrideImage : loadedPngImage;
        if (img && img->ImageLoaded())
        {
            w = img->m_W;
            h = img->m_H;
        }
        else
        {
            w = 0;
            h = 0;
        }
    }
    else
    {
        HDC sizeHdc = (mainHdc != mainHdc) ? mainHdc : maskHdc;
        h = 0;
        w = 0;
        if (sizeHdc == nullptr) return;
        
        BITMAP bmp;
        HBITMAP hbmp;

        // Get handle to bitmap
        hbmp = (HBITMAP)GetCurrentObject(sizeHdc, OBJ_BITMAP);
        if (hbmp == nullptr) return;

        // Get bitmap structure to check the height/width
        GetObject(hbmp, sizeof(BITMAP), &bmp);
        w = bmp.bmWidth;
        h = bmp.bmHeight;
    }
}

void SMBXMaskedImage::QueueDraw(double x, double y, double w, double h, double sx, double sy, bool sceneCoords, double priority)
{
    RenderSpriteOp* maskedRenderOp = new RenderSpriteOp();
    maskedRenderOp->sprite = this;
    maskedRenderOp->sx = sx;
    maskedRenderOp->sy = sy;
    maskedRenderOp->sw = w;
    maskedRenderOp->sh = h;
    maskedRenderOp->x = x;
    maskedRenderOp->y = y;
    maskedRenderOp->sceneCoords = sceneCoords;
    maskedRenderOp->m_renderPriority = priority;
    gLunaRender.AddOp(maskedRenderOp);
}

SMBXMaskedImage* SMBXMaskedImage::GetCharacterSprite(short charId, short powerup)
{
    HDC* mainArray = nullptr;
    HDC* maskArray = nullptr;
    HDC mainHdc = nullptr;
    HDC maskHdc = nullptr;

    // Sanity check
    if (powerup < 1 || powerup > 7) return nullptr;

    switch (charId)
    {
    case 1:
        mainArray = GM_GFX_MARIO_PTR;
        maskArray = GM_GFX_MARIO_MASK_PTR;
        break;
    case 2:
        mainArray = GM_GFX_LUIGI_PTR;
        maskArray = GM_GFX_LUIGI_MASK_PTR;
        break;
    case 3:
        mainArray = GM_GFX_PEACH_PTR;
        maskArray = GM_GFX_PEACH_MASK_PTR;
        break;
    case 4:
        mainArray = GM_GFX_TOAD_PTR;
        maskArray = GM_GFX_TOAD_MASK_PTR;
        break;
    case 5:
        mainArray = GM_GFX_LINK_PTR;
        maskArray = GM_GFX_LINK_MASK_PTR;
        break;
    default:
        return nullptr;
    }

    mainHdc = (mainArray != nullptr) ? mainArray[powerup - 1] : nullptr;
    maskHdc = (maskArray != nullptr) ? maskArray[powerup - 1] : nullptr;
    if (mainHdc == nullptr && maskHdc == nullptr) return nullptr;

    return SMBXMaskedImage::Get(maskHdc, mainHdc);
}

SMBXMaskedImage* SMBXMaskedImage::GetByName(const std::string& t, int index)
{
    HDC* mainArray = nullptr;
    HDC* maskArray = nullptr;
    HDC mainHdc = nullptr;
    HDC maskHdc = nullptr;
    int maxIndex = 0;

    if (t == "block")
    {
        mainArray = GM_GFX_BLOCKS_PTR;
        maskArray = GM_GFX_BLOCKS_MASK_PTR;
        maxIndex = 700;
    }
    else if (t == "background2")
    {
        mainArray = GM_GFX_BACKGROUND2_PTR;
        maxIndex = 58;
    }
    else if (t == "npc")
    {
        mainArray = GM_GFX_NPC_PTR;
        maskArray = GM_GFX_NPC_MASK_PTR;
        maxIndex = 300;
    }
    else if (t == "effect")
    {
        mainArray = GM_GFX_EFFECTS_PTR;
        maskArray = GM_GFX_EFFECTS_MASK_PTR;
        maxIndex = 200;
    }
    else if (t == "background")
    {
        mainArray = GM_GFX_BACKGROUND_PTR;
        maskArray = GM_GFX_BACKGROUND_MASK_PTR;
        maxIndex = 200;
    }
    else if (t == "mario")
    {
        mainArray = GM_GFX_MARIO_PTR;
        maskArray = GM_GFX_MARIO_MASK_PTR;
        maxIndex = 7;
    }
    else if (t == "luigi")
    {
        mainArray = GM_GFX_LUIGI_PTR;
        maskArray = GM_GFX_LUIGI_MASK_PTR;
        maxIndex = 7;
    }
    else if (t == "peach")
    {
        mainArray = GM_GFX_PEACH_PTR;
        maskArray = GM_GFX_PEACH_MASK_PTR;
        maxIndex = 7;
    }
    else if (t == "toad")
    {
        mainArray = GM_GFX_TOAD_PTR;
        maskArray = GM_GFX_TOAD_MASK_PTR;
        maxIndex = 7;
    }
    else if (t == "link")
    {
        mainArray = GM_GFX_LINK_PTR;
        maskArray = GM_GFX_LINK_MASK_PTR;
        maxIndex = 7;
    }
    else if (t == "yoshib")
    {
        mainArray = GM_GFX_YOSHIB_PTR;
        maskArray = GM_GFX_YOSHIB_MASK_PTR;
        maxIndex = 8;
    }
    else if (t == "yoshit")
    {
        mainArray = GM_GFX_YOSHIT_PTR;
        maskArray = GM_GFX_YOSHIT_MASK_PTR;
        maxIndex = 8;
    }
    else if (t == "tile")
    {
        mainArray = GM_GFX_TILES_PTR;
        maskArray = nullptr;
        maxIndex = 328;
    }
    else if (t == "level")
    {
        mainArray = GM_GFX_LEVEL_PTR;
        maskArray = GM_GFX_LEVEL_MASK_PTR;
        maxIndex = 32;
    }
    else if (t == "scene")
    {
        mainArray = GM_GFX_SCENE_PTR;
        maskArray = GM_GFX_SCENE_MASK_PTR;
        maxIndex = 65;
    }
    else if (t == "path")
    {
        mainArray = GM_GFX_PATH_PTR;
        maskArray = GM_GFX_PATH_MASK_PTR;
        maxIndex = 32;
    }
    else if (t == "player")
    {
        mainArray = GM_GFX_PLAYER_PTR;
        maskArray = GM_GFX_PLAYER_MASK_PTR;
        maxIndex = 5;
    }

    // Check range on index and get HDCs
    if (index >= 1 && index <= maxIndex)
    {
        mainHdc = (mainArray != nullptr) ? mainArray[index - 1] : nullptr;
        maskHdc = (maskArray != nullptr) ? maskArray[index - 1] : nullptr;
    }

    // If we have no HDC abort, or check for a custom one...
    if (mainHdc == nullptr && maskHdc == nullptr)
    {
        std::string keyStr = t + "-" + std::to_string(index);
        {
            auto it = customOverridable.find(keyStr);
            if (it != customOverridable.end())
            {
                return it->second.get();
            }
        }

        return nullptr;
    }

    // Get the image
    return SMBXMaskedImage::Get(maskHdc, mainHdc);
}

SMBXMaskedImage* SMBXMaskedImage::GetHardcoded(const std::string& name)
{
    HDC mainHdc = nullptr;
    HDC maskHdc = nullptr;
    HardcodedGraphicsItem::GetHDCByName(name, &mainHdc, &maskHdc);
    return SMBXMaskedImage::Get(maskHdc, mainHdc);
}

SMBXMaskedImage* SMBXMaskedImage::RegisterCustomOverridable(const std::string& t, int index, const std::shared_ptr<BMPBox>& img)
{
    std::string keyStr = t + "-" + std::to_string(index);
    {
        auto it = customOverridable.find(keyStr);
        if (it != customOverridable.end())
        {
            if (img) it->second->SetLoadedPng(img);
            return it->second.get();
        }
    }

    std::shared_ptr<SMBXMaskedImage> obj = std::make_shared<SMBXMaskedImage>();
    if (img) obj->SetLoadedPng(img);
    customOverridable[keyStr] = obj;
    return obj.get();
}

void SMBXMaskedImage::UnregisterCustomOverridable(const std::string& t, int index)
{
    std::string keyStr = t + "-" + std::to_string(index);
    customOverridable.erase(keyStr);
}
