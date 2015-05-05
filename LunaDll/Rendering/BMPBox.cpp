#include "BMPBox.h"
#include "../Globals.h"
#include "RenderUtils.h"

// CTOR
BMPBox::BMPBox() {
    Init();
}

// DTOR
BMPBox::~BMPBox() {
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
}

// INIT
void BMPBox::Init() {
    m_H = 0;
    m_W = 0;
    m_hbmp = NULL;
    m_hdc = NULL;
}

// Makes a specified color transparent
void BMPBox::MakeColorTransparent(int transparency_color) {
    // Convert RGB to BGRA
    uint32_t match_color = (
        ((uint32_t)(transparency_color & 0xFF0000) >> 8) |
        ((uint32_t)(transparency_color & 0x00FF00) << 8) |
        ((uint32_t)(transparency_color & 0x0000FF) << 24) |
        0xFF
    );

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
                if (pData[idx] == match_color) {
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
