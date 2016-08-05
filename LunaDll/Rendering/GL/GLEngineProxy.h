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
    std::thread* mpThread;
    ThreadedCmdQueue<std::shared_ptr<GLEngineCmd>> mQueue;
    std::atomic<uint32_t> mFrameCount;
    std::atomic<uint32_t> mPendingClear;
    bool mSkipFrame;
    // This is needed, if we need to do GL-stuff, but on another thread
    std::recursive_mutex mProxyLock; 

public:
    GLEngine mInternalGLEngine;
protected:
    // Internal routines
    void Init();
    void ThreadMain();
    inline std::recursive_mutex& getLock() { return mProxyLock; };

public:
    GLEngineProxy();
    ~GLEngineProxy();

    // Queue command
    void GLEngineProxy::QueueCmd(const std::shared_ptr<GLEngineCmd>& cmd);

    template <typename T>
    void GLEngineProxy::QueueCmd(const std::shared_ptr<T>& cmd) {
        QueueCmd(std::static_pointer_cast<GLEngineCmd>(cmd));
    }

    // Functions which run on the main thread, but needs to access GL resources
    std::shared_ptr<GLShader> CreateNewShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);

    // Other function on main thread
    template<typename FuncT>
    void SafeCall(FuncT func) {
        GLLock lock(*this, GLLock::MainThread);
        func();
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

    inline bool IsEnabled() { return mInternalGLEngine.IsEnabled(); };
    inline void Enable() { mInternalGLEngine.Enable(); };
    inline void Disable() { mInternalGLEngine.Disable(); };
    inline void TriggerScreenshot() { mInternalGLEngine.TriggerScreenshot(); }
    inline void TriggerScreenshot(const SCREENSHOT_CALLBACK& func) { mInternalGLEngine.TriggerScreenshot(func); }
    inline bool GifRecorderToggle() { return mInternalGLEngine.GifRecorderToggle(); }
};

class GLLock {
    friend class GLEngineProxy;
private:
    std::unique_lock<std::recursive_mutex> internal_lock;    
public:
    enum GLLockType {
        QueueThread,
        MainThread
    };

    GLLock(GLEngineProxy& engineToLock, GLLockType type);
};

// Instance
extern GLEngineProxy g_GLEngine;

#endif
