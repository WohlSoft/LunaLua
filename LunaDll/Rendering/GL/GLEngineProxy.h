#ifndef GLEngineProxy__hhhh
#define GLEngineProxy__hhhh

#include <gl/glew.h>
#include <thread>
#include <atomic>
#include <memory>
#include "../../Misc/ThreadedCmdQueue.h"
#include "GLDraw.h"
#include "GLEngineCmds.h"
#include "GLEngine.h"

class GLEngineProxy {
    friend class GLLock;
private:
    std::unique_ptr<std::thread> mpThread;
    ThreadedCmdQueue<std::shared_ptr<GLEngineCmd>> mQueue;
    std::atomic<uint32_t> mFrameCount;
    std::atomic<uint32_t> mPendingClear;
    bool mSkipFrame;
    bool mIsDirty; // FIXME: This is currently due to a Deadlock because of the destructor of the BMPBox-Cache @ BMPBox.cpp/loadShared/basegameImageCache
    
public:
    GLEngine mInternalGLEngine;
protected:
    // Internal routines
    void Init();
    void ThreadMain();
    
public:
    GLEngineProxy();
    ~GLEngineProxy();

    // Queue command
    void GLEngineProxy::QueueCmd(const std::shared_ptr<GLEngineCmd>& cmd);

    template <typename T>
    void GLEngineProxy::QueueCmd(const std::shared_ptr<T>& cmd) {
        QueueCmd(std::static_pointer_cast<GLEngineCmd>(cmd));
    }

    // Convenience command functions
    void ClearSMBXSprites();
    void ClearLunaTexture(const BMPBox& bmp);
    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    void RenderCameraToScreen(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop);
    void DrawLunaSprite(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        const BMPBox& bmp, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, float opacity);
    void EndFrame(HDC hdcDest);
    void InitForHDC(HDC hdcDest);

    inline bool IsEnabled() { return mInternalGLEngine.IsEnabled(); };
    inline void Enable() { mInternalGLEngine.Enable(); };
    inline void Disable() { mInternalGLEngine.Disable(); };
    inline void TriggerScreenshot() { mInternalGLEngine.TriggerScreenshot(); }
    inline void TriggerScreenshot(const SCREENSHOT_CALLBACK& func) { mInternalGLEngine.TriggerScreenshot(func); }
    inline bool GifRecorderToggle() { return mInternalGLEngine.GifRecorderToggle(); }

    // Stuff that run at the main thread:
    // Functions which run on the main thread, but needs to access GL resources
    std::shared_ptr<GLShader> CreateNewShader(const std::string& vertexSource, const std::string& fragmentSource);

    // With this we don't need SafeCall anymore. 
    // However, before any GL operations at the main thread this function has to be call first.
    void EnsureMainThreadCTXApplied();

    // Shutdown GL Engine:
    void Shutdown();
	
	static void CheckRendererInit(void);
};

// Instance
extern GLEngineProxy g_GLEngine;

#endif
