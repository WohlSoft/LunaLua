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
		if (!g_GLContextManager.IsInitialized()) return;

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

	virtual bool allowFrameSkippability(void) const {
		return !mBuff->mNonskippable;
	}

	virtual bool isSkippable(void) const {
		return !mBuff->mNonskippable;
	}
};

class GLEngineCmd_ClearCaptureBuffer : public GLEngineCmd {
public:
    std::shared_ptr<CaptureBuffer> mBuff;
    virtual void run(GLEngine& glEngine) const
    {
		if (!g_GLContextManager.IsInitialized()) return;

        if (mBuff->mFramebuffer != nullptr)
        {
            static const gl::GLclampf colorTrans[] = { 0.0, 0.0, 0.0, 0.0 };
            mBuff->mFramebuffer->Clear(colorTrans);
        }
    }

	virtual bool isSkippable(void) const {
		return !mBuff->mNonskippable;
	}
};

class GLEngineCmd_DeleteCaptureBuffer : public GLEngineCmd {
public:
    GLFramebuffer* mFb;
    virtual void run(GLEngine& glEngine) const
    {
        delete mFb;
    }

	virtual bool isSkippable(void) const {
		return false;
	}
};

CaptureBuffer::CaptureBuffer(int w, int h, bool nonskippable) :
    mW(w), mH(h), mNonskippable(nonskippable), mFramebuffer(nullptr)
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
