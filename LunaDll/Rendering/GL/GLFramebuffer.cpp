#include "GLFramebuffer.h"
#include "GLContextManager.h"

GLFramebuffer::GLFramebuffer(int w, int h, bool haveAlpha) :
    mFB(0),
    mBufTex(0, w, h),
    mDepthTex(0, w, h)
{
    GLint glFormat = haveAlpha ? GL_RGBA : GL_RGB;

    // Set up framebuffer object
    glGenFramebuffersANY(1, &mFB);
    GLERRORCHECK();
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
    GLERRORCHECK();

    glGenTextures(1, &mBufTex.name);
    GLERRORCHECK();
    g_GLDraw.BindTexture(&mBufTex);
    GLERRORCHECK();
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, mBufTex.pw, mBufTex.ph, 0, glFormat, GL_UNSIGNED_BYTE, NULL);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLERRORCHECK();

    glFramebufferTexture2DANY(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mBufTex.name, 0);
    GLERRORCHECK();

    glGenTextures(1, &mDepthTex.name);
    GLERRORCHECK();
    g_GLDraw.BindTexture(&mDepthTex);
    GLERRORCHECK();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, mDepthTex.pw, mDepthTex.ph, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLERRORCHECK();

    glFramebufferTexture2DANY(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mDepthTex.name, 0);
    GLERRORCHECK();

    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0_EXT };
    glDrawBuffers(1, DrawBuffers);
    GLERRORCHECK();

    GLenum status = glCheckFramebufferStatusANY(GL_FRAMEBUFFER_EXT);
    GLERRORCHECK();
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
        throw;
    }

    // Clear the new framebuffer
    if (haveAlpha)
    {
        static const GLclampf colorTrans[] = { 0.0, 0.0, 0.0, 0.0 };
        Clear(colorTrans);
    }
    else
    {
        static const GLclampf colorBlack[] = { 0.0, 0.0, 0.0, 1.0 };
        Clear(colorBlack);
    }
}

GLFramebuffer::~GLFramebuffer()
{
    // Unbind framebuffer (if currently bound)
    if (g_GLContextManager.GetCurrentCameraFB() == this)
    {
        g_GLContextManager.RedirectCameraFB(nullptr);
    }
    if (g_GLContextManager.GetCurrentFB() == this)
    {
        g_GLContextManager.BindScreen();
    }

    // Unbind texture just in case
    if (mBufTex.name && (mBufTex.name == g_GLDraw.GetCurrentTexName())) {
        g_GLDraw.UnbindTexture();
    }

    // Delete framebuffer
    if (mFB) {
        glDeleteFramebuffersANY(1, &mFB);
        GLERRORCHECK();
        mFB = 0;
    }

    // Delete depth texture
    if (mDepthTex.name) {
        glDeleteTextures(1, &mDepthTex.name);
        GLERRORCHECK();
        mDepthTex.name = 0;
    }

    // Delete texture
    if (mBufTex.name) {
        glDeleteTextures(1, &mBufTex.name);
        GLERRORCHECK();
        mBufTex.name = 0;
    }
}

void GLFramebuffer::Bind()
{
    if (g_GLContextManager.GetCurrentFB() == this) return;

    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
    GLERRORCHECK();

    glViewport(0, 0, mBufTex.pw, mBufTex.ph);
    GLERRORCHECK();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLERRORCHECK();
    glOrtho(0.0f, ((float)mBufTex.pw), 0.0f, ((float)mBufTex.ph), -1.0f, 1.0f);
    GLERRORCHECK();

    // Record this binding...
    g_GLContextManager.SetCurrentFB(this);
}

void GLFramebuffer::Clear()
{
    static const GLclampf colorBlack[] = { 0.0, 0.0, 0.0, 1.0 };
    Clear(colorBlack);
}

void GLFramebuffer::Clear(const GLclampf color[4])
{
    GLFramebuffer* oldFB = g_GLContextManager.GetCurrentFB();

    if (oldFB != this)
    {
        Bind();
    }

    // Bind framebuffer
    glClearColor(color[0], color[1], color[2], color[3]);
    glClearDepth(100.0f);
    GLERRORCHECK();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLERRORCHECK();

    // Bind what was bound before
    if (oldFB != this)
    {
        // Restore the old bound FB (if null, bind screen)
        g_GLContextManager.RestoreBoundFB(oldFB);
    }
}