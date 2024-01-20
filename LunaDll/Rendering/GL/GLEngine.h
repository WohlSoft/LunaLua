#ifndef GLEngine_hhhh
#define GLEngine_hhhh

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <chrono>
#include "GLDraw.h"
#include "../AsyncGifRecorder.h"

class GLShader;
typedef std::function<bool(HGLOBAL /*globalMem*/, const BITMAPINFOHEADER* /*header*/, void* /*pData*/, HWND /*curHwnd*/)> SCREENSHOT_CALLBACK;

class GLEngine {
public:
    static constexpr int mFrameTimeLen = 128;
    typedef float frameTimes_t[mFrameTimeLen];

private:
    bool mEnabled;
    bool mBitwiseCompat;

    HWND mHwnd;
    bool mScreenshot;
    SCREENSHOT_CALLBACK mScreenshotCallback;

    AsyncGifRecorder mGifRecorder;

    double mCameraX, mCameraY;

    GLShader* mpUpscaleShader;

    bool mFrameTimeInit;
    std::chrono::high_resolution_clock::time_point mFrameTimeTimestamp;
    std::mutex mFrameTimeMutex;
    int mFrameTimeIdx;
    frameTimes_t mFrameTimeBuffer;
    frameTimes_t mFrameTimeCopy;

public:
    GLEngine();
    ~GLEngine();

    inline bool IsBitwiseCompatEnabled() { return mBitwiseCompat; };

    void InitForHDC(HDC hdcDest);
    void EndFrame(HDC hdcDest, bool skipFlipToScreen, bool redrawOnly, bool resizeOverlay);

    // External commands
    inline bool IsEnabled() { return mEnabled; };
    inline void Enable() { mEnabled = true; }
    inline void Disable() { mEnabled = false; }
    inline void TriggerScreenshot() { mScreenshot = true; mScreenshotCallback = nullptr;  }
    inline void TriggerScreenshot(const SCREENSHOT_CALLBACK& screenshotCallback) { mScreenshot = true; mScreenshotCallback = screenshotCallback; }
    bool GifRecorderToggle();
    bool GifRecorderIsRunning();

    bool GenerateScreenshot(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    void GifRecorderNextFrame(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

    inline void SetCameraPositionInScene(double x, double y) { mCameraX = x; mCameraY = y; }
    inline void GetCamera(double &x, double &y) { x = mCameraX; y = mCameraY; }

    void RecordFrameTime();
    const frameTimes_t& GetFrameTimes();
};

#include "GLEngineProxy.h"

#endif
