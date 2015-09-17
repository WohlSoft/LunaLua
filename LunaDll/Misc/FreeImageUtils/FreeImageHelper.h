#ifndef FreeImageHelper_hhhhh
#define FreeImageHelper_hhhhh
#include <Windows.h>

namespace FreeImageHelper {
    HBITMAP CreateEmptyBitmap(int width, int height, int bpp, void** data);
}

#endif