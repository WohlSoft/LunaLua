#include "../Defines.h"
#include "../Globals.h"
#include "../Rendering/GL/GLEngineProxy.h"
#include "../Misc/HardcodedGraphicsAccess.h"
#include "SMBXMaskedImage.h"

// Lookup table instance
std::unordered_map<HDC, std::shared_ptr<SMBXMaskedImage>> SMBXMaskedImage::lookupTable;

SMBXMaskedImage* SMBXMaskedImage::get(HDC maskHdc, HDC mainHdc)
{
    if (maskHdc == nullptr && mainHdc == nullptr) {
        return nullptr;
    }

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

    it = lookupTable.find(mainHdc);
    if (it != lookupTable.end())
    {
        if (maskHdc != nullptr && it->second->maskHdc == nullptr)
        {
            it->second->maskHdc = maskHdc;
            lookupTable[maskHdc] = it->second;
        }
        return it->second.get();
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

    // So... for almost all images, the BitBltEmulation class correctly infers
    // the pairing between the mask painting and the main image painting. This
    // however fails in the case of the heart display on the HUD.
    // This failure is because instead of rendering SRCAND/SRCPAINT in
    // alternation for each heart, all three hearts have their masks rendered
    // before rendering the actual image.
    //
    // I'm rather sure this quirk is unique to HUD hearts, and doesn't apply
    // to anything else.
    
    // TODO: Consider moving this special casing to BitBltEmulation, instead of
    //       preloading the SMBXMaskedImage lookup table?
    //       A caveat about that, is even if we move it there, we still need to
    //       ensure we correctly handle that both filled and non filled share
    //       a mask HDC. We'll probably be safe if we switch to using mainHdc
    //       instead of maskHdc as the highest priority lookup in the
    //       SMBXMaskedImage::get(maskHdc, mainHdc) implementation.

    // Get the HDCs associated with heart graphics
    HDC heartMask = (HDC)getHDCForHardcodedGraphic(0x384, 0x1);
    HDC filledHeart = (HDC)getHDCForHardcodedGraphic(0x388, 0x1);
    HDC emptyHeart = (HDC)getHDCForHardcodedGraphic(0x388, 0x2);

    // Dummy to force ignoring the mask render
    std::shared_ptr<SMBXMaskedImage> heartMaskDummy = std::make_shared<SMBXMaskedImage>();
    lookupTable[heartMask] = heartMaskDummy;

    // Filled heart image
    std::shared_ptr<SMBXMaskedImage> filledHeartImg = std::make_shared<SMBXMaskedImage>();
    filledHeartImg->maskHdc = heartMask;
    filledHeartImg->mainHdc = filledHeart;
    lookupTable[filledHeart] = filledHeartImg;

    // Empty heart image
    std::shared_ptr<SMBXMaskedImage> emptyHeartImg = std::make_shared<SMBXMaskedImage>();
    emptyHeartImg->maskHdc = heartMask;
    emptyHeartImg->mainHdc = emptyHeart;
    lookupTable[emptyHeart] = emptyHeartImg;
}

SMBXMaskedImage::SMBXMaskedImage() :
    maskHdc(nullptr), mainHdc(nullptr)
{
}

void SMBXMaskedImage::Draw(int dx, int dy, int w, int h, int sx, int sy, bool maskOnly)
{
    if (maskHdc == nullptr && mainHdc == nullptr) return;
    
    if (maskHdc == nullptr && mainHdc != nullptr) {
        // If there's no mask, assume opaque rendering
        if (!maskOnly) {
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
            g_GLEngine.EmulatedBitBlt(dx, dy, w, h, maskHdc, sx, sy, SRCAND);
            if (mainHdc != nullptr && !maskOnly)
            {
                g_GLEngine.EmulatedBitBlt(dx, dy, w, h, mainHdc, sx, sy, SRCPAINT);
            }
        }
        else
        {
            BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, maskHdc, sx, sy, SRCAND);
            if (mainHdc != nullptr && !maskOnly)
            {
                BitBlt((HDC)GM_SCRN_HDC, dx, dy, w, h, mainHdc, sx, sy, SRCPAINT);
            }
        }
    }
}