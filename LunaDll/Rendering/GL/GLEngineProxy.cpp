#include <thread>
#include <memory>
#include "../../Defines.h"
#include "../../Misc/ThreadedCmdQueue.h"
#include "GLEngineProxy.h"
#include "GLEngineCmds.h"
#include "GLContextManager.h"
#include "../Shaders/GLShader.h"

// Instance
GLEngineProxy g_GLEngine;

GLEngineProxy::GLEngineProxy() {
    mFrameCount = 0;
    mPendingClear = 0;
    mSkipFrame = false;
	mNextEndFrameIsSkippable = true;
    mpThread = NULL;
}

GLEngineProxy::~GLEngineProxy() {
    if (mpThread != NULL) {
        QueueCmd(std::make_shared<GLEngineCmd_Exit>());
        mpThread->join();
    }
}

void GLEngineProxy::Init() {
    // Don't use built-in SMBX frameskip for OpenGL renderer.
    GM_FRAMESKIP = COMBOOL(false);

    if (mpThread == NULL) {
        mpThread = new std::thread( [this] {this->ThreadMain(); });
    }
}

void GLEngineProxy::ThreadMain() {
    while (1) {
        std::shared_ptr<GLEngineCmd> cmd = mQueue.peek();

        if ((mPendingClear == 0 && !mSkipFrame) || (!cmd->isSkippable()) || cmd->shouldBeSynchronous()) {
            cmd->run(mInternalGLEngine);
        }

        if (cmd->isSmbxClearCmd()) {
            mPendingClear--;
        }
        if (cmd->isFrameEnd()) {
			bool mFrameSkippable = mQueuedFrameSkippability.front();
			mQueuedFrameSkippability.pop();

            if (mFrameCount-- > 1) {
				mSkipFrame = mFrameSkippable;
            }
            else {
                mSkipFrame = false;
            }

            // Clean up deleted textures if any
            while (!mDeletedTextures.empty())
            {
                uint64_t uid = mDeletedTextures.pop();
                g_GLTextureStore.ClearLunaImageTexture(uid);
            }
        }

        mQueue.pop();

        // Upon exit command, exit the thread
        if (cmd->isExitCmd()) return;
    }
}

void GLEngineProxy::QueueCmd(const std::shared_ptr<GLEngineCmd> &cmd) {
    // Don't queue commands if we're not enabled
    if (!IsEnabled()) return;

    // Ensure we're initialized
    Init();

	// Allow frame skippability to be invalidated by something
	mNextEndFrameIsSkippable = mNextEndFrameIsSkippable && cmd->allowFrameSkippability();

    if (cmd->isFrameEnd()) { 
        // Increment count of stored frames
        mFrameCount++;
		mQueuedFrameSkippability.push(mNextEndFrameIsSkippable);
		mNextEndFrameIsSkippable = true;
    }
    if (cmd->isSmbxClearCmd())
    {
        mPendingClear++;
    }

    // Push the command
    mQueue.push(cmd);

    if (cmd->shouldBeSynchronous())
    {
        mQueue.waitTillEmpty();
    }
}


std::shared_ptr<GLShader> GLEngineProxy::CreateNewShader(const std::string& vertexSource, const std::string& fragmentSource)
{
    // Be sure that the main thread has the CTX applied before creating our shader.
    g_GLContextManager.EnsureMainThreadCTXApplied();
    
    return std::make_shared<GLShader>(vertexSource, fragmentSource);
}

void GLEngineProxy::EnsureMainThreadCTXApplied()
{
    g_GLContextManager.EnsureMainThreadCTXApplied();
}

void GLEngineProxy::ClearTextures() {
    QueueCmd(std::make_shared<GLEngineCmd_ClearTextures>());
}
void GLEngineProxy::EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    auto obj = std::make_shared<GLEngineCmd_EmulateBitBlt>();
    obj->mXDest = nXDest;
    obj->mYDest = nYDest;
    obj->mWidthDest = nWidth;
    obj->mHeightDest = nHeight;
    obj->mXSrc = nXSrc;
    obj->mYSrc = nYSrc;
    obj->mWidthSrc = nWidth;
    obj->mHeightSrc = nHeight;

    obj->mHdcSrc = hdcSrc;
    obj->mRop = dwRop;
    QueueCmd(obj);
}
void GLEngineProxy::RenderCameraToScreen(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
    DWORD dwRop)
{
    auto obj = std::make_shared<GLEngineCmd_RenderCameraToScreen>();
    obj->mXDest = nXOriginDest;
    obj->mYDest = nYOriginDest;
    obj->mWidthDest = nWidthDest;
    obj->mHeightDest = nHeightDest;
    obj->mXSrc = nXOriginSrc;
    obj->mYSrc = nYOriginSrc;
    obj->mWidthSrc = nWidthSrc;
    obj->mHeightSrc = nHeightSrc;

    obj->mHdcDest = hdcDest;
    obj->mHdcSrc = hdcSrc;
    obj->mRop = dwRop;
    QueueCmd(obj);
}

void GLEngineProxy::EndFrame(HDC hdcDest)
{
    auto obj = std::make_shared<GLEngineCmd_EndFrame>();
    obj->mHdcDest = hdcDest;
    QueueCmd(obj);
}

void GLEngineProxy::InitForHDC(HDC hdcDest)
{
    auto obj = std::make_shared<GLEngineCmd_InitForHDC>();
    obj->mHdcDest = hdcDest;
    QueueCmd(obj);
}

void GLEngineProxy::CheckRendererInit(void)
{
    static bool ranEarlyInit = false;
    if ((!ranEarlyInit) && g_GLEngine.IsEnabled())
    {
        void* mainFrmPtr = *((void**)0xB25010);
        if (mainFrmPtr != nullptr)
        {
            auto frmGetHDC = (HRESULT(__stdcall *)(void*, HDC*)) *(void**)(*(uintptr_t*)mainFrmPtr + 0xD8);
            HDC targetHdc = nullptr;
            frmGetHDC(mainFrmPtr, &targetHdc);

            if (targetHdc != nullptr) {
                //static char foo[256];
                //sprintf(foo, "Early GL Init: 0x%08x", targetHdc);
                //dbgboxA(foo);
                ranEarlyInit = true;
                g_GLEngine.InitForHDC(targetHdc);
            }
        }
    }
}
