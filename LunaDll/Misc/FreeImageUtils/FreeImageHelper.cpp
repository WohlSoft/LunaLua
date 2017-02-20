#include "FreeImageHelper.h"
#include <Windows.h>

#define FREEIMAGE_LIB
#include <FreeImageLite.h>

HBITMAP FreeImageHelper::CreateEmptyBitmap(int width, int height, int bpp, void** data)
{
    BITMAPINFO convBMI;
    memset(&convBMI, 0, sizeof(BITMAPINFO));
    convBMI.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    convBMI.bmiHeader.biWidth = width;
    convBMI.bmiHeader.biHeight = -(int)height;
    convBMI.bmiHeader.biPlanes = 1;
    convBMI.bmiHeader.biBitCount = bpp;
    convBMI.bmiHeader.biCompression = BI_RGB;
    HDC screenHDC = GetDC(NULL);
    if (screenHDC == NULL)
    {
        return NULL;
    }
    *data = NULL;
    HBITMAP convHBMP = CreateDIBSection(screenHDC, &convBMI, DIB_RGB_COLORS, data, NULL, 0);

    ReleaseDC(NULL, screenHDC);
    screenHDC = NULL;

    return convHBMP;
}


#include <iostream>

bool FreeImageHelper::ToRawBGRA(FIBITMAP* bitmap, void* out)
{
    /*
    std::cout << "Original Image: " << std::hex << std::endl;
    std::cout << "R: " << FreeImage_GetRedMask(bitmap) << std::endl;
    std::cout << "G: " << FreeImage_GetGreenMask(bitmap) << std::endl;
    std::cout << "B: " << FreeImage_GetBlueMask(bitmap) << std::endl;
    std::cout << std::dec;
    std::cout << "BPP: " << FreeImage_GetBPP(bitmap) << std::endl;
    std::cout << "Color type: " << (int)FreeImage_GetColorType(bitmap) << std::endl;
    std::cout << "--------------------------------" << std::endl;
    */

    // Convert to 32bit so we can be sure that it is compatible.
    FIBITMAP* frame32bit = FreeImage_ConvertTo32Bits(bitmap);
    if (frame32bit == nullptr)
    {
        return false;
    }

    // Make it premultiplied so we can use it for opengl
    if (!FreeImage_PreMultiplyWithAlpha(frame32bit)) {
        FreeImage_Unload(frame32bit);
        return false;
    }

    /*
    std::cout << "Converted Image: " << std::hex << std::endl;
    std::cout << "R: " << FreeImage_GetRedMask(frame32bit) << std::endl;
    std::cout << "G: " << FreeImage_GetGreenMask(frame32bit) << std::endl;
    std::cout << "B: " << FreeImage_GetBlueMask(frame32bit) << std::endl;
    std::cout << std::dec;
    std::cout << "BPP: " << FreeImage_GetBPP(frame32bit) << std::endl;
    std::cout << "Color type: " << (int)FreeImage_GetColorType(frame32bit) << std::endl;
    std::cout << "================================" << std::endl;
    */

    // Copy the bits from out FI bitmap to raw data
    FreeImage_ConvertToRawBits((BYTE*)out, frame32bit, FreeImage_GetPitch(frame32bit), 32, 0x0000FF, 0x00FF00, 0xFF0000, true);

    // Free and return
    FreeImage_Unload(frame32bit);

    return true;
}

HBITMAP FreeImageHelper::FromFreeImage(FIBITMAP* bitmap)
{
    int width = FreeImage_GetWidth(bitmap);
    int height = FreeImage_GetHeight(bitmap);

    // Make HBITMAP handle
    BYTE* out; //BGRA
    HBITMAP outBitmap = FreeImageHelper::CreateEmptyBitmap(width, height, 32, (void**)&out);

    // Convert to the needed raw BGRA data
    ToRawBGRA(bitmap, out);

    return outBitmap;
}
