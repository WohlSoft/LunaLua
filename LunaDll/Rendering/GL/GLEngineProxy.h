#ifndef GLEngineProxy__hhhh
#define GLEngineProxy__hhhh

#include <GL/glew.h>
#include <thread>
#include <atomic>
#include <memory>
#include <queue>
#include "../../Misc/ThreadedCmdQueue.h"
#include "GLDraw.h"
#include "GLEngineCmds.h"
#include "GLEngine.h"

class GLEngineProxy {
    friend class GLLock;
public:
    struct FrameStatStruct
    {
        unsigned long long skipCount;
        unsigned long long totalCount;
    };
private:
    std::thread* mpThread;
    ThreadedCmdQueue<std::shared_ptr<GLEngineCmd>> mQueue;
    ThreadedCmdQueue<uint64_t> mDeletedTextures;
    std::atomic<uint32_t> mPendingClear;
    bool mSkipFrame;
    
    bool mNextEndFrameIsSkippable;
    ThreadedCmdQueue<bool> mQueuedFrameSkippability;

    std::atomic<FrameStatStruct> mFrameStats;

    bool mFirstFramePending;
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
    void QueueCmd(const std::shared_ptr<GLEngineCmd>& cmd);

    template <typename T>
    void QueueCmd(const std::shared_ptr<T>& cmd) {
        QueueCmd(std::static_pointer_cast<GLEngineCmd>(cmd));
    }

    void NotifyTextureDeletion(uint64_t uid) { mDeletedTextures.push(uid); }

    // Convenience command functions
    void ClearTextures();
    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    void RenderCameraToScreen(int camIdx, double renderX, double renderY, double height, double width);
    void EndFrame(HDC hdcDest, bool isLoadScreen=false, bool redrawOnly=false, bool resizeOverlay=false, bool pauseOverlay=false);
    void InitForHDC(HDC hdcDest);

    inline bool IsEnabled() { return mInternalGLEngine.IsEnabled(); };
    inline void Enable() { mInternalGLEngine.Enable(); };
    inline void Disable() { mInternalGLEngine.Disable(); };
    inline void TriggerScreenshot() { mInternalGLEngine.TriggerScreenshot(); }
    inline void TriggerScreenshot(const SCREENSHOT_CALLBACK& func) { mInternalGLEngine.TriggerScreenshot(func); }
    inline bool GifRecorderToggle() { return mInternalGLEngine.GifRecorderToggle(); }
    inline bool GifRecorderIsRunning() { return mInternalGLEngine.GifRecorderIsRunning(); }

    // Stuff that run at the main thread:
    // Functions which run on the main thread, but needs to access GL resources
    std::shared_ptr<GLShader> CreateNewShader(const std::string& vertexSource, const std::string& fragmentSource);

    // With this we don't need SafeCall anymore. 
    // However, before any GL operations at the main thread this function has to be call first.
    void EnsureMainThreadCTXApplied();

    static HDC GetHDC(void);
    static void CheckRendererInit(void);

    FrameStatStruct GetFrameStats(void) { return mFrameStats.load(std::memory_order_relaxed); }

    void SetFirstFramePending(void) { mFirstFramePending = true; }
};

// Instance
extern GLEngineProxy g_GLEngine;

#endif
