#ifndef GLEngineProxy__hhhh
#define GLEngineProxy__hhhh

#include <gl/glew.h>
#include <thread>
#include <atomic>
#include "GLDraw.h"
#include "../Misc/ThreadedCmdQueue.h"
#include "GLEngineCmds.h"
#include "GLEngine.h"

class GLEngineProxy {
private:
    std::thread* mpThread;
    ThreadedCmdQueue<GLEngineCmd> mQueue;
    std::atomic<uint32_t> mFrameCount;
    std::atomic<uint32_t> mPendingClear;
    bool mSkipFrame;

    GLEngine mGLEngine;
protected:
    // Internal routines
    void Init();
    void ThreadMain();
    void RunCmd(const GLEngineCmd& cmd);

public:
    GLEngineProxy();
    ~GLEngineProxy();

    // Queue command
    void GLEngineProxy::QueueCmd(const GLEngineCmd cmd);

    // Convenience command functions
    void ClearSMBXTextures();
    void ClearLunaTexture(const BMPBox& bmp);
    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    void EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop);
    void DrawLunaSprite(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        const BMPBox& bmp, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc);
    void EndFrame(HDC hdcDest);

    inline bool IsEnabled() { return mGLEngine.IsEnabled(); };
    inline void Enable() { mGLEngine.Enable(); };
    inline void Disable() { mGLEngine.Disable(); };
};

// Instance
extern GLEngineProxy g_GLEngine;

#endif
