#ifndef FreeImageHelper_hhhhh
#define FreeImageHelper_hhhhh
#include <Windows.h>
#include <FreeImage.h>

namespace FreeImageHelper {
    HBITMAP CreateEmptyBitmap(int width, int height, int bpp, void** data);
    HBITMAP FromFreeImage(FIBITMAP* bitmap);
}

#endif