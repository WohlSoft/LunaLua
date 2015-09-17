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

    FIBITMAP* frame32bit = FreeImage_ConvertTo32Bits(bitmap);

    BYTE* out; //BGRA
    HBITMAP outBitmap = FreeImageHelper::CreateEmptyBitmap(width, height, 32, (void**)&out);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            RGBQUAD outPixel;
            FreeImage_GetPixelColor(bitmap, x, y, &outPixel);
            out[(y * width + x) * 4 + 0] = outPixel.rgbBlue;
            out[(y * width + x) * 4 + 1] = outPixel.rgbGreen;
            out[(y * width + x) * 4 + 2] = outPixel.rgbRed;
            if (FreeImage_IsTransparent(bitmap))
                out[(y * width + x) * 4 + 3] = outPixel.rgbReserved;
            else
                out[(y * width + x) * 4 + 3] = 255u;
        }
    }


    FreeImage_Unload(frame32bit);
    return outBitmap;
}
