#include <thread>
#include "../Misc/ThreadedCmdQueue.h"
#include "GLEngineProxy.h"
#include "../Defines.h"
#include "GLEngineCmds.h"

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
        GLEngineCmd cmd;
        cmd.mCmd = GLEngineCmd::GL_ENGINE_CMD_EXIT;
        mQueue.push(cmd);
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
        GLEngineCmd cmd = mQueue.pop();

        RunCmd(cmd);

        // Upon exit command, exit the eng
        if (cmd.mCmd == GLEngineCmd::GL_ENGINE_CMD_EXIT) return;
    }
}

void GLEngineProxy::RunCmd(const GLEngineCmd& cmd) {
    switch (cmd.mCmd) {
    case GLEngineCmd::GL_ENGINE_CMD_CLEAR_SMBX_TEXTURES:
        mGLEngine.ClearSMBXTextures();
        mPendingClear--;
        break;
    case GLEngineCmd::GL_ENGINE_CMD_CLEAR_LUNA_TEXTURE:
        mGLEngine.ClearLunaTexture(*cmd.mData.mClearLunaTexture.bmp);
        break;
    case GLEngineCmd::GL_ENGINE_CMD_EMULATE_BITBLT:
        if (mPendingClear == 0 && !mSkipFrame) {
            mGLEngine.EmulatedBitBlt(
                cmd.mData.mBitBlt.nXDest, cmd.mData.mBitBlt.nYDest,
                cmd.mData.mBitBlt.nWidth, cmd.mData.mBitBlt.nHeight,
                cmd.mData.mBitBlt.hdcSrc,
                cmd.mData.mBitBlt.nXSrc, cmd.mData.mBitBlt.nYSrc,
                cmd.mData.mBitBlt.dwRop);
        }
        break;
    case GLEngineCmd::GL_ENGINE_CMD_EMULATE_STRETCHBLT:
        if (mPendingClear == 0 && !mSkipFrame) {
            mGLEngine.EmulatedStretchBlt(
                cmd.mData.mStretchBlt.hdcDest,
                cmd.mData.mStretchBlt.nXOriginDest, cmd.mData.mStretchBlt.nYOriginDest,
                cmd.mData.mStretchBlt.nWidthDest, cmd.mData.mStretchBlt.nHeightDest,
                cmd.mData.mStretchBlt.hdcSrc,
                cmd.mData.mStretchBlt.nXOriginSrc, cmd.mData.mStretchBlt.nYOriginSrc,
                cmd.mData.mStretchBlt.nWidthSrc, cmd.mData.mStretchBlt.nHeightSrc,
                cmd.mData.mStretchBlt.dwRop);
        }
        break;
    case GLEngineCmd::GL_ENGINE_CMD_DRAW_LUNA_SPRITE:
        if (mPendingClear == 0 && !mSkipFrame) {
            mGLEngine.DrawLunaSprite(
                cmd.mData.mLunaSprite.nXOriginDest, cmd.mData.mLunaSprite.nYOriginDest,
                cmd.mData.mLunaSprite.nWidthDest, cmd.mData.mLunaSprite.nHeightDest,
                *cmd.mData.mLunaSprite.bmp,
                cmd.mData.mLunaSprite.nXOriginSrc, cmd.mData.mLunaSprite.nYOriginSrc,
                cmd.mData.mLunaSprite.nWidthSrc, cmd.mData.mLunaSprite.nHeightSrc);
        }
        break;
    case GLEngineCmd::GL_ENGINE_CMD_END_FRAME:
        if (mPendingClear == 0 && !mSkipFrame) {
            mGLEngine.EndFrame(cmd.mData.mEndFrame.hdcDest);
        }
        if (mFrameCount-- > 1) {
            mSkipFrame = true;
        } else {
            mSkipFrame = false;
        }
        break;
    case GLEngineCmd::GL_ENGINE_CMD_EXIT:
        return;


    case GLEngineCmd::GL_ENGINE_CMD_SET_TEX:
        if (mPendingClear == 0 && !mSkipFrame) {
            mGLEngine.SetTex(
                cmd.mData.mSetTex.bmp,
                cmd.mData.mSetTex.color);
        }
        break;
    case GLEngineCmd::GL_ENGINE_CMD_DRAW_TRIANGLES:
        if (mPendingClear == 0 && !mSkipFrame) {
            mGLEngine.DrawTriangles(
                cmd.mData.mDrawTriangles.vert,
                cmd.mData.mDrawTriangles.tex,
                cmd.mData.mDrawTriangles.count);
        }
        delete cmd.mData.mDrawTriangles.vert;
        delete cmd.mData.mDrawTriangles.tex;
        break;
    default:
        break;
    }
}

void GLEngineProxy::QueueCmd(const GLEngineCmd cmd) {
    // Ensure we're initialized
    Init();

    switch (cmd.mCmd) {
    case GLEngineCmd::GL_ENGINE_CMD_END_FRAME:
        // Increment count of stored frames
        mFrameCount++;
        break;
    case GLEngineCmd::GL_ENGINE_CMD_CLEAR_SMBX_TEXTURES:
        mPendingClear++;
        break;
    default:
        break;
    }

    // Push the command
    mQueue.push(cmd);

    switch (cmd.mCmd) {
    case GLEngineCmd::GL_ENGINE_CMD_CLEAR_SMBX_TEXTURES:
    case GLEngineCmd::GL_ENGINE_CMD_CLEAR_LUNA_TEXTURE:
        // Texture clear commands will be synchronous
        mQueue.waitTillEmpty();
        break;
    default:
        break;
    }
}


void GLEngineProxy::ClearSMBXTextures() {
    QueueCmd(GLEngineCmd::ClearSMBXTextures());
}
void GLEngineProxy::ClearLunaTexture(const BMPBox& bmp) {
    QueueCmd(GLEngineCmd::ClearLunaTexture(bmp));
}
void GLEngineProxy::EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
    QueueCmd(GLEngineCmd::EmulatedBitBlt(nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop));
}
void GLEngineProxy::EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
    DWORD dwRop)
{
    QueueCmd(GLEngineCmd::EmulatedStretchBlt(
        hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
        hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop));
}
void GLEngineProxy::DrawLunaSprite(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
    const BMPBox& bmp, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc)
{
    QueueCmd(GLEngineCmd::DrawLunaSprite(
        nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
        bmp, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc));
}
void GLEngineProxy::EndFrame(HDC hdcDest)
{
    QueueCmd(GLEngineCmd::EndFrame(hdcDest));
}