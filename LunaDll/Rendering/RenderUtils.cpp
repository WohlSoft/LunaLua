#include "RenderUtils.h"

#include <wincodec.h>
#include <wincodecsdk.h>
#include <comutil.h>
#include <functional>
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Misc/FreeImageUtils/FreeImageData.h"

HBITMAP CreateEmptyBitmap(int width, int height, int bpp, void** data)
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

void LoadGfxAndEnumFrames(const std::wstring& filename, std::function<bool(HBITMAP, IWICMetadataQueryReader*)> bitmapEnumFunc){
    HRESULT hr;
    IWICImagingFactory *pFactory = NULL;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pFrame = NULL;
    IWICFormatConverter *pConvertedFrame = NULL;
    IWICMetadataQueryReader* pMetadataReader = NULL;
    HBITMAP hDIBBitmap = NULL;
    unsigned int width = 0, height = 0;

    
    // Skip if it doesn't exist
    if (GetFileAttributesW(filename.c_str()) == INVALID_FILE_ATTRIBUTES) return;

    // Get an IWICImagingFactory
    pFactory = getWICFactory();
    if (pFactory == NULL) goto cleanup;

    hr = pFactory->CreateDecoderFromFilename(filename.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder);
    if (FAILED(hr)) goto cleanup;

    UINT frames = 0;
    hr = pDecoder->GetFrameCount(&frames);
    

    if (FAILED(hr)) goto cleanup;
    for (UINT i = 0; i < frames; i++){
        hr = pDecoder->GetFrame(i, &pFrame);
        if (FAILED(hr)) goto cleanup;
        
        hr = pFrame->GetMetadataQueryReader(&pMetadataReader);
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

        void* pData = NULL;
        hDIBBitmap = CreateEmptyBitmap(width, height, 32, &pData);
        if (!hDIBBitmap) goto cleanup;

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
        if (!bitmapEnumFunc(hDIBBitmap, pMetadataReader))
            break;
        
        if (pMetadataReader) {
            pMetadataReader->Release();
            pMetadataReader = NULL;
        }
        if (pConvertedFrame) {
            pConvertedFrame->Release();
            pConvertedFrame = NULL;
        }
        if (pFrame) {
            pFrame->Release();
            pFrame = NULL;
        }
    }

cleanup:
    // NOTE: Not using CComPtr here because this should be possible to build with VS Express
#pragma warning(suppress: 6102)
    if (pMetadataReader) {
        pMetadataReader->Release();
    }
    if (pConvertedFrame) {
        pConvertedFrame->Release();
    }
    if (pFrame) {
        pFrame->Release();
    }
    if (pDecoder) {
        pDecoder->Release();
    }
}


HBITMAP LoadGfxAsBitmap(const std::wstring& filename)
{
    HBITMAP retSingleBitmap = NULL;
    LoadGfxAndEnumFrames(filename, [&retSingleBitmap](HBITMAP nextBitmap, IWICMetadataQueryReader*){
        retSingleBitmap = nextBitmap;
        return false;
    });

    return retSingleBitmap;
}

std::tuple<std::vector<HBITMAP>, int> LoadAnimatedGfx(const std::wstring& filename)
{
    std::vector<HBITMAP> allBitmapFrames;
    int frameDelayQueries = 0;
    short sumFrameDelay = 0;
    LoadGfxAndEnumFrames(filename, [&allBitmapFrames, &frameDelayQueries, &sumFrameDelay](HBITMAP nextBitmap, IWICMetadataQueryReader* pMetadata){
        allBitmapFrames.push_back(nextBitmap);
        
        _variant_t delayVal;
        HRESULT hr = pMetadata->GetMetadataByName(L"/grctlext/Delay", (PROPVARIANT*)&delayVal);
        if (SUCCEEDED(hr)) {
            frameDelayQueries++;
            sumFrameDelay += (short)delayVal;
        }
        
        return true; //continue enum
    });
    
    int frameTime = 0;
    if (frameDelayQueries == 0) 
    {
        frameTime = 9;
    }
    else
    {
        frameTime = sumFrameDelay / frameDelayQueries;
    }

    return std::make_tuple(allBitmapFrames, frameTime);
}


IWICImagingFactory* getWICFactory() {
    static IWICImagingFactory *pInstance = NULL;

    if (pInstance == NULL) {
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory, (LPVOID*)&pInstance);
    }

    return pInstance;
}

void GenerateScreenshot(const std::wstring& fName, const BITMAPINFOHEADER& header, void* pData)
{
    FreeImageData screenshotFile;
    screenshotFile.init(header.biWidth, header.biHeight, (BYTE*)pData);
    screenshotFile.saveFile(utf8_encode(fName));
}
