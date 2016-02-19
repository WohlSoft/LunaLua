#ifndef GLEngine_hhhh
#define GLEngine_hhhh

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <unordered_map>
#include <functional>
#include "GLDraw.h"
#include "../BMPBox.h"
#include "../AsyncGifRecorder.h"
#include "GLTextureStore.h"

typedef std::function<bool(HGLOBAL /*globalMem*/, const BITMAPINFOHEADER* /*header*/, void* /*pData*/, HWND /*curHwnd*/)> SCREENSHOT_CALLBACK;

class GLEngine {
private:
    bool mEnabled;
    bool mBitwiseCompat;

    HWND mHwnd;
    bool mScreenshot;
    SCREENSHOT_CALLBACK mScreenshotCallback;

    AsyncGifRecorder mGifRecorder;

    double mCameraX, mCameraY;

public:
    GLEngine();
    ~GLEngine();
    void ClearSMBXSprites();
    void ClearLunaTexture(const BMPBox& bmp);

    inline bool IsBitwiseCompatEnabled() { return mBitwiseCompat; };

    BOOL RenderCameraToScreen(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop);
    void EndFrame(HDC hdcDest);

    // External commands
    inline bool IsEnabled() { return mEnabled; };
    inline void Enable() { mEnabled = true; }
    inline void Disable() { mEnabled = false; }
    inline void TriggerScreenshot() { mScreenshot = true; mScreenshotCallback = nullptr;  }
    inline void TriggerScreenshot(const SCREENSHOT_CALLBACK& screenshotCallback) { mScreenshot = true; mScreenshotCallback = screenshotCallback; }
    bool GifRecorderToggle();

    bool GenerateScreenshot(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    void GifRecorderNextFrame(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

    inline void SetCamera(double x, double y) { mCameraX = x; mCameraY = y; }
    inline void GetCamera(double &x, double &y) { x = mCameraX; y = mCameraY; }
};

#include "GLEngineProxy.h"

#endif
