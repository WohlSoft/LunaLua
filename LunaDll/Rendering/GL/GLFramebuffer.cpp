#include "GLFramebuffer.h"
#include "GLContextManager.h"

using namespace gl;
using namespace glcompat;

GLFramebuffer::GLFramebuffer(int w, int h) :
    mFB(0), mDepthRB(0),
    mBufTex(0, w, h)
{
    // Set up framebuffer object
    glGenFramebuffersANY(1, &mFB);
    GLERRORCHECK();
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
    GLERRORCHECK();

    glGenTextures(1, &mBufTex.name);
    GLERRORCHECK();
    g_GLDraw.BindTexture(&mBufTex);
    GLERRORCHECK();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mBufTex.pw, mBufTex.ph, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLERRORCHECK();

    glGenRenderbuffersANY(1, &mDepthRB);
    GLERRORCHECK();
    glBindRenderbufferANY(GL_RENDERBUFFER_EXT, mDepthRB);
    GLERRORCHECK();
    glRenderbufferStorageANY(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, mBufTex.pw, mBufTex.ph);
    GLERRORCHECK();
    glFramebufferRenderbufferANY(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mDepthRB);
    GLERRORCHECK();

    glFramebufferTexture2DANY(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mBufTex.name, 0);
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
    Clear();
}

GLFramebuffer::~GLFramebuffer()
{
    // Unbind framebuffer
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, 0);
    GLERRORCHECK();

    // Unbind texture just in case
    g_GLDraw.UnbindTexture();

    // Delete framebuffer
    if (mFB) {
        glDeleteFramebuffersANY(1, &mFB);
        GLERRORCHECK();
        mFB = 0;
    }

    // Delete depth renderbuffer
    if (mDepthRB) {
        glDeleteRenderbuffersANY(1, &mDepthRB);
        GLERRORCHECK();
        mDepthRB = 0;
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
    GLFramebuffer* oldFB = g_GLContextManager.GetCurrentFB();

    if (oldFB != this)
    {
        Bind();
    }

    // Bind framebuffer
    glBindFramebufferANY(GL_FRAMEBUFFER_EXT, mFB);
    GLERRORCHECK();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    GLERRORCHECK();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLERRORCHECK();

    // Bind what was bound before
    if (oldFB == nullptr)
    {
        g_GLContextManager.BindScreen();
    }
    else if (oldFB != this)
    {
        oldFB->Bind();
    }
}