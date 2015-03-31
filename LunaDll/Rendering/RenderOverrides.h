#ifndef RenderOverrides_hhhh
#define RenderOverrides_hhhh

#include <windows.h>

void loadRenderOverrideGfx();
bool renderOverrideBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);

#endif