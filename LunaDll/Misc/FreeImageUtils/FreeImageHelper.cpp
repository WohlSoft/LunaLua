#include "FreeImageHelper.h"


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

HBITMAP FreeImageHelper::FromFreeImage(FIBITMAP* bitmap)
{
    int width = FreeImage_GetWidth(bitmap);
    int height = FreeImage_GetHeight(bitmap);
    
    // Convert to 32bit so we can be sure that it is compatible.
    FIBITMAP* frame32bit = FreeImage_ConvertTo32Bits(bitmap);
    // Flip the image to top-down.
    FreeImage_FlipVertical(frame32bit);
    // Make it premultiplied so we can use it for opengl
    FreeImage_PreMultiplyWithAlpha(frame32bit);

    // Make HBITMAP handle
    BYTE* out; //BGRA
    HBITMAP outBitmap = FreeImageHelper::CreateEmptyBitmap(width, height, 32, (void**)&out);

    // Copy the bits from out FI bitmap to HBITMAP
    BYTE* flippedBits = FreeImage_GetBits(frame32bit);
    memcpy(out, flippedBits, width * height * 4);
  
    // Free and return
    FreeImage_Unload(frame32bit);
    return outBitmap;
}
