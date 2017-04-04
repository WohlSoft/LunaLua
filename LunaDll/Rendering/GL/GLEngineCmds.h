#ifndef GLEngineCmds__hhhh
#define GLEngineCmds__hhhh

#include <windows.h>
#include <cstdint>
#include <vector>
#include <GL/glew.h>

#include "../FrameCapture.h"
#include "../Shaders/GLShaderVariableEntry.h"
#include "GLDraw.h"

class GLEngine;
class GLShader;
class LunaImage;

/****************************************************/
/* Common argumets for bitmap rendering coordinates */
/****************************************************/
struct GLBitmapRenderCoords {
    int mXDest;
    int mYDest;
    int mWidthDest;
    int mHeightDest;
    int mXSrc;
    int mYSrc;
    int mWidthSrc;
    int mHeightSrc;
};

/************************************/
/* Base class for GLEngine Commands */
/************************************/
class GLEngineCmd;
class GLEngineCmd {
public:
    GLEngineCmd() {}
    virtual ~GLEngineCmd() {}
public:
    virtual void run(GLEngine& glEngine) const = 0;
    virtual bool shouldBeSynchronous(void) const { return false; }
    virtual bool isFrameEnd(void) const { return false; }
    virtual bool isSmbxClearCmd(void) const { return false; }
    virtual bool isExitCmd(void) const { return false; }
};

/******************************/
/* Specific GLEngine Commands */
/******************************/

class GLEngineCmd_ClearTextures : public GLEngineCmd {
public:
    virtual void run(GLEngine& glEngine) const;
    virtual bool shouldBeSynchronous(void) const { return true; }
    virtual bool isSmbxClearCmd(void) const { return true; }
};
class GLEngineCmd_EmulateBitBlt : public GLEngineCmd, public GLBitmapRenderCoords { // DEPRECATED
public:
    HDC mHdcSrc;
    DWORD mRop;
    virtual void run(GLEngine& glEngine) const;
};
class GLEngineCmd_RenderCameraToScreen : public GLEngineCmd, public GLBitmapRenderCoords {
public:
    HDC mHdcDest;
    HDC mHdcSrc;
    DWORD mRop;
    virtual void run(GLEngine& glEngine) const;
};
class GLEngineCmd_EndFrame : public GLEngineCmd {
public:
    HDC mHdcDest;
    virtual void run(GLEngine& glEngine) const;
    virtual bool isFrameEnd(void) const { return true; }
};
class GLEngineCmd_InitForHDC : public GLEngineCmd {
public:
    HDC mHdcDest;
    virtual void run(GLEngine& glEngine) const;
    virtual bool shouldBeSynchronous(void) const { return true; }
};
class GLEngineCmd_DrawSprite : public GLEngineCmd, public GLBitmapRenderCoords {
public:
    std::shared_ptr<LunaImage> mImg;
    float mOpacity;
    GLDraw::RenderMode mMode;
    virtual void run(GLEngine& glEngine) const;
};
class GLEngineCmd_Exit : public GLEngineCmd {
public:
    virtual void run(GLEngine& glEngine) const;
    virtual bool isExitCmd(void) const { return true; }
};
class GLEngineCmd_SetTexture : public GLEngineCmd { // DEPRECATED
public:
    std::shared_ptr<LunaImage> mImg;
    uint32_t mColor;
    virtual void run(GLEngine& glEngine) const;
};
class GLEngineCmd_Draw2DArray : public GLEngineCmd { // DEPRECATED
public:
    GLuint mType;
    const float* mVert;
    const float* mTex;
    uint32_t mCount;
    virtual void run(GLEngine& glEngine) const;
    virtual ~GLEngineCmd_Draw2DArray() {
        if (mVert) {
            free((void*)mVert);
            mVert = NULL;
        }
        if (mTex) {
            free((void*)mTex);
            mTex = NULL;
        }
    }
};

class GLEngineCmd_LuaDraw : public GLEngineCmd {
public:
    struct LuaDrawShaderEntry {
        GLenum type;
        void* data;
    };
    
    std::shared_ptr<LunaImage> mImg;
    std::shared_ptr<CaptureBuffer> mCapBuff;
    std::shared_ptr<GLShader> mShader;
    std::vector<GLShaderVariableEntry> mAttributes;
    std::vector<GLShaderVariableEntry> mUniforms;
    float mColor[4];

    GLuint mType;
    const float* mVert;
    const float* mTex;
    const float* mVertColor;
    uint32_t mCount;
    bool mSceneCoords;
    bool mDepthTest;

    GLEngineCmd_LuaDraw() :
        mImg(nullptr),
        mCapBuff(nullptr),
        mShader(nullptr),
        mAttributes(),
        mUniforms(),
        mColor{ 0, 0, 0, 0 },
        mType(0),
        mVert(nullptr),
        mTex(nullptr),
        mVertColor(nullptr),
        mCount(0),
        mSceneCoords(false),
        mDepthTest(false)
    {}

    virtual void run(GLEngine& glEngine) const;
    virtual ~GLEngineCmd_LuaDraw() {
        if (mVert) {
            free((void*)mVert);
            mVert = NULL;
        }
        if (mTex) {
            free((void*)mTex);
            mTex = NULL;
        }
        if (mVertColor) {
            free((void*)mVertColor);
            mVertColor = NULL;
        }
    }
};

class GLEngineCmd_SetCamera : public GLEngineCmd {
public:
    double mX, mY;
    virtual void run(GLEngine& glEngine) const;
};

#endif
