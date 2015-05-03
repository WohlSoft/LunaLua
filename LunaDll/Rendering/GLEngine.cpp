#include <windows.h>
#include <SDL2/SDL.h>
#include <gl/glew.h>
#include "GLEngine.h"
#include "../Defines.h"
#include "../Globals.h"
#include "RenderUtils.h"
#include "GLDraw.h"
#include "GLTextureStore.h"
#include "GLContextManager.h"

GLEngine::GLEngine() :
    mInitialized(false), mHadError(false),
    mEnabled(false), mBitwiseCompat(true),
    mFB(0), mColorRB(0), mDepthRB(0) {
    mBufTex.w = 800;
    mBufTex.h = 600;
    mBufTex.name = NULL;
}

GLEngine::~GLEngine() {
}

void GLEngine::Init() {
    if (!g_GLContextManager.IsInitialized()) return;

    if (mInitialized || mHadError) return;
    mHadError = true;

#if 1
    // Set up framebuffer object
    glGenFramebuffersEXT(1, &mFB);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);

    mBufTex.w = 800;
    mBufTex.h = 600;
    g_GLDraw.Unbind();
    glGenTextures(1, &mBufTex.name);
    glBindTexture(GL_TEXTURE_2D, mBufTex.name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenRenderbuffersEXT(1, &mDepthRB);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mDepthRB);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 800, 600);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthRB);

    glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, mBufTex.name, 0);
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0_EXT };
    glDrawBuffers(1, DrawBuffers);

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        dbgboxA("error setting up");
    }

    // Bind framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set projection (test)
    glLoadIdentity();
    glOrtho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
    
    mInitialized = true;
    mHadError = false;
}

void GLEngine::ClearTextures() {
    g_GLTextureStore.ClearTextures();
}

void GLEngine::EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    if (!mInitialized) Init();
    if (!mInitialized) return;

    if (dwRop == BLACKNESS || dwRop == 0x10)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        g_GLDraw.DrawRectangle(nXDest, nYDest, nWidth, nHeight);
        glColor3f(1, 1, 1);
        return;
    }

    GLDraw::RenderMode mode;
    switch (dwRop) {
    case SRCAND:
        mode = mBitwiseCompat ? GLDraw::RENDER_MODE_AND : GLDraw::RENDER_MODE_MULTIPLY;
        break;
    case SRCPAINT:
        mode = mBitwiseCompat ? GLDraw::RENDER_MODE_OR : GLDraw::RENDER_MODE_MAX;
        break;
    default:
        mode = GLDraw::RENDER_MODE_ALPHA;
        break;
    }

    const GLDraw::Texture* tex = g_GLTextureStore.TextureFromBitmapHDC(hdcSrc);
    if (tex == NULL) {
        return;
    }

    g_GLDraw.DrawSprite(nXDest, nYDest, nWidth, nHeight, tex, nXSrc, nYSrc, mode);
}

BOOL GLEngine::EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
    DWORD dwRop)
{
    if (!g_GLContextManager.IsInitialized()) {
        if (!g_GLContextManager.Init(hdcDest)) {
            dbgboxA("Failed to init...");
        } else {
            //dbgboxA("Initialized");
        }
    }
    if (!mInitialized) Init();
    if (!mInitialized) return FALSE;

    g_GLDraw.Unbind();

    // Unbind the texture from the framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    // Set viewport for window size (Note: broken for split screen. Need to fix that later)
    glViewport(0.0f, 0.0f, nWidthDest, nHeightDest);
    glLoadIdentity();
    glOrtho(0.0f, nWidthDest, 0.0f, nHeightDest, -1.0f, 1.0f);

    // Draw the buffer, flipped/stretched as appropriate
    g_GLDraw.DrawStretched(nXOriginDest, nYOriginDest + nHeightDest, nWidthDest, -nHeightDest, &mBufTex, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
    glFlush();
    SwapBuffers(hdcDest);

    // Get ready to draw some more
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
    glViewport(0.0f, 0.0f, 800.0f, 600.0f);
    glLoadIdentity();
    glOrtho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return TRUE;
}
