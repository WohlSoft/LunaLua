#ifndef RenderUtils_hhhh
#define RenderUtils_hhhh

#include <windows.h>
#include <string>

HBITMAP CreateEmptyBitmap(int width, int height, int bpp, void** data);
HBITMAP LoadGfxAsBitmap(const std::wstring& filename);

#endif