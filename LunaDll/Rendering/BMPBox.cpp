#include <algorithm>
#include "BMPBox.h"
#include "../Globals.h"
#include "RenderUtils.h"
#include "GL/GLEngine.h"

// CTOR
BMPBox::BMPBox() {
    Init();
}

// DTOR
BMPBox::~BMPBox() {
    // If the GL Engine is running, deallocate the associated texture and erase the BMPBox->Texture mapping.
    if (g_GLEngine.IsEnabled())
    {
        g_GLEngine.ClearLunaTexture(*this);
    }

    if (m_hbmp != NULL) {
        DeleteObject(m_hbmp);
        m_hbmp = NULL;
    }
    if (m_hdc != NULL) {
        DeleteDC(m_hdc);
        m_hdc = NULL;
    }
}

// CTOR - Load from a file path
BMPBox::BMPBox(std::wstring filename, HDC screen_dc) {
    Init();

    if (filename.length() < 1)
        return;

    // Load any image, converted to pre-multiplied BGRA
    m_hbmp = LoadGfxAsBitmap(filename);

    //gLogger.Log(L"Requested handle for: " + filename, LOG_STD);
    //int lasterr = GetLastError();
    //gLogger.Log(L"Last error: " + to_wstring((long long)lasterr), LOG_STD);

    m_hdc = CreateCompatibleDC(screen_dc);
    SelectObject(m_hdc, m_hbmp);

    // Get dimensions
    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(m_hbmp, sizeof(BITMAP), &bm);
    m_H = bm.bmHeight;
    m_W = bm.bmWidth;
    m_Filename = filename;
}

BMPBox::BMPBox(HBITMAP bitmapData, HDC screen_dc)
{
    Init();

    // Load any image, converted to pre-multiplied BGRA
    m_hbmp = bitmapData;

    //gLogger.Log(L"Requested handle for: " + filename, LOG_STD);
    //int lasterr = GetLastError();
    //gLogger.Log(L"Last error: " + to_wstring((long long)lasterr), LOG_STD);

    m_hdc = CreateCompatibleDC(screen_dc);
    SelectObject(m_hdc, m_hbmp);

    // Get dimensions
    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(m_hbmp, sizeof(BITMAP), &bm);
    m_H = bm.bmHeight;
    m_W = bm.bmWidth;
}

std::shared_ptr<BMPBox> BMPBox::loadShared(const std::wstring& filename)
{
    static std::map<std::wstring, std::shared_ptr<BMPBox>> basegameImageCache;

    // Check for in basegameImageCache
    {
        auto it = basegameImageCache.find(filename);
        if (it != basegameImageCache.end())
        {
            return it->second;
        }
    }

    // Try to load the image
    std::shared_ptr<BMPBox> img = std::make_shared<BMPBox>(filename, gLunaRender.GetScreenDC());

    if (img->ImageLoaded() == false) {
        // If image loading failed, return null
        return nullptr;
    }

    // Check if this image path is within the basegame's graphics folder
    {
        std::wstring basePath = gAppPathWCHAR + L"\\graphics\\";
        bool notInPath = false;
        for (auto i1 = basePath.cbegin(), i2 = filename.cbegin();
            (i1 != basePath.cend()) && (i2 != filename.cend()); ++i1, ++i2)
        {
            wchar_t w1 = ::towlower(*i1);
            wchar_t w2 = ::towlower(*i2);
            if (w1 == L'/') w1 = L'\\';
            if (w2 == L'/') w2 = L'\\';
            if (w1 != w2)
            {
                notInPath = true;
                break;
            }
        }
        
        // If it is in the basegame path, cache it
        if (!notInPath)
        {
            basegameImageCache[filename] = img;
        }
    }

    return img;
}

// INIT
void BMPBox::Init() {
    m_H = 0;
    m_W = 0;
    m_hbmp = NULL;
    m_hdc = NULL;
    m_modified.store(false, std::memory_order_relaxed);
}

// Makes a specified color transparent
void BMPBox::MakeColorTransparent(int rgb_color) {
    union {
        struct {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        } asStruct;
        uint32_t asInt;
    } bgra_color;

    // Convert RGB to BGRA
    bgra_color.asStruct.b = (rgb_color & 0x0000FF);
    bgra_color.asStruct.g = (rgb_color & 0x00FF00) >> 8;
    bgra_color.asStruct.r = (rgb_color & 0xFF0000) >> 16;
    bgra_color.asStruct.a = 255;

    if (m_hbmp) {
        BITMAP bm;
        memset(&bm, 0, sizeof(BITMAP));
        GetObject(m_hbmp, sizeof(BITMAP), &bm);
        uint32_t *pData = (uint32_t *)bm.bmBits;
        uint32_t dataLen = bm.bmHeight * bm.bmWidth;

        // Presuming we can get the bitmap data, replace all of the color with
        // transparency.
        if (pData) {
            for (uint32_t idx = 0; idx < dataLen; idx++) {
                if (pData[idx] == bgra_color.asInt) {
                    pData[idx] = 0;
                }
            }
        }
    }
}

// IMAGE LOADED - Returns true if this object loaded correctly / the bitmap handle isn't null
bool BMPBox::ImageLoaded() {
    if (m_hbmp == NULL)
        return false;
    return true;
}

void BMPBox::forEachPixelValue(std::function<void(BYTE)> forEachFunc)
{
    if (!forEachFunc)
        return;

    if (!m_hbmp)
        return;

    BITMAP bm;
    memset(&bm, 0, sizeof(BITMAP));
    GetObject(m_hbmp, sizeof(BITMAP), &bm);
    BYTE *pData = (BYTE *)bm.bmBits;
    uint32_t dataLen = bm.bmHeight * bm.bmWidth * 4;

    if (pData) {
        for (uint32_t i = 0; i < dataLen; i++) {
            forEachFunc(pData[i]);
        }
    }
    
}

void* BMPBox::getBits()
{
    BITMAP bm = { 0 };
    GetObject(m_hbmp, sizeof(bm), &bm);
    return bm.bmBits;
}

BMPBox* BMPBox::loadIfExist(const std::wstring& filename, HDC screen_dc)
{
    DWORD fAttrib = GetFileAttributesW(filename.c_str());
    if (fAttrib == INVALID_FILE_ATTRIBUTES || fAttrib & FILE_ATTRIBUTE_DIRECTORY)
        return nullptr;
    return new BMPBox(filename, screen_dc);
}
