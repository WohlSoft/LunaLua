#include "../Defines.h"
#include "../Globals.h"
#include "../Rendering/GL/GLEngineProxy.h"
#include "../SMBXInternal/HardcodedGraphicsAccess.h"
#include "SMBXMaskedImage.h"
#include "RenderOps/RenderBitmapOp.h"

// Lookup table instance
std::unordered_map<HDC, std::shared_ptr<SMBXMaskedImage>> SMBXMaskedImage::lookupTable;

SMBXMaskedImage* SMBXMaskedImage::get(HDC maskHdc, HDC mainHdc)
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

void SMBXMaskedImage::clearLookupTable(void)
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
		overrideFunc.dw = w;
		overrideFunc.dh = h;
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
