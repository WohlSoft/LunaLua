#include <memory>
#include "FrameCapture.h"
#include "GL/GLFramebuffer.h"
#include "GL/GLEngineCmds.h"
#include "RenderOps/RenderGLOp.h"
#include "Rendering.h"
#include "../Globals.h"
#include "GL/GLEngineProxy.h"
#include "GL/GLContextManager.h"

class GLEngineCmd_CaptureBuffer : public GLEngineCmd {
public:
    std::shared_ptr<CaptureBuffer> mBuff;
    virtual void run(GLEngine& glEngine) const
    {
        // Create framebuffer if not yet existing
        mBuff->EnsureFramebufferExists();

        GLFramebuffer* fb = mBuff->mFramebuffer;
        if (fb != nullptr)
        {
            int w = mBuff->mW, h = mBuff->mH;

            // Bind framebuffer
            fb->Bind();

            g_GLDraw.DrawStretched(0, 0, w, h, &g_GLContextManager.GetBufTex(), 0, 0, w, h, 1.0f);

            // Bind old framebuffer
            g_GLContextManager.BindFramebuffer();
        }
    }
};

class GLEngineCmd_ClearCaptureBuffer : public GLEngineCmd {
public:
    std::shared_ptr<CaptureBuffer> mBuff;
    virtual void run(GLEngine& glEngine) const
    {
        if (mBuff->mFramebuffer != nullptr)
        {
            static const GLclampf colorTrans[] = { 0.0, 0.0, 0.0, 0.0 };
            mBuff->mFramebuffer->Clear(colorTrans);
        }
    }
};

class GLEngineCmd_DeleteCaptureBuffer : public GLEngineCmd {
public:
    GLFramebuffer* mFb;
    virtual void run(GLEngine& glEngine) const
    {
        delete mFb;
    }
};

CaptureBuffer::CaptureBuffer(int w, int h) :
    mW(w), mH(h), mFramebuffer(nullptr)
{
}

CaptureBuffer::~CaptureBuffer()
{
    if (mFramebuffer != nullptr)
    {
        auto cmd = std::make_shared<GLEngineCmd_DeleteCaptureBuffer>();
        cmd->mFb = mFramebuffer;
        mFramebuffer = nullptr;
        g_GLEngine.QueueCmd(cmd);
    }
}

void CaptureBuffer::CaptureAt(double priority)
{
    auto cmd = std::make_shared<GLEngineCmd_CaptureBuffer>();
    cmd->mBuff = shared_from_this();
    gLunaRender.GLCmd(cmd, priority);
}

void CaptureBuffer::Clear(double priority)
{
    auto cmd = std::make_shared<GLEngineCmd_ClearCaptureBuffer>();
    cmd->mBuff = shared_from_this();
    gLunaRender.GLCmd(cmd, priority);
}

void CaptureBuffer::EnsureFramebufferExists()
{
    // Create framebuffer if not yet existing
    if (mFramebuffer == nullptr)
    {
        mFramebuffer = new GLFramebuffer(mW, mH, true);
    }
}
