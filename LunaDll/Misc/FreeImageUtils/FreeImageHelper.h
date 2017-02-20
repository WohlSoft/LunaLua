#ifndef FreeImageHelper_hhhhh
#define FreeImageHelper_hhhhh

// Forward declerations
struct FIBITMAP;
struct HBITMAP__;
typedef HBITMAP__ *HBITMAP;

namespace FreeImageHelper {
    HBITMAP CreateEmptyBitmap(int width, int height, int bpp, void** data);
    HBITMAP FromFreeImage(FIBITMAP* bitmap);
    bool ToRawBGRA(FIBITMAP* bitmap, void* out);
}

#endif