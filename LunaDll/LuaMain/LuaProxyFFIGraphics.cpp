
#include <cstdlib>
#include <memory>
#include <mutex>

#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Rendering/GL/GLContextManager.h"
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
#include "LunaLuaMain.h"

#define FFI_EXPORT extern "C" __declspec(dllexport)

static std::mutex g_graphicsMutex;

// LuaImageResource class

FFI_EXPORT LunaImageRef* __fastcall FFI_ImageLoad(const char* filename, uint32_t* sizeOut)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (img != nullptr)
    {
        delete img;
    }
}

FFI_EXPORT uint32_t __fastcall FFI_ImageGetDataPtr(LunaImageRef* img)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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
                typedef GLShaderVariableEntry::SamplerVectorEntry::SamplerType SamplerType;
                SamplerType type = static_cast<SamplerType>(reinterpret_cast<int>(rawData[2 * j + 0]));
                switch (type)
                {
                    case SamplerType::ELunaImage:
                    {
                        auto img = (LunaImageRef*)rawData[2 * j + 1];
                        data->emplace_back(type, img ? *img : nullptr, nullptr);
                    } break;
                    case SamplerType::ECaptureBuffer:
                    case SamplerType::EDepthBuffer:
                    {
                        auto cap = (CaptureBufferRef*)rawData[2 * j + 1];
                        data->emplace_back(type, nullptr, cap ? *cap : nullptr);
                    } break;
                    default:
                    {
                        data->emplace_back(type, nullptr, nullptr);
                    } break;
                }
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
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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
    obj->mLinearFiltered = cmd->mLinearFiltered;
    if ((cmd->mNumClipPlane) > 0 && (cmd->mNumClipPlane <= 6))
    {
        obj->mNumClipPlane = cmd->mNumClipPlane;
        for (uint8_t idx = 0; idx < obj->mNumClipPlane; idx++)
        {
            obj->mClipPlane[idx][0] = cmd->mClipPlane[idx][0];
            obj->mClipPlane[idx][1] = cmd->mClipPlane[idx][1];
            obj->mClipPlane[idx][2] = cmd->mClipPlane[idx][2];
            obj->mClipPlane[idx][3] = cmd->mClipPlane[idx][3];
        }
    }

    // Ensure we have something for a sampler to sample
    if ((!obj->mImg) && (!obj->mCapBuff))
    {
        obj->mImg = LunaImage::getBlank();
    }

    // If we have vertex colors, multiply it with the other color
    if ((obj->mVertColor != nullptr) &&
        (
            (obj->mColor[0] != 1.0) ||
            (obj->mColor[1] != 1.0) || 
            (obj->mColor[2] != 1.0) || 
            (obj->mColor[3] != 1.0)
        ))
    {
        float* vertColorData = (float*)obj->mVertColor;
        for (unsigned int i = 0; i < obj->mCount; i++)
        {
            vertColorData[4*i+0] *= obj->mColor[0];
            vertColorData[4*i+1] *= obj->mColor[1];
            vertColorData[4*i+2] *= obj->mColor[2];
            vertColorData[4*i+3] *= obj->mColor[3];
        }
        obj->mColor[0] = 1.0;
        obj->mColor[1] = 1.0;
        obj->mColor[2] = 1.0;
        obj->mColor[3] = 1.0;
        obj->mVertColor = vertColorData;
    }

    FFI_ShaderObjRef* shader = cmd->mShader;
    if ((shader != nullptr) && (*shader) && (*shader)->mShader && (*shader)->mShader->isValid())
    {
        obj->mShader = (*shader)->mShader;

        GLPlaceAttributes(obj->mAttributes, GLShaderVariableType::Attribute, cmd->mAttrs, cmd->mAttrCount);
        GLPlaceAttributes(obj->mUniforms, GLShaderVariableType::Uniform, cmd->mUnifs, cmd->mUnifCount);
    }

    Renderer::Get().GLCmd(obj, cmd->mPriority);
}

// Shader FFI Calls

FFI_EXPORT FFI_ShaderObjRef* __fastcall FFI_ShaderFromStrings(const char* vertexSource, const char* fragmentSource)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (obj != nullptr)
    {
        delete obj;
    }
}

FFI_EXPORT const char* __fastcall FFI_ShaderError(FFI_ShaderObjRef* obj)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (obj == nullptr) return nullptr;
    if (!*obj) return nullptr;
    if ((*obj)->mError == false) return nullptr;

    return (*obj)->mErrorString.c_str();
}

FFI_EXPORT bool __fastcall FFI_ShaderCompile(FFI_ShaderObjRef* obj)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    static thread_local FFI_ShaderVariableInfo out;

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
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    static thread_local FFI_ShaderVariableInfo out;

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
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    return Renderer::Get().LoadBitmapResource(Str2WStr(filename), resNumber, transColor);
}

FFI_EXPORT void __cdecl FFI_SpritePlace(int type, int resNumber, LunaImageRef* img, int xPos, int yPos, const char* extra, int time)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (img == nullptr) return;
    gSpriteMan.ClearSprites(*img);
}

FFI_EXPORT void __cdecl FFI_SpriteUnplaceWithPos(LunaImageRef* img, int x, int y)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (img == nullptr) return;
    gSpriteMan.ClearSprites(*img, x, y);
}

// FFI Image Drawing Call

FFI_EXPORT void __cdecl FFI_ImageDraw(LunaImageRef* img, double x, double y, double sx, double sy, double sw, double sh, double priority, float opacity, bool sceneCoords)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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
    Renderer::Get().AddOp(bitmapRenderOp);
}

// FFI Text Drawing Call

FFI_EXPORT void __cdecl FFI_TextDraw(const char* text, int type, int x, int y, double priority, bool sceneCoords)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (text == nullptr) return;

    std::wstring wText = Str2WStr(text);

    if (type == 3)
        for (std::wstring::iterator it = wText.begin(); it != wText.end(); ++it)
            *it = towupper(*it);

    RenderStringOp* printTextOp = new RenderStringOp(wText, type, (float)x, (float)y);
    printTextOp->m_renderPriority = priority;
    printTextOp->sceneCoords = sceneCoords;
    Renderer::Get().AddOp(printTextOp);
}

// Sprites API Calls

FFI_EXPORT void __fastcall FFI_SetSpriteOverride(const char* name, LunaImageRef* img)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    ImageLoader::OverrideByName(name, img ? *img : nullptr);
    // LUNAIMAGE_TODO: Use return value to error
}

FFI_EXPORT LunaImageRef* __fastcall FFI_GetSpriteOverride(const char* name, uint32_t* sizeOut)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
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

FFI_EXPORT void __fastcall FFI_RegisterExtraSprite(const char* folderName, const char* name)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    ImageLoader::LuaRegisterExtraGfx(folderName, name);
}

// CaptureBuffer class

FFI_EXPORT CaptureBufferRef* __fastcall FFI_CaptureBuffer(uint32_t w, uint32_t h, bool nonskippable)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    std::shared_ptr<CaptureBuffer> ref = std::make_shared<CaptureBuffer>(w, h, nonskippable);
    if (ref)
    {
        // Return heap-allocated shared_ptr
        return new std::shared_ptr<CaptureBuffer>(ref);
    }
    return nullptr;
}

FFI_EXPORT void __fastcall FFI_CaptureBufferFree(CaptureBufferRef* img)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (img != nullptr)
    {
        delete img;
    }
}

FFI_EXPORT void __cdecl FFI_CaptureBufferCaptureAt(CaptureBufferRef* img, double priority)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (img != nullptr)
    {
        (*img)->CaptureAt(priority);
    }
}

FFI_EXPORT void __cdecl FFI_CaptureBufferClear(CaptureBufferRef* img, double priority)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);
    if (img != nullptr)
    {
        (*img)->Clear(priority);
    }
}

// Level HUD Control

FFI_EXPORT void __fastcall FFI_GraphicsActivateHud(bool activate)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    gSMBXHUDSettings.skip = !activate;
}

FFI_EXPORT bool __fastcall FFI_GraphicsIsHudActivated()
{
    CLunaFFILock ffiLock(__FUNCTION__);
    return !gSMBXHUDSettings.skip;
}

// Overworld HUD Control

FFI_EXPORT void __fastcall FFI_GraphicsActivateOverworldHud(int activateFlag)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    gSMBXHUDSettings.overworldHudState = (WORLD_HUD_CONTROL)activateFlag;
}

FFI_EXPORT int __fastcall FFI_GraphicsGetOverworldHudState()
{
    CLunaFFILock ffiLock(__FUNCTION__);
    return (int)gSMBXHUDSettings.overworldHudState;
}

FFI_EXPORT bool __fastcall FFI_GraphicsIsSoftwareGL()
{
    CLunaFFILock ffiLock(__FUNCTION__);
    return gStartupSettings.softwareGL;
}

FFI_EXPORT void __fastcall FFI_GraphicsGetFrameStats(GLEngineProxy::FrameStatStruct* frameStats)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    *frameStats = g_GLEngine.GetFrameStats();
}

FFI_EXPORT const GLContextManager::GLConstants* __fastcall FFI_GraphicsGetConstants()
{
    return &g_GLContextManager.Constants();
}

// Main framebuffer control
FFI_EXPORT void FFI_GraphicsSetMainFramebufferSize(int width, int height)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);

    auto obj = std::make_shared<GLEngineCmd_SetFramebufferSize>();
    obj->mWidth = width;
    obj->mHeight = height;

    // NOTE: This command is processed synchronously, avoiding the potential
    //       for race conditions.
    //       This does however mean that switching framebuffer size may cause
    //       a momentary hitch.
    g_GLEngine.QueueCmd(obj);
}

struct FBSize {
    int w;
    int h;
};
FFI_EXPORT FBSize FFI_GraphicsGetMainFramebufferSize()
{
    // Return struct by value
    return { g_GLContextManager.GetMainFBWidth(), g_GLContextManager.GetMainFBHeight() };
}

FFI_EXPORT void __fastcall FFI_RedirectCameraFB(CaptureBufferRef* fb, double startPriority, double endPriority)
{
    CLunaFFILock ffiLock(__FUNCTION__);
    std::lock_guard<std::mutex> graphicsLock(g_graphicsMutex);

    auto startCmd = std::make_shared<GLEngineCmd_RedirectCameraFB>();
    startCmd->mBuff = fb ? *fb : nullptr;
    Renderer::Get().GLCmd(startCmd, startPriority);

    auto endCmd = std::make_shared<GLEngineCmd_UnRedirectCameraFB>();
    endCmd->mStartCmd = startCmd;
    Renderer::Get().GLCmd(endCmd, endPriority);
}
