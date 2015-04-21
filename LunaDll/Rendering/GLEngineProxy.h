#ifndef GLEngineProxy__hhhh
#define GLEngineProxy__hhhh

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <thread>
#include "GLDraw.h"
#include "../Misc/ThreadedCmdQueue.h"
#include "GLEngine.h"

struct GLEngineCmd {
    enum GLEngineCmdType {
        GL_ENGINE_CMD_CLEAR,
        GL_ENGINE_CMD_BITBLT,
        GL_ENGINE_CMD_STRETCHBLT,
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
    } mData;
};

class GLEngineProxy {
private:
    std::thread* mpThread;
    ThreadedCmdQueue<GLEngineCmd> mQueue;

    GLEngine mGLEngine;
protected:
    // Internal routines
    void Init();
    void ThreadMain();
    void ThreadMain2();
    void RunCmd(const GLEngineCmd& cmd);

public:
    GLEngineProxy();
    ~GLEngineProxy();
    void ClearTextures();

    // External commands
    void EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop);
    BOOL EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop);

    inline bool IsEnabled() { return mGLEngine.IsEnabled(); };
};

// Instance
extern GLEngineProxy g_GLEngine;

#endif
