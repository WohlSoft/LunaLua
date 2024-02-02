#include <thread>
#include <memory>
#include "../../Defines.h"
#include "../../Misc/ThreadedCmdQueue.h"
#include "../../Misc/TestModeMenu.h"
#include "GLEngineProxy.h"
#include "GLEngineCmds.h"
#include "GLContextManager.h"
#include "../Shaders/GLShader.h"

// Instance
GLEngineProxy g_GLEngine;

GLEngineProxy::GLEngineProxy() {
    mPendingClear = 0;
    mSkipFrame = false;
    mNextEndFrameIsSkippable = true;
    mpThread = NULL;
    mFrameStats = { 0, 0 };
    mFirstFramePending = true;
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
            // Track frame stats
            FrameStatStruct frameStats = mFrameStats.load();
            if (mSkipFrame)
            {
                frameStats.skipCount++;
            }
            frameStats.totalCount++;
            mFrameStats.store(frameStats);

            mQueuedFrameSkippability.pop();

            // Determine frame skippability
            // It is skippable if there is another FrameEnd queued with the
            // flag for skippability set to true.
            bool mFrameSkippable = false;
            if (!mQueuedFrameSkippability.empty())
            {
                mFrameSkippable = mQueuedFrameSkippability.peek();
            }
            mSkipFrame = mFrameSkippable;

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
void GLEngineProxy::RenderCameraToScreen(int camIdx, double renderX, double renderY, double width, double height)
{
    auto obj = std::make_shared<GLEngineCmd_RenderCameraToScreen>();
    obj->mCamIdx = camIdx;
    obj->mRenderX = renderX;
    obj->mRenderY = renderY;
    obj->mHeight = height;
    obj->mWidth = width;
    QueueCmd(obj);
}

void GLEngineProxy::EndFrame(HDC hdcDest, bool isLoadScreen, bool redrawOnly, bool resizeOverlay, bool pauseOverlay)
{
    auto obj = std::make_shared<GLEngineCmd_EndFrame>();
    obj->mHdcDest = hdcDest;
    obj->mRedrawOnly = redrawOnly;
    obj->mResizeOverlay = resizeOverlay;
    obj->mPauseOverlay = pauseOverlay;

    if (isLoadScreen || redrawOnly)
    {
        obj->mForceSkip = false;
        obj->mIsFirstFrame = false;
    }
    else if (testModeMenuIsSkipTickPending())
    {
        // In test mode, force the flip of the frame to be skipped
        obj->mForceSkip = true;
        obj->mIsFirstFrame = false;
    }
    else
    {
        obj->mForceSkip = false;
        obj->mIsFirstFrame = mFirstFramePending;
        mFirstFramePending = false;
    }
    QueueCmd(obj);
}

void GLEngineProxy::InitForHDC(HDC hdcDest)
{
    auto obj = std::make_shared<GLEngineCmd_InitForHDC>();
    obj->mHdcDest = hdcDest;
    QueueCmd(obj);
}

HDC GLEngineProxy::GetHDC(void)
{
    void* mainFrmPtr = *((void**)0xB25010);
    if (mainFrmPtr == nullptr)
    {
        return nullptr;
    }

    auto frmGetHDC = (HRESULT(__stdcall *)(void*, HDC*)) *(void**)(*(uintptr_t*)mainFrmPtr + 0xD8);
    HDC targetHdc = nullptr;
    frmGetHDC(mainFrmPtr, &targetHdc);
    return targetHdc;
}

void GLEngineProxy::CheckRendererInit(void)
{
    static bool ranEarlyInit = false;
    if ((!ranEarlyInit) && g_GLEngine.IsEnabled())
    {
        HDC targetHdc = GetHDC();

        if (targetHdc != nullptr) {
            ranEarlyInit = true;
            g_GLEngine.InitForHDC(targetHdc);
        }
    }
}
