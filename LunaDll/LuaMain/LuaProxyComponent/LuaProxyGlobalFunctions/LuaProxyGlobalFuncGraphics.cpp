#include "../../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../Rendering/RenderOps/RenderBitmapOp.h"
#include "../../../Rendering/RenderOps/RenderStringOp.h"
#include "../../../SMBXInternal/CameraInfo.h"
#include "../../../Rendering/LunaImage.h"
#include "../../../Rendering/ImageLoader.h"
#include "../../../Rendering/GL/GLEngineProxy.h"
#include "../../../SMBXInternal/HardcodedGraphicsAccess.h"
#include <luabind/adopt_policy.hpp>
#include <luabind/out_value_policy.hpp>
#include "../../../Utils/EncodeUtils.h"

void LuaProxy::Graphics::activateHud(bool activate)
{
    gSMBXHUDSettings.skip = !activate;
}

bool LuaProxy::Graphics::isHudActivated()
{
    return !gSMBXHUDSettings.skip;
}

void LuaProxy::Graphics::activateOverworldHud(WORLD_HUD_CONTROL activateFlag)
{
    gSMBXHUDSettings.overworldHudState = activateFlag;
}

WORLD_HUD_CONTROL LuaProxy::Graphics::getOverworldHudState()
{
    return gSMBXHUDSettings.overworldHudState;
}

std::shared_ptr<LunaImage> LuaProxy::Graphics::loadImage(const std::string& filename, lua_State* L)
{
    std::wstring full_path;

    if (!isAbsolutePath(filename)) {
        full_path = getCustomFolderPath() + LunaLua::EncodeUtils::Str2WStr(filename);
    }
    else
    {
        full_path = LunaLua::EncodeUtils::Str2WStr(filename);
    }

    std::shared_ptr<LunaImage> img = LunaImage::fromFile(full_path.c_str());

    if (!img) {
        // If image loading failed, return null
        return nullptr;
    }

    // Allocate a LuaImageResource to allow us to automatically garbage collect the image when no longer referenced in Lua
    return img;
}

luabind::object LuaProxy::Graphics::loadAnimatedImage(const std::string& filename, int& smbxFrameTime, lua_State* L)
{
    luabind::object tLuaImageResources = luabind::newtable(L);
    /*
    std::vector<std::shared_ptr<BMPBox>> frames = Renderer::Get().LoadAnimatedBitmapResource(LunaLua::EncodeUtils::Str2WStr(filename), &smbxFrameTime);
    for (unsigned int i = 0; i < frames.size(); i++){
        tLuaImageResources[i + 1] = luabind::object(L, new LunaImage(frames[i]), luabind::adopt(luabind::result));
    }
    */
    // LUNAIMAGE_TODO: Support loadAnimatedImage
    return tLuaImageResources;
}



bool LuaProxy::Graphics::loadImage(const std::string& filename, int resNumber, int transColor)
{
    return Renderer::Get().LoadBitmapResource(LunaLua::EncodeUtils::Str2WStr(filename), resNumber, transColor);
}


void LuaProxy::Graphics::placeSprite(int type, int imgResource, int xPos, int yPos, const std::string& extra, int time)
{
    CSpriteRequest req;
    req.type = type;
    req.img_resource_code = imgResource;
    req.direct_img = nullptr;
    req.x = xPos;
    req.y = yPos;
    req.time = time;
    req.str = LunaLua::EncodeUtils::Str2WStr(extra);
    gSpriteMan.InstantiateSprite(&req, false);
}

void LuaProxy::Graphics::placeSprite(int type, int imgResource, int xPos, int yPos, const std::string& extra)
{
    placeSprite(type, imgResource, xPos, yPos, extra, 0);
}


void LuaProxy::Graphics::placeSprite(int type, int imgResource, int xPos, int yPos)
{
    placeSprite(type, imgResource, xPos, yPos, "");
}

void LuaProxy::Graphics::placeSprite(int type, const std::shared_ptr<LunaImage>& img, int xPos, int yPos, const std::string& extra, int time)
{
    CSpriteRequest req;
    req.type = type;
    req.img_resource_code = -1;
    req.direct_img = img;
    req.x = xPos;
    req.y = yPos;
    req.time = time;
    req.str = LunaLua::EncodeUtils::Str2WStr(extra);
    gSpriteMan.InstantiateSprite(&req, false);
}

void LuaProxy::Graphics::placeSprite(int type, const std::shared_ptr<LunaImage>& img, int xPos, int yPos, const std::string& extra)
{
    placeSprite(type, img, xPos, yPos, extra, 0);
}

void LuaProxy::Graphics::placeSprite(int type, const std::shared_ptr<LunaImage>& img, int xPos, int yPos)
{
    placeSprite(type, img, xPos, yPos, "");
}


void LuaProxy::Graphics::unplaceSprites(const std::shared_ptr<LunaImage>& img, int xPos, int yPos)
{
    gSpriteMan.ClearSprites(img, xPos, yPos);
}

void LuaProxy::Graphics::unplaceSprites(const std::shared_ptr<LunaImage>& img)
{
    gSpriteMan.ClearSprites(img);
}

luabind::object LuaProxy::Graphics::getPixelData(const std::shared_ptr<LunaImage>& img, int& width, int& height, lua_State *L)
{
    if (!img){
        luaL_error(L, "Internal error: Failed to find image resource!");
        return luabind::object();
    }

    luabind::object returnTable = luabind::newtable(L);
    unsigned char* data = (unsigned char*)img->getDataPtr();
    unsigned int pixelCount = img->getW() * img->getH();
    for (unsigned int i = 0; i < pixelCount; i++) {
        returnTable[i + 1] = data[i];
    }
    width = img->getW();
    height = img->getH();

    return returnTable;
}


void LuaProxy::Graphics::drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, false, priority, L);
}



void LuaProxy::Graphics::drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, true, priority, L);
}

void LuaProxy::Graphics::drawImageGeneric(const std::shared_ptr<LunaImage>& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, bool sceneCoords, double priority, lua_State* L)
{
    if (priority < RENDEROP_PRIORITY_MIN || priority > RENDEROP_PRIORITY_MAX) {
        luaL_error(L, "Priority value is not valid (must be between %f and %f, got %f).", RENDEROP_PRIORITY_MIN, RENDEROP_PRIORITY_MAX, priority);
        return;
    }

    RenderBitmapOp* renderOp = new RenderBitmapOp();
    renderOp->direct_img = img;
    renderOp->x = xPos;
    renderOp->y = yPos;
    renderOp->sx = (sourceX <= 0.0 ? 0.0 : sourceX);
    renderOp->sy = (sourceY <= 0.0 ? 0.0 : sourceY);
    renderOp->sw = (sourceWidth <= 0.0 ? static_cast<double>(img->getW()) : sourceWidth);
    renderOp->sh = (sourceHeight <= 0.0 ? static_cast<double>(img->getH()) : sourceHeight);
    renderOp->opacity = opacity;
    renderOp->sceneCoords = sceneCoords;
    renderOp->m_renderPriority = priority; 

    Renderer::Get().AddOp(renderOp);
}


void LuaProxy::Graphics::draw(const luabind::object& namedArgs, lua_State* L)
{
    if (luabind::type(namedArgs) != LUA_TTABLE) {
        luaL_error(L, "Argument #1 must be a table with named arguments!");
        return;
    }
    

    double x, y;
    RENDER_TYPE type;
    double priority;
    RenderOp* renderOperation = nullptr;
    bool isSceneCoordinates;
    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, x);
    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, y);
    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, type);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, isSceneCoordinates, false);
    if (type == RTYPE_TEXT) 
    {
        priority = RENDEROP_DEFAULT_PRIORITY_TEXT;

        std::string text;
        int fontType;
        LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, text);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, fontType, 3);

        RenderStringOp* strRenderOp = new RenderStringOp();
        strRenderOp->m_String = LunaLua::EncodeUtils::Str2WStr(text);
        if (fontType == 3)
            for (auto& nextChar : strRenderOp->m_String)
                nextChar = towupper(nextChar);
        strRenderOp->m_X = static_cast<float>(x);
        strRenderOp->m_Y = static_cast<float>(y);
        strRenderOp->m_FontType = fontType;
        strRenderOp->sceneCoords = isSceneCoordinates;
        renderOperation = strRenderOp;
    }
    else if (type == RTYPE_IMAGE) 
    {
        priority = RENDEROP_DEFAULT_PRIORITY_RENDEROP;
        
        std::shared_ptr<LunaImage> rgbaImage = nullptr;
        double sourceX;
        double sourceY;
        double sourceWidth;
        double sourceHeight;
        float opacity;
        
        {
            std::shared_ptr<LunaImage> image;
            LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_NOERROR(namedArgs, image, nullptr);
            rgbaImage = image;
        }

        if (!rgbaImage) {
            luaL_error(L, "Image may not be nil.");
            return;
        }

        int defW = 0, defH = 0;
        defW = rgbaImage->getW();
        defH = rgbaImage->getH();

        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceX, 0.0);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceY, 0.0);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceWidth, static_cast<double>(defW));
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceHeight, static_cast<double>(defH));
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, opacity, 1.0f);
        
        // Exit fast if opacity is 0
        if (opacity == 0.0f) return;

        if (rgbaImage)
        {
            RenderBitmapOp* bitmapRenderOp = new RenderBitmapOp();
            bitmapRenderOp->direct_img = rgbaImage;
            bitmapRenderOp->sx = sourceX;
            bitmapRenderOp->sy = sourceY;
            bitmapRenderOp->sw = sourceWidth;
            bitmapRenderOp->sh = sourceHeight;
            bitmapRenderOp->x = x;
            bitmapRenderOp->y = y;
            bitmapRenderOp->sceneCoords = isSceneCoordinates;
            bitmapRenderOp->opacity = opacity;
            renderOperation = bitmapRenderOp;
        }
    }
    else
    {
        luaL_error(L, "No valid 'type'. Must be RTYPE_TEXT or RTYPE_IMAGE");
        return;
    }
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, priority, priority);
 
    if (renderOperation != nullptr)
    {
        renderOperation->m_renderPriority = priority;
        Renderer::Get().AddOp(renderOperation);
    }
}



bool LuaProxy::Graphics::isOpenGLEnabled()
{
    return g_GLEngine.IsEnabled();
}

void LuaProxy::Graphics::glSetTexture(const std::shared_ptr<LunaImage>& img, uint32_t color)
{   
    // Convert RGB to RGBA
    LuaProxy::Graphics::glSetTextureRGBA(img, (color << 8) | 0xFF);
}

void LuaProxy::Graphics::glSetTextureRGBA(const std::shared_ptr<LunaImage>& img, uint32_t color)
{
    auto obj = std::make_shared<GLEngineCmd_SetTexture>();
    obj->mImg = img;
    obj->mColor = color;
    Renderer::Get().GLCmd(obj);
}

void LuaProxy::Graphics::__glInternalDraw(const luabind::object& namedArgs, lua_State* L)
{
    double priority;
    std::shared_ptr<LunaImage> luaImageResource = nullptr;
    const LuaProxy::Shader* shader = nullptr;
    std::shared_ptr<CaptureBuffer> capBuff = nullptr;
    float r, g, b, a;
    unsigned int rawVer, rawTex, rawCol, rawCnt;
    unsigned int primitive;
    bool sceneCoords;
    bool depthTest;

    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, priority, RENDEROP_DEFAULT_PRIORITY_RENDEROP);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, primitive, (unsigned int)gl::GL_TRIANGLES);
    {
        std::shared_ptr<LunaImage> texture;
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_NOERROR(namedArgs, texture, nullptr);
        luaImageResource = texture;
    }
    if (!luaImageResource) {
        std::shared_ptr<CaptureBuffer> texture;
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, texture, nullptr);
        capBuff = texture;
    }
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, rawVer, (unsigned int)nullptr);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, rawTex, (unsigned int)nullptr);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, rawCol, (unsigned int)nullptr);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, rawCnt, 0);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, r, 1.0);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, g, 1.0);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, b, 1.0);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, a, 1.0);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sceneCoords, false);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, depthTest, false);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, shader, nullptr);

    auto obj = std::make_shared<GLEngineCmd_LuaDraw>();
    obj->mImg = luaImageResource;
    obj->mCapBuff = capBuff;
    obj->mColor[0] = r;
    obj->mColor[1] = g;
    obj->mColor[2] = b;
    obj->mColor[3] = a;
    obj->mType = (gl::GLenum)primitive;
    obj->mVert = (const float*)rawVer;
    obj->mTex = (const float*)rawTex;
    obj->mVertColor = (const float*)rawCol;
    obj->mCount = rawCnt;
    obj->mSceneCoords = sceneCoords;
    obj->mDepthTest = depthTest;

    if (shader) {
        obj->mShader = shader->getInternalShader();

        bool success = false;
        auto collectVars = [L, &success](std::vector<GLShaderVariableEntry>& mapTo, const luabind::object& varTbl, GLShaderVariableType typeOfVar) -> void {
            if (!varTbl.is_valid()) // If it is nil, then just skip
            {
                success = true;
                return;
            }

            int typeVal = luabind::type(varTbl);
            if (typeVal == LUA_TNIL) {  // If it is nil, then just skip
                success = true;
                return;
            }
            if (typeVal != LUA_TTABLE) {
                luaL_error(L, (std::string(getGLShaderVariableTypeName(typeOfVar)) + " is not a table (internal error)").c_str());
                return;
            }
                
            for (luabind::iterator i(varTbl), end; i != end; ++i) {
                // i->key()
                luabind::object val = *i;

                // Values
                unsigned int data;
                unsigned int glType;
                unsigned int count;
                LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(val, data);
                LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(val, glType);
                LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(val, count);

                // Keys
                luabind::object key = i.key();
                unsigned int location;
                try {
                    location = luabind::object_cast<unsigned int>(key);
                }
                catch (luabind::cast_failed&) {
                    luaL_error(L, (std::string(getGLShaderVariableTypeName(typeOfVar)) + " key is invalid (internal error)").c_str());
                    return;
                }

                // GLShaderVariableType type, GLenum typeData, size_t m_count, void* data
                mapTo.emplace_back(typeOfVar, (gl::GLuint)location, (gl::GLenum)glType, count, reinterpret_cast<void*>(data));
            }
            success = true;
        };

        collectVars(obj->mUniforms, namedArgs["uniforms"], GLShaderVariableType::Uniform);
        if (!success)
            return;
        success = false;
        collectVars(obj->mAttributes, namedArgs["attributes"], GLShaderVariableType::Attribute);
        if (!success)
            return;
    }

    Renderer::Get().GLCmd(obj, priority);
}

void LuaProxy::Graphics::__setSpriteOverride(const std::string& t, int index, std::shared_ptr<LunaImage>* overrideImg, lua_State* L)
{
    if (overrideImg == nullptr)
    {
        ImageLoader::OverrideByName(t + "-" + std::to_string(index), nullptr);
    }
    else
    {
        ImageLoader::OverrideByName(t + "-" + std::to_string(index), *overrideImg);
    }
    // LUNAIMAGE_TODO: Use return value to error
}

void LuaProxy::Graphics::__setHardcodedSpriteOverride(const std::string & name, std::shared_ptr<LunaImage>* overrideImg, lua_State * L)
{
    if (overrideImg == nullptr)
    {
        ImageLoader::OverrideByName(name, nullptr);
    }
    else
    {
        ImageLoader::OverrideByName(name, *overrideImg);
    }
    // LUNAIMAGE_TODO: Use return value to error
}

std::shared_ptr<LunaImage> LuaProxy::Graphics::__getHardcodedSpriteOverride(const std::string& name, lua_State* L)
{
    return ImageLoader::GetByName(name);
}


std::shared_ptr<LunaImage> LuaProxy::Graphics::__getSpriteOverride(const std::string& t, int index, lua_State* L)
{
    return ImageLoader::GetByName(t + "-" + std::to_string(index));
}
