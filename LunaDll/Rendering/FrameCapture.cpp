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
        GLFramebuffer* fb = mBuff->mFramebuffer;
        int w = mBuff->mW, h = mBuff->mH;
        if (fb == nullptr)
        {
            fb = new GLFramebuffer(w, h);
            mBuff->mFramebuffer = fb;
        }
        
        // Bind framebuffer
        fb->Bind();

        g_GLDraw.DrawStretched(0, 0, w, h, &g_GLContextManager.GetBufTex(), 0, 0, w, h, 1.0f);

        // Bind old framebuffer
        g_GLContextManager.BindFramebuffer();
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

void CaptureBuffer::captureAt(double priority)
{
    auto cmd = std::make_shared<GLEngineCmd_CaptureBuffer>();
    cmd->mBuff = shared_from_this();
    gLunaRender.GLCmd(cmd, priority);
}
