#include "RenderOverrides.h"
#include <unordered_map>
#include "../Defines.h"
#include "../SMBXInternal/Level.h"

#include <wincodec.h>
#include <wincodecsdk.h>

#include <atlbase.h>

static HBITMAP loadLevelNpcGfx(std::wstring& path, unsigned short npcid);
static IWICImagingFactory* getWICFactory();
static std::unordered_map<unsigned short, HBITMAP> npcGfxMap;

void loadRenderOverrideGfx()
{
    // Get world and level filenames
    std::wstring worldPath = std::wstring((wchar_t*)GM_FULLDIR);
    std::wstring levelFile = Level::GetName();

    // Generate the custom resource dir
    size_t idx = levelFile.rfind(L'.');
    if (idx == std::wstring::npos) return;
    std::wstring levelFolder = worldPath + levelFile.substr(0, idx) + L'\\';

    // Clear existing npc graphics resources
    for (auto kv : npcGfxMap) {
        DeleteObject(kv.second);
    }
    npcGfxMap.clear();

    // Load new npc graphics resources
    for (unsigned short i = 1; i < 300; i++) {
        HBITMAP npbBmp = loadLevelNpcGfx(levelFolder, i);
        if (npbBmp != NULL) {
            npcGfxMap[i] = npbBmp;
        }
    }
}

bool renderNpcMaskOverride(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, unsigned short npcid, int nXSrc, int nYSrc)
{
    auto it = npcGfxMap.find(npcid);
    if (it != npcGfxMap.end()) {
        return true;
    }

    return false;
}

bool renderNpcOverride(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, unsigned short npcid, int nXSrc, int nYSrc)
{
    auto it = npcGfxMap.find(npcid);
    if (it != npcGfxMap.end()) {
        HDC hdc = CreateCompatibleDC(hdcDest);
        SelectObject(hdc, it->second);

        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = AC_SRC_ALPHA;

        AlphaBlend(hdcDest, nXDest, nYDest, nWidth, nHeight, hdc, nXSrc, nYSrc, nWidth, nHeight, bf);

        DeleteDC(hdc);

        return true;
    }

    return false;
}

static HBITMAP loadLevelNpcGfx(std::wstring& path, unsigned short npcid)
{
    HRESULT hr;
    CComPtr<IWICImagingFactory> pFactory;
    CComPtr<IWICBitmapDecoder> pDecoder;
    CComPtr<IWICBitmapFrameDecode> pFrame = NULL;
    CComPtr<IWICFormatConverter> pConvertedFrame = NULL;
    HBITMAP hDIBBitmap = NULL;
    unsigned int width = 0, height = 0;

    std::wstring filename = path + L"npc-" + std::to_wstring(npcid) + L".png";
    
    // Skip if it doesn't exist
    if (GetFileAttributesW(filename.c_str()) == INVALID_FILE_ATTRIBUTES) return NULL;

    // Get an IWICImagingFactory
    pFactory = getWICFactory();
    if (pFactory == NULL) goto cleanup;

    hr = pFactory->CreateDecoderFromFilename(filename.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
    if (FAILED(hr)) goto cleanup;

    hr = pDecoder->GetFrame(0, &pFrame);
    if (FAILED(hr)) goto cleanup;

    hr = pFactory->CreateFormatConverter(&pConvertedFrame);
    if (FAILED(hr)) goto cleanup;

    hr = pFrame->GetSize(&width, &height);
    if (FAILED(hr)) goto cleanup;

    hr = pConvertedFrame->Initialize(
        pFrame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        NULL,
        0.f,
        WICBitmapPaletteTypeCustom
        );
    if (FAILED(hr)) goto cleanup;

    HDC hdcScreen = GetDC(NULL);
    if (hdcScreen == NULL) goto cleanup;

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -(int)height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pData = NULL;
    hDIBBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pData, NULL, 0);
    if (!hDIBBitmap) goto cleanup;

    ReleaseDC(NULL, hdcScreen);
    hdcScreen = NULL;

    // Calculate the number of bytes in 1 scanline
    unsigned int nStride = ((((width * 32) + 31) >> 5) << 2);
    // Calculate the total size of the image
    unsigned int nImage = nStride * height;
    // Copy the pixels to the DIB section
    hr = pConvertedFrame->CopyPixels(nullptr, nStride, nImage, (unsigned char*)pData);
    if (FAILED(hr))
    {
        DeleteObject(hDIBBitmap);
        hDIBBitmap = NULL;
        goto cleanup;
    }

cleanup:
    if (hdcScreen != NULL) {
        ReleaseDC(NULL, hdcScreen);
    }
    
    return hDIBBitmap;
}

static IWICImagingFactory* getWICFactory() {
    static IWICImagingFactory *pInstance = NULL;

    if (pInstance == NULL) {
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory, (LPVOID*)&pInstance);
    }

    return pInstance;
}
