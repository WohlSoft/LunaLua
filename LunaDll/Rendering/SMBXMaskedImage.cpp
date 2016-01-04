#include "../Defines.h"
#include "../Globals.h"
#include "../Rendering/GL/GLEngineProxy.h"
#include "../Misc/HardcodedGraphicsAccess.h"
#include "SMBXMaskedImage.h"

// Lookup table instance
std::unordered_map<HDC, std::shared_ptr<SMBXMaskedImage>> SMBXMaskedImage::lookupTable;

std::shared_ptr<SMBXMaskedImage> SMBXMaskedImage::get(HDC maskHdc, HDC mainHdc)
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
        return it->second;
    }

    it = lookupTable.find(mainHdc);
    if (it != lookupTable.end())
    {
        if (maskHdc != nullptr && it->second->maskHdc == nullptr)
        {
            it->second->maskHdc = maskHdc;
            lookupTable[maskHdc] = it->second;
        }
        return it->second;
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
    return img;
}

void SMBXMaskedImage::clearLookupTable(void)
{
    lookupTable.clear();

    // Special case for hearts, since they share a mask, and work in batches
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