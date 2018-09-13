#ifndef GLContextManager_hhhh
#define GLContextManager_hhhh

#include <windows.h>
#include <glbinding/gl/gl.h>
#include <glbinding/ContextHandle.h>
#include "GLTextureStore.h"
#include "GLFramebuffer.h"

class GLContextManager {
public:
    GLContextManager();
    bool Init(HDC hDC);
    bool IsInitialized();

    void BindScreen();
    void BindAndClearFramebuffer();
    void BindFramebuffer();
    inline const GLDraw::Texture& GetBufTex()
    {
        static GLDraw::Texture nullTex(0, 0, 0);
        if (mFramebuffer == nullptr)
        {
            return nullTex;
        }
        return mFramebuffer->AsTexture();
    }
    inline GLFramebuffer* GetCurrentFB() { return mCurrentFB; }
    inline void SetCurrentFB(GLFramebuffer* fb) { mCurrentFB = fb; }
    void EnsureMainThreadCTXApplied();

private:
    bool  mInitialized;
    bool  mHadError;
    bool  mMainThreadCTXApplied;  // Whether wglMakeCurrent has been called for the main thread or not

    // Context variables
    int   mOldPixelFormat;
    HDC   hDC;
    HGLRC hQueueThreadCTX;
    HGLRC hMainThreadCTX;
    
    glbinding::ContextHandle mQueueThreadCTXHandle;

    // Framebuffer variables
    GLFramebuffer* mCurrentFB;
    GLFramebuffer* mFramebuffer;

    // Init functions
    bool InitContextFromHDC(HDC hDC);
    bool InitFramebuffer();
    bool InitProjectionAndState();

    // Release functions
    void ReleaseContext();
    void ReleaseFramebuffer();
};

// Instance
extern GLContextManager g_GLContextManager;

#endif
