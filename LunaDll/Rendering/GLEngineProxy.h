#ifndef GLEngineProxy__hhhh
#define GLEngineProxy__hhhh

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <thread>
#include <atomic>
#include "GLDraw.h"
#include "../Misc/ThreadedCmdQueue.h"
#include "GLEngine.h"

struct GLEngineCmd {
    enum GLEngineCmdType {
        GL_ENGINE_CMD_CLEAR_SMBX_TEXTURES,
        GL_ENGINE_CMD_EMULATE_BITBLT,
        GL_ENGINE_CMD_EMULATE_STRETCHBLT,
        GL_ENGINE_CMD_END_FRAME,
        GL_ENGINE_CMD_EXIT
    };

    GLEngineCmdType mCmd;
    union {
        struct {
            int nXDest;
            int nYDest;
            int nWidth;
            int nHeight;
            HDC hdcSrc;
            int nXSrc;
            int nYSrc;
            DWORD dwRop;
        } mBitBlt;
        struct {
            HDC hdcDest;
            int nXOriginDest;
            int nYOriginDest;
            int nWidthDest;
            int nHeightDest;
            HDC hdcSrc;
            int nXOriginSrc;
            int nYOriginSrc;
            int nWidthSrc;
            int nHeightSrc;
            DWORD dwRop;
        } mStretchBlt;
        struct {
            HDC hdcDest;
        } mEndFrame;
    } mData;
};

class GLEngineProxy {
private:
    std::thread* mpThread;
    ThreadedCmdQueue<GLEngineCmd> mQueue;
    std::atomic<uint32_t> mFrameCount;
    std::atomic<uint32_t> mPendingClear;

    GLEngine mGLEngine;
protected:
    // Internal routines
    void Init();
    void ThreadMain();
    void RunCmd(const GLEngineCmd& cmd);

public:
    GLEngineProxy();
    ~GLEngineProxy();
    void ClearSMBXTextures();

    // External commands
    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    BOOL EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop);
    void EndFrame(HDC hdcDest);

    inline bool IsEnabled() { return mGLEngine.IsEnabled(); };
};

// Instance
extern GLEngineProxy g_GLEngine;

#endif
