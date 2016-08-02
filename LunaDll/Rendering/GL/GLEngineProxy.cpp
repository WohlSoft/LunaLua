#include <thread>
#include <memory>
#include "../../Defines.h"
#include "../../Misc/ThreadedCmdQueue.h"
#include "GLEngineProxy.h"
#include "GLEngineCmds.h"
#include "GLContextManager.h"

// Instance
GLEngineProxy g_GLEngine;

GLEngineProxy::GLEngineProxy() {
    mFrameCount = 0;
    mPendingClear = 0;
    mSkipFrame = false;
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
        std::shared_ptr<GLEngineCmd> cmd = mQueue.pop();

        if (cmd->shouldBeSynchronous() || (mPendingClear == 0 && !mSkipFrame)) {
            GLLock lock(*this, GLLock::QueueThread);
            cmd->run(mInternalGLEngine);
        }

        if (cmd->isSmbxClearCmd()) {
            mPendingClear--;
        }
        if (cmd->isFrameEnd()) {
            if (mFrameCount-- > 1) {
                mSkipFrame = true;
            }
            else {
                mSkipFrame = false;
            }
        }

        // Upon exit command, exit the thread
        if (cmd->isExitCmd()) return;
    }
}

void GLEngineProxy::QueueCmd(const std::shared_ptr<GLEngineCmd> &cmd) {
    // Ensure we're initialized
    Init();

    if (cmd->isFrameEnd()) { 
        // Increment count of stored frames
        mFrameCount++;
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


std::shared_ptr<GLShader> GLEngineProxy::CreateNewShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
{
    GLLock lock(*this, GLLock::MainThread);

    return std::shared_ptr<GLShader>(new GLShader(name, vertexSource, fragmentSource), 
        [this](GLShader* shaderToDelete) { // TODO: We might have a better solution for deletion
        GLLock lockForDeleter(*this, GLLock::MainThread);
        delete shaderToDelete;
    });
}

void GLEngineProxy::ClearSMBXSprites() {
    QueueCmd(std::make_shared<GLEngineCmd_ClearSMBXSprites>());
}
void GLEngineProxy::ClearLunaTexture(const BMPBox& bmp) {
    auto obj = std::make_shared<GLEngineCmd_ClearLunaTexture>();
    obj->mBmp = &bmp;
    QueueCmd(obj);
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
void GLEngineProxy::DrawLunaSprite(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    const BMPBox& bmp, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc, float opacity)
{
    auto obj = std::make_shared<GLEngineCmd_LunaDrawSprite>();
    obj->mXDest = nXOriginDest;
    obj->mYDest = nYOriginDest;
    obj->mWidthDest = nWidthDest;
    obj->mHeightDest = nHeightDest;
    obj->mXSrc = nXOriginSrc;
    obj->mYSrc = nYOriginSrc;
    obj->mWidthSrc = nWidthSrc;
    obj->mHeightSrc = nHeightSrc;

    obj->mBmp = &bmp;
    obj->mOpacity = opacity;
    QueueCmd(obj);
}
void GLEngineProxy::EndFrame(HDC hdcDest)
{
    auto obj = std::make_shared<GLEngineCmd_EndFrame>();
    obj->mHdcDest = hdcDest;
    QueueCmd(obj);
}

GLLock::GLLock(GLEngineProxy& engineToLock, GLLockType type) : internal_lock(engineToLock.getLock())
{
    if (type == QueueThread) {
        g_GLContextManager.SwitchToQueueThreadCTX();
    } else if (type == MainThread) {
        g_GLContextManager.SwitchToMainThreadCTX();
    }
}
