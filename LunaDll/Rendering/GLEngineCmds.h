#ifndef GLEngineCmds__hhhh
#define GLEngineCmds__hhhh

#include <windows.h>
#include "BMPBox.h"

struct GLEngineCmd {
    enum GLEngineCmdType {
        GL_ENGINE_CMD_CLEAR_SMBX_TEXTURES,
        GL_ENGINE_CMD_CLEAR_LUNA_TEXTURE,
        GL_ENGINE_CMD_EMULATE_BITBLT,
        GL_ENGINE_CMD_EMULATE_STRETCHBLT,
        GL_ENGINE_CMD_DRAW_LUNA_SPRITE,
        GL_ENGINE_CMD_END_FRAME,
        GL_ENGINE_CMD_EXIT
    };

    GLEngineCmdType mCmd;
    union {
        struct {
            const BMPBox* bmp;
        } mClearLunaTexture;
        struct {
            int nXDest;
            int nYDest;
            int nWidth;
            int nHeight;
            HDC hdcSrc;
            int nXSrc;
            int nYSrc;
            DWORD dwRop;
        } mBitBlt;
        struct {
            HDC hdcDest;
            int nXOriginDest;
            int nYOriginDest;
            int nWidthDest;
            int nHeightDest;
            HDC hdcSrc;
            int nXOriginSrc;
            int nYOriginSrc;
            int nWidthSrc;
            int nHeightSrc;
            DWORD dwRop;
        } mStretchBlt;
        struct {
            int nXOriginDest;
            int nYOriginDest;
            int nWidthDest;
            int nHeightDest;
            const BMPBox* bmp;
            int nXOriginSrc;
            int nYOriginSrc;
            int nWidthSrc;
            int nHeightSrc;
        } mLunaSprite;
        struct {
            HDC hdcDest;
        } mEndFrame;
    } mData;

    static inline GLEngineCmd ClearSMBXTextures() {
        GLEngineCmd cmd;

        cmd.mCmd = GLEngineCmd::GL_ENGINE_CMD_CLEAR_SMBX_TEXTURES;

        return cmd;
    }

    static inline GLEngineCmd ClearLunaTexture(const BMPBox& bmp) {
        GLEngineCmd cmd;

        cmd.mCmd = GLEngineCmd::GL_ENGINE_CMD_CLEAR_LUNA_TEXTURE;
        cmd.mData.mClearLunaTexture.bmp = &bmp;
        
        return cmd;
    }

    static inline GLEngineCmd EmulatedBitBlt(int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
    {
        GLEngineCmd cmd;

        cmd.mCmd = GLEngineCmd::GL_ENGINE_CMD_EMULATE_BITBLT;
        cmd.mData.mBitBlt.nXDest = nXDest;
        cmd.mData.mBitBlt.nYDest = nYDest;
        cmd.mData.mBitBlt.nWidth = nWidth;
        cmd.mData.mBitBlt.nHeight = nHeight;
        cmd.mData.mBitBlt.hdcSrc = hdcSrc;
        cmd.mData.mBitBlt.nXSrc = nXSrc;
        cmd.mData.mBitBlt.nYSrc = nYSrc;
        cmd.mData.mBitBlt.dwRop = dwRop;

        return cmd;
    }

    static inline GLEngineCmd EmulatedStretchBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        HDC hdcSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,
        DWORD dwRop)
    {
        GLEngineCmd cmd;

        cmd.mCmd = GLEngineCmd::GL_ENGINE_CMD_EMULATE_STRETCHBLT;
        cmd.mData.mStretchBlt.hdcDest = hdcDest;
        cmd.mData.mStretchBlt.nXOriginDest = nXOriginDest;
        cmd.mData.mStretchBlt.nYOriginDest = nYOriginDest;
        cmd.mData.mStretchBlt.nWidthDest = nWidthDest;
        cmd.mData.mStretchBlt.nHeightDest = nHeightDest;
        cmd.mData.mStretchBlt.hdcSrc = hdcSrc;
        cmd.mData.mStretchBlt.nXOriginSrc = nXOriginSrc;
        cmd.mData.mStretchBlt.nYOriginSrc = nYOriginSrc;
        cmd.mData.mStretchBlt.nWidthSrc = nWidthSrc;
        cmd.mData.mStretchBlt.nHeightSrc = nHeightSrc;
        cmd.mData.mStretchBlt.dwRop = dwRop;

        return cmd;
    }

    static inline GLEngineCmd DrawLunaSprite(int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
        const BMPBox& bmp, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc)
    {
        GLEngineCmd cmd;

        cmd.mCmd = GLEngineCmd::GL_ENGINE_CMD_DRAW_LUNA_SPRITE;
        cmd.mData.mLunaSprite.nXOriginDest = nXOriginDest;
        cmd.mData.mLunaSprite.nYOriginDest = nYOriginDest;
        cmd.mData.mLunaSprite.nWidthDest = nWidthDest;
        cmd.mData.mLunaSprite.nHeightDest = nHeightDest;
        cmd.mData.mLunaSprite.bmp = &bmp;
        cmd.mData.mLunaSprite.nXOriginSrc = nXOriginSrc;
        cmd.mData.mLunaSprite.nYOriginSrc = nYOriginSrc;
        cmd.mData.mLunaSprite.nWidthSrc = nWidthSrc;
        cmd.mData.mLunaSprite.nHeightSrc = nHeightSrc;

        return cmd;
    }

    static inline GLEngineCmd EndFrame(HDC hdcDest)
    {
        GLEngineCmd cmd;

        cmd.mCmd = GLEngineCmd::GL_ENGINE_CMD_END_FRAME;
        cmd.mData.mEndFrame.hdcDest = hdcDest;

        return cmd;
    }
};

#endif
