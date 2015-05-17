#include <windows.h>
#include <gl/glew.h>
#include "GLEngine.h"
#include "../Defines.h"
#include "../Globals.h"
#include "RenderUtils.h"
#include "GLCompat.h"
#include "GLDraw.h"
#include "GLTextureStore.h"
#include "GLContextManager.h"

GLEngine::GLEngine() :
    mInitialized(false), mHadError(false),
    mEnabled(true), mBitwiseCompat(true),
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
    glGenFramebuffersANY(1, &mFB);
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);

    mBufTex.w = 800;
    mBufTex.h = 600;
    g_GLDraw.Unbind();
    glGenTextures(1, &mBufTex.name);
    glBindTexture(GL_TEXTURE_2D, mBufTex.name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenRenderbuffersANY(1, &mDepthRB);
    glBindRenderbufferANY(GL_RENDERBUFFER_EXT, mDepthRB);
    glRenderbufferStorageANY(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 800, 600);
    glFramebufferRenderbufferANY(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthRB);
    
    glFramebufferTexture2DANY(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mBufTex.name, 0);
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0_EXT };
    glDrawBuffers(1, DrawBuffers);

    GLenum status = glCheckFramebufferStatusANY(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        dbgboxA("error setting up");
    }

    // Bind framebuffer
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
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

void GLEngine::ClearSMBXTextures() {
    g_GLDraw.Unbind();
    g_GLTextureStore.ClearSMBXTextures();
}

void GLEngine::ClearLunaTexture(const BMPBox& bmp) {
    g_GLDraw.Unbind();
    g_GLTextureStore.ClearLunaTexture(bmp);
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

    const GLDraw::Texture* tex = g_GLTextureStore.TextureFromSMBXBitmap(hdcSrc);
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
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, 0);

    // Get window size
    RECT clientRect;
    GetClientRect(WindowFromDC(hdcDest), &clientRect);
    int32_t windowWidth = clientRect.right - clientRect.left;
    int32_t windowHeight = clientRect.bottom - clientRect.top;

    // Implement letterboxing correction
    float scaledWidth = windowWidth / 800.0f;
    float scaledHeight = windowHeight / 600.0f;
    float xScale = 1.0f;
    float yScale = 1.0f;
    if (scaledWidth > scaledHeight) {
        xScale = scaledHeight / scaledWidth;
    } else if (scaledWidth < scaledHeight) {
        yScale = scaledWidth / scaledHeight;
    }
    float xOffset = (windowWidth / xScale - windowWidth) * 0.5f;
    float yOffset = (windowHeight / yScale - windowHeight) * 0.5f;

    // Set viewport for window size
    glViewport(0, 0, windowWidth, windowHeight);
    glLoadIdentity();
    glOrtho(-xOffset, (float)windowWidth + xOffset, (float)windowHeight + yOffset, -yOffset, -1.0f, 1.0f);

    // Draw the buffer, flipped/stretched as appropriate
    g_GLDraw.DrawStretched(nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, &mBufTex, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
    glFlush();

    // Get ready to draw some more
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
    glViewport(0, 0, 800, 600);
    glLoadIdentity();
    glOrtho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return TRUE;
}

void GLEngine::DrawLunaSprite(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    const BMPBox& bmp, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc)
{
    if (!mInitialized) Init();
    if (!mInitialized) return;

    const GLDraw::Texture* tex = g_GLTextureStore.TextureFromLunaBitmap(bmp);
    if (tex == NULL) {
        return;
    }

    g_GLDraw.DrawStretched(nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, tex, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

void GLEngine::EndFrame(HDC hdcDest)
{
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, 0);
    SwapBuffers(hdcDest);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
}

void GLEngine::SetTex(const BMPBox* bmp, uint32_t color) {
    const GLDraw::Texture* tex = NULL;
    if (bmp) {
        tex = g_GLTextureStore.TextureFromLunaBitmap(*bmp);
    }

    glBlendEquationANY(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    if (tex) {
        if (g_GLDraw.mLastTexName != tex->name)
        {
            g_GLDraw.mLastTexName = tex->name;
            glBindTexture(GL_TEXTURE_2D, tex->name);
        }
    }
    else {
        g_GLDraw.Unbind();
    }

    float r = ((0xFF0000 & color) >> 16) / 255.0f;
    float g = ((0x00FF00 & color) >> 8) / 255.0f;
    float b = ((0x0000FF & color) >> 0) / 255.0f;
    glColor3f(r, g, b);
}

void GLEngine::DrawTriangles(const float* vert, const float* tex, uint32_t count) {
    glVertexPointer(2, GL_FLOAT, 0, vert);
    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glDrawArrays(GL_TRIANGLES, 0, count);
}
