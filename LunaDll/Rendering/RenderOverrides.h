#ifndef RenderOverrides_hhhh
#define RenderOverrides_hhhh

#include <windows.h>

void loadRenderOverrideGfx();
bool renderNpcMaskOverride(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, unsigned short npcid, int nXSrc, int nYSrc);
bool renderNpcOverride(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, unsigned short npcid, int nXSrc, int nYSrc);

#endif