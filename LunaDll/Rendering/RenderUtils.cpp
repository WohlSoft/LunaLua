#include "RenderUtils.h"

#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Misc/FreeImageUtils/FreeImageData.h"
#include "../Misc/FreeImageUtils/FreeImageGifData.h"



HBITMAP LoadGfxAsBitmap(const std::wstring& filename)
{
    FreeImageData bitmapData;
    bitmapData.loadFile(WStr2Str(filename));
    return bitmapData.toHBITMAP();
}

std::tuple<std::vector<HBITMAP>, int> LoadAnimatedGfx(const std::wstring& filename)
{
    std::vector<HBITMAP> allBitmapFrames;
    short sumFrameDelay = 0;

    FreeImageGifData gifData(WStr2Str(filename), false);
    if (!gifData.isOpen())
        return make_tuple(allBitmapFrames, 9);
    
    for (int i = 0; i < gifData.count(); i++) {
        HBITMAP nextBitmap = gifData.getFrame(i);
        int nextDelay = gifData.getDelayValue(i);
        if (nextDelay == -1)
            nextDelay = 9;
        allBitmapFrames.push_back(nextBitmap);
        sumFrameDelay += (short)nextDelay;
    }

    int frameTime = sumFrameDelay / gifData.count();
    return std::make_tuple(allBitmapFrames, frameTime);
}

void GenerateScreenshot(const std::wstring& fName, const BITMAPINFOHEADER& header, void* pData)
{
    FreeImageData screenshotFile;
    screenshotFile.init(header.biWidth, header.biHeight, (BYTE*)pData);
    screenshotFile.saveFile(WStr2Str(fName));
}

HBITMAP CopyBitmapFromHdc(HDC hdc)
{
    int w, h;
    {
        BITMAP bmp;
        HBITMAP hbmp;

        // Get handle to bitmap
        hbmp = (HBITMAP)GetCurrentObject(hdc, OBJ_BITMAP);
        if (hbmp == nullptr) return nullptr;

        // Get bitmap structure to check the height/width
        GetObject(hbmp, sizeof(BITMAP), &bmp);
        w = bmp.bmWidth;
        h = bmp.bmHeight;
    }

    // Convert to 24bpp BGR in memory that's accessible
    void* pData = nullptr;
    HBITMAP convHBMP = FreeImageHelper::CreateEmptyBitmap(w, h, 32, &pData);
    HDC screenHDC = GetDC(nullptr);
    if (screenHDC == nullptr) {
        return nullptr;
    }
    HDC convHDC = CreateCompatibleDC(screenHDC);
    SelectObject(convHDC, convHBMP);
    BitBlt(convHDC, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
    DeleteDC(convHDC);
    convHDC = nullptr;
    ReleaseDC(nullptr, screenHDC);
    screenHDC = nullptr;

    // Set alpha channel to 0xFF, because it's always supposed to be and won't
    // be copied as such by BitBlt
    uint32_t pixelCount = w * h;
    for (uint32_t idx = 0; idx < pixelCount; idx++) {
        ((uint8_t*)pData)[idx * 4 + 3] = 0xFF;
    }

    return convHBMP;
}