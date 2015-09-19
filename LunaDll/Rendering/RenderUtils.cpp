#include "RenderUtils.h"

#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Misc/FreeImageUtils/FreeImageData.h"
#include "../Misc/FreeImageUtils/FreeImageGifData.h"



HBITMAP LoadGfxAsBitmap(const std::wstring& filename)
{
    FreeImageData bitmapData;
    bitmapData.loadFile(utf8_encode(filename));
    return bitmapData.toHBITMAP();
}

std::tuple<std::vector<HBITMAP>, int> LoadAnimatedGfx(const std::wstring& filename)
{
    std::vector<HBITMAP> allBitmapFrames;
    short sumFrameDelay = 0;

    FreeImageGifData gifData(utf8_encode(filename), false);
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
    screenshotFile.saveFile(utf8_encode(fName));
}
