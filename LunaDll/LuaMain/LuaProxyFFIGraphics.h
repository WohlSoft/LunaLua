#ifndef LuaProxyFFIGraphics_hhhh
#define LuaProxyFFIGraphics_hhhh

#include <memory>
#include <string>
#include <vector>

#include "../Rendering/Shaders/GLShader.h"

class LunaImage;
class CaptureBuffer;
struct FFI_ShaderObj;

typedef std::shared_ptr<LunaImage> LunaImageRef;
typedef std::shared_ptr<CaptureBuffer> CaptureBufferRef;
typedef std::shared_ptr<FFI_ShaderObj> FFI_ShaderObjRef;

struct FFI_GL_Draw_Var
{
    unsigned int mId;
    unsigned int mType;
    unsigned int mCount;
    void* mData;
};

struct FFI_GL_Draw_Cmd
{
    LunaImageRef* mImg;
    CaptureBufferRef* mCap;
    float mColor[4];
    uint32_t mType;
    uint32_t mCount;
    bool mSceneCoords;
    bool mDepthTest;
    const float* mVert;
    const float* mTex;
    const float* mVertColor;
    FFI_ShaderObjRef* mShader;
    unsigned int mAttrCount;
    FFI_GL_Draw_Var* mAttrs;
    unsigned int mUnifCount;
    FFI_GL_Draw_Var* mUnifs;
    double mPriority;
};

struct FFI_ShaderObj
{
    std::shared_ptr<GLShader> mShader;
    std::string mVertexSource;
    std::string mFragmentSource;
    bool        mError;
    std::string mErrorString;
    std::vector<GLShaderUniformInfo> mUniformInfo;
    std::vector<GLShaderAttributeInfo> mAttributeInfo;
};

struct FFI_ShaderVariableInfo
{
    int varInfoType;
    int id;
    int arrayCount;
    unsigned int type;
    const char* name;
    const char* rawName;
    int arrayDepth;
};


#endif
