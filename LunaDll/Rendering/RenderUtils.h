#ifndef RenderUtils_hhhh
#define RenderUtils_hhhh

#include <windows.h>
#include <string>
#include <vector>
#include <tuple>
#include <wincodec.h>
#include "../Misc/FreeImageUtils/FreeImageHelper.h"

HBITMAP LoadGfxAsBitmap(const std::wstring& filename);
std::tuple<std::vector<HBITMAP>, int> LoadAnimatedGfx(const std::wstring& filename);
void GenerateScreenshot(const std::wstring& fName, const BITMAPINFOHEADER& header, void* pData);

#endif