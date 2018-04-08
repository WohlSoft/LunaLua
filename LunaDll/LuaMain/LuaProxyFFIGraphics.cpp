
#include <cstdlib>
#include <memory>

#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Rendering/Rendering.h"
#include "../Rendering/RenderOps/RenderBitmapOp.h"
#include "../Rendering/RenderOps/RenderStringOp.h"
#include "../Rendering/LunaImage.h"
#include "../Rendering/ImageLoader.h"
#include "../Rendering/FrameCapture.h"
#include "../Rendering/Shaders/GLShader.h"
#include "../Rendering/Shaders/GLShaderAttributeInfo.h"
#include "../Rendering/Shaders/GLShaderUniformInfo.h"
#include "LuaProxyFFIGraphics.h"

#define FFI_EXPORT extern "C" __declspec(dllexport)

// LuaImageResource class

FFI_EXPORT LunaImageRef* __fastcall FFI_ImageLoad(const char* filename, uint32_t* sizeOut)
{
    std::wstring full_path;

    if (!isAbsolutePath(filename)) {
        full_path = getCustomFolderPath() + Str2WStr(filename);
    }
    else
    {
        full_path = Str2WStr(filename);
    }

    std::shared_ptr<LunaImage> img = LunaImage::fromFile(full_path.c_str());
    if (img)
    {
        if (sizeOut)
        {
            sizeOut[0] = img->getW();
            sizeOut[1] = img->getH();
        }

        // Return heap-allocated shared_ptr
        return new std::shared_ptr<LunaImage>(img);
    }
    return nullptr;
}

FFI_EXPORT void __fastcall FFI_ImageFree(LunaImageRef* img)
{
    if (img != nullptr)
    {
        delete img;
    }
}

FFI_EXPORT uint32_t __fastcall FFI_ImageGetDataPtr(LunaImageRef* img)
{
    if ((img != nullptr) && *img)
    {
        return (*img)->getDataPtrAsInt();
    }
    return 0;
}

// GLDraw things

static void GLPlaceAttributes(std::vector<GLShaderVariableEntry>& out, GLShaderVariableType type, FFI_GL_Draw_Var* vars, unsigned int count)
{
    for (unsigned int i = 0; i < count; i++)
    {
        auto& unif = vars[i];
        if ((unif.mType == GL_SAMPLER_2D) && (unif.mData != nullptr))
        {
            // For Sampler2D we need to convert things
            auto rawData = (void**)unif.mData;
            auto data = new GLShaderVariableEntry::SamplerVector();
            for (unsigned int j = 0; j < unif.mCount; j++)
            {
                auto img = (LunaImageRef*)rawData[2*j + 0];
                auto cap = (CaptureBufferRef*)rawData[2*j + 1];
                data->emplace_back(img ? *img : nullptr, cap ? *cap : nullptr);
            }
            free(unif.mData);

            out.emplace_back(type, unif.mId, unif.mType, unif.mCount, nullptr, data);
        }
        else
        {
            out.emplace_back(type, unif.mId, unif.mType, unif.mCount, unif.mData);
        }
    }
}

FFI_EXPORT void __fastcall FFI_GLDraw(const FFI_GL_Draw_Cmd* cmd)
{
    if (cmd == nullptr) return;

    auto obj = std::make_shared<GLEngineCmd_LuaDraw>();
    obj->mTarget = (cmd->mTarget != nullptr) ? (*cmd->mTarget) : nullptr;
    obj->mImg = (cmd->mImg != nullptr) ? (*cmd->mImg) : nullptr;
    obj->mCapBuff = (cmd->mCap != nullptr) ? (*cmd->mCap) : nullptr;
    obj->mColor[0] = cmd->mColor[0];
    obj->mColor[1] = cmd->mColor[1];
    obj->mColor[2] = cmd->mColor[2];
    obj->mColor[3] = cmd->mColor[3];
    obj->mType = cmd->mType;
    obj->mVert = cmd->mVert;
    obj->mTex = cmd->mTex;
    obj->mVertColor = cmd->mVertColor;
    obj->mCount = cmd->mCount;
    obj->mSceneCoords = cmd->mSceneCoords;
    obj->mDepthTest = cmd->mDepthTest;

    FFI_ShaderObjRef* shader = cmd->mShader;
    if ((shader != nullptr) && (*shader) && (*shader)->mShader && (*shader)->mShader->isValid())
    {
        obj->mShader = (*shader)->mShader;

        GLPlaceAttributes(obj->mAttributes, GLShaderVariableType::Attribute, cmd->mAttrs, cmd->mAttrCount);
        GLPlaceAttributes(obj->mUniforms, GLShaderVariableType::Uniform, cmd->mUnifs, cmd->mUnifCount);
    }

    gLunaRender.GLCmd(obj, cmd->mPriority);
}

// Shader FFI Calls

FFI_EXPORT FFI_ShaderObjRef* __fastcall FFI_ShaderFromStrings(const char* vertexSource, const char* fragmentSource)
{
    if (vertexSource && fragmentSource)
    {
        FFI_ShaderObjRef obj = std::make_shared<FFI_ShaderObj>();
        if (obj)
        {
            obj->mShader = nullptr;
            obj->mVertexSource = vertexSource;
            obj->mFragmentSource = fragmentSource;
            obj->mError = false;
            obj->mErrorString = "";
            obj->mUniformInfo.clear();
            obj->mAttributeInfo.clear();

            // Return heap-allocated shared_ptr
            return new std::shared_ptr<FFI_ShaderObj>(obj);
        }
    }
    return nullptr;
}

FFI_EXPORT void __fastcall FFI_ShaderFree(FFI_ShaderObjRef* obj)
{
    if (obj != nullptr)
    {
        delete obj;
    }
}

FFI_EXPORT const char* __fastcall FFI_ShaderError(FFI_ShaderObjRef* obj)
{
    if (obj == nullptr) return nullptr;
    if (!*obj) return nullptr;
    if ((*obj)->mError == false) return nullptr;

    return (*obj)->mErrorString.c_str();
}

FFI_EXPORT bool __fastcall FFI_ShaderCompile(FFI_ShaderObjRef* obj)
{
    // Invalid pointer or error condition, early return
    if (obj == nullptr) return false;
    if (!*obj) return false;
    if ((*obj)->mError) return false;

    // Early return for already being compiled
    if (((*obj)->mShader) && ((*obj)->mShader->isValid())) return true;

    // Run shader compile in GL thread (this is a synchronous command)
    auto cmd = std::make_shared<GLEngineCmd_CompileShaderObj>();
    cmd->mShaderObj = *obj;
    g_GLEngine.QueueCmd(cmd);

    return ((!(*obj)->mError) && ((*obj)->mShader) && ((*obj)->mShader->isValid()));
}

FFI_EXPORT FFI_ShaderVariableInfo* __fastcall FFI_GetAttributeInfo(FFI_ShaderObjRef* obj, uint32_t idx)
{
    static FFI_ShaderVariableInfo out;

    // Invalid pointer or error condition, early return
    if (obj == nullptr) return nullptr;
    if (!*obj) return nullptr;
    if ((*obj)->mError) return nullptr;
    if (!(((*obj)->mShader) && ((*obj)->mShader->isValid()))) return nullptr;
    if (idx >= (*obj)->mAttributeInfo.size()) return nullptr;

    auto& info = (*obj)->mAttributeInfo[idx];

    out.varInfoType = static_cast<int>(info.getVarType());
    out.id = info.getId();
    out.arrayCount = info.arrayCount();
    out.type = info.getType();
    out.name = info.getNamePtr();
    out.rawName = info.getRawNamePtr();
    out.arrayDepth = info.getArrayDepth();

    return &out;
}

FFI_EXPORT FFI_ShaderVariableInfo* __fastcall FFI_GetUniformInfo(FFI_ShaderObjRef* obj, uint32_t idx)
{
    static FFI_ShaderVariableInfo out;

    // Invalid pointer or error condition, early return
    if (obj == nullptr) return nullptr;
    if (!*obj) return nullptr;
    if ((*obj)->mError) return nullptr;
    if (!(((*obj)->mShader) && ((*obj)->mShader->isValid()))) return nullptr;
    if (idx >= (*obj)->mUniformInfo.size()) return nullptr;

    auto& info = (*obj)->mUniformInfo[idx];

    out.varInfoType = static_cast<int>(info.getVarType());
    out.id = info.getId();
    out.arrayCount = info.arrayCount();
    out.type = info.getType();
    out.name = info.getNamePtr();
    out.rawName = info.getRawNamePtr();
    out.arrayDepth = info.getArrayDepth();

    return &out;
}

// Deprecated sprite calls

FFI_EXPORT bool __fastcall FFI_SpriteImageLoad(const char* filename, int resNumber, int transColor)
{
    return gLunaRender.LoadBitmapResource(Str2WStr(filename), resNumber, transColor);
}

FFI_EXPORT void __cdecl FFI_SpritePlace(int type, int resNumber, LunaImageRef* img, int xPos, int yPos, const char* extra, int time)
{
    if ((resNumber == -1) && (img == nullptr)) return;

    CSpriteRequest req;
    req.type = type;
    req.img_resource_code = resNumber;
    req.direct_img = (img != nullptr) ? (*img) : nullptr;
    req.x = xPos;
    req.y = yPos;
    req.time = time;
    req.str = extra ? Str2WStr(extra) : L"";
    gSpriteMan.InstantiateSprite(&req, false);
}

FFI_EXPORT void __fastcall FFI_SpriteUnplace(LunaImageRef* img)
{
    if (img == nullptr) return;
    gSpriteMan.ClearSprites(*img);
}

FFI_EXPORT void __cdecl FFI_SpriteUnplaceWithPos(LunaImageRef* img, int x, int y)
{
    if (img == nullptr) return;
    gSpriteMan.ClearSprites(*img, x, y);
}

// FFI Image Drawing Call

FFI_EXPORT void __cdecl FFI_ImageDraw(LunaImageRef* img, double x, double y, double sx, double sy, double sw, double sh, double priority, float opacity, bool sceneCoords)
{
    if (img == nullptr) return;
    RenderBitmapOp* bitmapRenderOp = new RenderBitmapOp();
    bitmapRenderOp->direct_img = *img;
    bitmapRenderOp->x = x;
    bitmapRenderOp->y = y;
    bitmapRenderOp->sx = sx;
    bitmapRenderOp->sy = sy;
    bitmapRenderOp->sw = sw;
    bitmapRenderOp->sh = sh;
    bitmapRenderOp->m_renderPriority = priority;
    bitmapRenderOp->opacity = opacity;
    bitmapRenderOp->sceneCoords = sceneCoords;
    gLunaRender.AddOp(bitmapRenderOp);
}

// FFI Text Drawing Call

FFI_EXPORT void __cdecl FFI_TextDraw(const char* text, int type, int x, int y, double priority, short sceneCoords)
{
    if (text == nullptr) return;

    std::wstring wText = Str2WStr(text);

    if (type == 3)
        for (std::wstring::iterator it = wText.begin(); it != wText.end(); ++it)
            *it = towupper(*it);

    RenderStringOp* printTextOp = new RenderStringOp(wText, type, (float)x, (float)y);
    printTextOp->m_renderPriority = priority;
	printTextOp->sceneCoords = (sceneCoords != 0);
    gLunaRender.AddOp(printTextOp);
}

// Sprites API Calls

FFI_EXPORT void __fastcall FFI_SetSpriteOverride(const char* name, LunaImageRef* img)
{
    ImageLoader::OverrideByName(name, img ? *img : nullptr);
    // LUNAIMAGE_TODO: Use return value to error
}

FFI_EXPORT LunaImageRef* __fastcall FFI_GetSpriteOverride(const char* name, uint32_t* sizeOut)
{
    std::shared_ptr<LunaImage> img = ImageLoader::GetByName(name);

    if (img)
    {
        if (sizeOut)
        {
            sizeOut[0] = img->getW();
            sizeOut[1] = img->getH();
        }

        return new std::shared_ptr<LunaImage>(img);
    }
    else
    {
        return nullptr;
    }
}

// CaptureBuffer class

FFI_EXPORT CaptureBufferRef* __fastcall FFI_CaptureBuffer(uint32_t w, uint32_t h)
{ 
    std::shared_ptr<CaptureBuffer> ref = std::make_shared<CaptureBuffer>(w, h);
    if (ref)
    {
        // Return heap-allocated shared_ptr
        return new std::shared_ptr<CaptureBuffer>(ref);
    }
    return nullptr;
}

FFI_EXPORT void __fastcall FFI_CaptureBufferFree(CaptureBufferRef* img)
{
    if (img != nullptr)
    {
        delete img;
    }
}

FFI_EXPORT void __cdecl FFI_CaptureBufferCaptureAt(CaptureBufferRef* img, double priority)
{
    if (img != nullptr)
    {
        (*img)->CaptureAt(priority);
    }
}

FFI_EXPORT void __cdecl FFI_CaptureBufferClear(CaptureBufferRef* img, double priority)
{
    if (img != nullptr)
    {
        (*img)->Clear(priority);
    }
}

// Level HUD Control

FFI_EXPORT void __fastcall FFI_GraphicsActivateHud(bool activate)
{
    gSMBXHUDSettings.skip = !activate;
}

FFI_EXPORT bool __fastcall FFI_GraphicsIsHudActivated()
{
    return !gSMBXHUDSettings.skip;
}

// Overworld HUD Control

FFI_EXPORT void __fastcall FFI_GraphicsActivateOverworldHud(int activateFlag)
{
    gSMBXHUDSettings.overworldHudState = (WORLD_HUD_CONTROL)activateFlag;
}

FFI_EXPORT int __fastcall FFI_GraphicsGetOverworldHudState()
{
    return (int)gSMBXHUDSettings.overworldHudState;
}

