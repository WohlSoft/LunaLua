#include "../../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../Rendering/RenderOps/RenderBitmapOp.h"
#include "../../../Rendering/RenderOps/RenderSpriteOp.h"
#include "../../../Rendering/RenderOps/RenderStringOp.h"
#include "../../../SMBXInternal/CameraInfo.h"
#include "../../../Rendering/GL/GLEngineProxy.h"
#include "../../../Rendering/SMBXMaskedImage.h"
#include "../../../SMBXInternal/HardcodedGraphicsAccess.h"
#include <luabind/adopt_policy.hpp>
#include <luabind/out_value_policy.hpp>

// Stores reference to a loaded image
LuaProxy::Graphics::LuaImageResource::LuaImageResource(const std::shared_ptr<BMPBox>& img) {
    this->img = img;
}

// Deconstructor for when a loaded image resource is no longer referenced by Lua
LuaProxy::Graphics::LuaImageResource::~LuaImageResource() {
}

int LuaProxy::Graphics::LuaImageResource::GetWidth() const {
    if (!img) return 0;
    return img->m_W;
}

int LuaProxy::Graphics::LuaImageResource::GetHeight() const {
    if (!img) return 0;
    return img->m_H;
}


uintptr_t LuaProxy::Graphics::LuaImageResource::__BMPBoxPtr()
{
    return (uintptr_t)img.get();
}


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

LuaProxy::Graphics::LuaImageResource* LuaProxy::Graphics::loadImage(const std::string& filename, lua_State* L)
{
    std::wstring full_path;

    if (!isAbsolutePath(filename)) {
        full_path = getCustomFolderPath() + Str2WStr(filename);
    }
    else
    {
        full_path = Str2WStr(filename);
    }

    std::shared_ptr<BMPBox> img = BMPBox::loadShared(full_path);

    if (!img) {
        // If image loading failed, return null
        return NULL;
    }

    // Allocate a LuaImageResource to allow us to automatically garbage collect the image when no longer referenced in Lua
    return new LuaProxy::Graphics::LuaImageResource(img);
}

luabind::object LuaProxy::Graphics::loadAnimatedImage(const std::string& filename, int& smbxFrameTime, lua_State* L)
{
    luabind::object tLuaImageResources = luabind::newtable(L);
    std::vector<std::shared_ptr<BMPBox>> frames = gLunaRender.LoadAnimatedBitmapResource(Str2WStr(filename), &smbxFrameTime);
    for (unsigned int i = 0; i < frames.size(); i++){
        tLuaImageResources[i + 1] = luabind::object(L, new LuaProxy::Graphics::LuaImageResource(frames[i]), luabind::adopt(luabind::result));
    }
    return tLuaImageResources;
}



bool LuaProxy::Graphics::loadImage(const std::string& filename, int resNumber, int transColor)
{
    return gLunaRender.LoadBitmapResource(Str2WStr(filename), resNumber, transColor);
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
    req.str = Str2WStr(extra);
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

void LuaProxy::Graphics::placeSprite(int type, const LuaProxy::Graphics::LuaImageResource& img, int xPos, int yPos, const std::string& extra, int time)
{
    CSpriteRequest req;
    req.type = type;
    req.img_resource_code = -1;
    req.direct_img = img.img;
    req.x = xPos;
    req.y = yPos;
    req.time = time;
    req.str = Str2WStr(extra);
    gSpriteMan.InstantiateSprite(&req, false);
}

void LuaProxy::Graphics::placeSprite(int type, const LuaProxy::Graphics::LuaImageResource& img, int xPos, int yPos, const std::string& extra)
{
    placeSprite(type, img.img, xPos, yPos, extra, 0);
}

void LuaProxy::Graphics::placeSprite(int type, const LuaProxy::Graphics::LuaImageResource& img, int xPos, int yPos)
{
    placeSprite(type, img.img, xPos, yPos, "");
}


void LuaProxy::Graphics::unplaceSprites(const LuaImageResource& img, int xPos, int yPos)
{
    gSpriteMan.ClearSprites(img.img, xPos, yPos);
}

void LuaProxy::Graphics::unplaceSprites(const LuaImageResource& img)
{
    gSpriteMan.ClearSprites(img.img);
}

luabind::object LuaProxy::Graphics::getPixelData(const LuaImageResource& img, int& width, int& height, lua_State *L)
{
    if (!img.img || !img.img->ImageLoaded()){
        luaL_error(L, "Internal error: Failed to find image resource!");
        return luabind::object();
    }

    luabind::object returnTable = luabind::newtable(L);
    int i = 1;
    img.img->forEachPixelValue([&returnTable, &i](BYTE nextPixelValue){returnTable[i++] = nextPixelValue; });
    width = img.img->m_W;
    height = img.img->m_H;

    return returnTable;
}


void LuaProxy::Graphics::drawImage(const LuaImageResource& img, double xPos, double yPos, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const LuaImageResource& img, double xPos, double yPos, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, double xPos, double yPos, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, double xPos, double yPos, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, false, priority, L);
}



void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, double xPos, double yPos, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, double xPos, double yPos, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, true, priority, L);
}

void LuaProxy::Graphics::drawImageGeneric(const LuaImageResource& img, double xPos, double yPos, double sourceX, double sourceY, double sourceWidth, double sourceHeight, float opacity, bool sceneCoords, double priority, lua_State* L)
{
    if (priority < RENDEROP_PRIORITY_MIN || priority > RENDEROP_PRIORITY_MAX) {
        luaL_error(L, "Priority value is not valid (must be between %f and %f, got %f).", RENDEROP_PRIORITY_MIN, RENDEROP_PRIORITY_MAX, priority);
        return;
    }

    RenderBitmapOp* renderOp = new RenderBitmapOp();
    renderOp->direct_img = img.img;
    renderOp->x = xPos;
    renderOp->y = yPos;
    renderOp->sx = (sourceX <= 0.0 ? 0.0 : sourceX);
    renderOp->sy = (sourceY <= 0.0 ? 0.0 : sourceY);
    renderOp->sw = (sourceWidth <= 0.0 ? static_cast<double>(img.img->m_W) : sourceWidth);
    renderOp->sh = (sourceHeight <= 0.0 ? static_cast<double>(img.img->m_H) : sourceHeight);
    renderOp->opacity = opacity;
    renderOp->sceneCoords = sceneCoords;
    renderOp->m_renderPriority = priority; 

    gLunaRender.AddOp(renderOp);
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
        strRenderOp->m_String = Str2WStr(text);
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
        
        LuaImageResource* rgbaImage = nullptr;
        SMBXMaskedImage* maskedImage = nullptr;
        double sourceX;
        double sourceY;
        double sourceWidth;
        double sourceHeight;
        float opacity;
        
        {
            LuaImageResource* image;
            LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_NOERROR(namedArgs, image, nullptr);
            rgbaImage = image;
        }
        if (rgbaImage == nullptr) {
            SMBXMaskedImage* image;
            LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, image);
            maskedImage = image;
        }

        if ((!rgbaImage || !rgbaImage->img || !rgbaImage->img->ImageLoaded()) && (!maskedImage)) {
            luaL_error(L, "Image may not be nil.");
            return;
        }

        int defW = 0, defH = 0;
        if (rgbaImage != nullptr)
        {
            defW = rgbaImage->img->m_W;
            defH = rgbaImage->img->m_H;
        }
        else if (maskedImage != nullptr)
        {
            maskedImage->getSize(defW, defH);
        }

        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceX, 0.0);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceY, 0.0);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceWidth, static_cast<double>(defW));
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceHeight, static_cast<double>(defH));
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, opacity, 1.0f);
        
        // Exit fast if opacity is 0
        if (opacity == 0.0f) return;

        if (rgbaImage != nullptr)
        {
            RenderBitmapOp* bitmapRenderOp = new RenderBitmapOp();
            bitmapRenderOp->direct_img = rgbaImage->img;
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
        else if (maskedImage != nullptr)
        {
            if (opacity != 1.0f)
            {
                luaL_error(L, "Opacity cannot be used for masked image rendering");
                return;
            }

            RenderSpriteOp* maskedRenderOp = new RenderSpriteOp();
            maskedRenderOp->sprite = maskedImage;
            maskedRenderOp->sx = sourceX;
            maskedRenderOp->sy = sourceY;
            maskedRenderOp->sw = sourceWidth;
            maskedRenderOp->sh = sourceHeight;
            maskedRenderOp->x = x;
            maskedRenderOp->y = y;
            maskedRenderOp->sceneCoords = isSceneCoordinates;
            renderOperation = maskedRenderOp;
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
        gLunaRender.AddOp(renderOperation);
    }
}



bool LuaProxy::Graphics::isOpenGLEnabled()
{
    return g_GLEngine.IsEnabled();
}

void LuaProxy::Graphics::glSetTexture(const LuaImageResource* img, uint32_t color)
{   
    // Convert RGB to RGBA
    LuaProxy::Graphics::glSetTextureRGBA(img, (color << 8) | 0xFF);
}

void LuaProxy::Graphics::glSetTextureRGBA(const LuaImageResource* img, uint32_t color)
{
    const BMPBox* bmp = NULL;
    if (img && img->img && img->img->ImageLoaded()) {
        bmp = img->img.get(); // Get a raw pointer, because currently the BMPBox destructor tells the render thread to cut it out
    }

    auto obj = std::make_shared<GLEngineCmd_SetTexture>();
    obj->mBmp = bmp;
    obj->mColor = color;
    gLunaRender.GLCmd(obj);
}

void LuaProxy::Graphics::__glInternalDraw(const luabind::object& namedArgs, lua_State* L)
{
    double priority;
    const LuaProxy::Graphics::LuaImageResource* luaImageResource = nullptr;
    const LuaProxy::Shader* shader = nullptr;
    std::shared_ptr<CaptureBuffer> capBuff = nullptr;
    float r, g, b, a;
    unsigned int rawVer, rawTex, rawCol, rawCnt;
    unsigned int primitive;
    bool sceneCoords;
    bool depthTest;

    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, priority, RENDEROP_DEFAULT_PRIORITY_RENDEROP);
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, primitive, GL_TRIANGLES);
    {
        const LuaProxy::Graphics::LuaImageResource* texture;
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_NOERROR(namedArgs, texture, nullptr);
        luaImageResource = texture;
    }
    if (luaImageResource == nullptr) {
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

    const BMPBox* bmp = nullptr;
    if (luaImageResource && luaImageResource->img && luaImageResource->img->ImageLoaded()) {
        bmp = luaImageResource->img.get(); // Get a raw pointer, because currently the BMPBox destructor tells the render thread to cut it out
    }

    auto obj = std::make_shared<GLEngineCmd_LuaDraw>();
    obj->mBmp = bmp;
    obj->mCapBuff = capBuff;
    obj->mColor[0] = r;
    obj->mColor[1] = g;
    obj->mColor[2] = b;
    obj->mColor[3] = a;
    obj->mType = primitive;
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
                GLenum glType;
                unsigned int count;
                LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(val, data);
                LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(val, glType);
                LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(val, count);

                // Keys
                luabind::object key = i.key();
                GLuint location;
                try {
                    location = luabind::object_cast<GLuint>(key);
                }
                catch (luabind::cast_failed&) {
                    luaL_error(L, (std::string(getGLShaderVariableTypeName(typeOfVar)) + " key is invalid (internal error)").c_str());
                    return;
                }

                // GLShaderVariableType type, GLenum typeData, size_t m_count, void* data
                mapTo.emplace_back(typeOfVar, location, glType, count, reinterpret_cast<void*>(data));
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

    gLunaRender.GLCmd(obj, priority);
}

void LuaProxy::Graphics::__setSpriteOverride(const std::string& t, int index, const luabind::object& overrideImg, lua_State* L)
{
    SMBXMaskedImage* img = SMBXMaskedImage::GetByName(t, index);
    if (img == nullptr)
    {
        luaL_error(L, "Graphics.sprite.%s[%d] does not exist", t.c_str(), index);
        return;
    }

    if (!overrideImg.is_valid())
    {
        img->UnsetOverride();
        return;
    }

    boost::optional<SMBXMaskedImage*> maskImg = luabind::object_cast_nothrow<SMBXMaskedImage*>(overrideImg);
    if (maskImg != boost::none) {
        img->SetOverride(*maskImg);
        return;
    }

    boost::optional<LuaProxy::Graphics::LuaImageResource*> rgbaImg = luabind::object_cast_nothrow<LuaProxy::Graphics::LuaImageResource*>(overrideImg);
    if (rgbaImg != boost::none) {
        if (*rgbaImg != nullptr && (*rgbaImg)->img) {
            img->SetOverride((*rgbaImg)->img);
        }
        return;
    }

    luaL_error(L, "Cannot set Graphics.sprite.%s[%d], invalid input type", t.c_str(), index);
}

void LuaProxy::Graphics::__setHardcodedSpriteOverride(const std::string & name, const luabind::object & overrideImg, lua_State * L)
{
    HDC mainHdc = nullptr, maskHdc = nullptr;
    if (name.find("hardcoded-") == 0)
    {
        // If non of the above applies, then try with the hardcoded ones:
        HardcodedGraphicsItem::GetHDCByName(name, &mainHdc, &maskHdc);
    }
    if (mainHdc == nullptr && maskHdc == nullptr) {
        luaL_error(L, "Failed to get hardcoded image!");
        return;
    }
    
    SMBXMaskedImage* img = SMBXMaskedImage::Get(mainHdc, maskHdc);
    if (!overrideImg.is_valid())
    {
        img->UnsetOverride();
        return;
    }

    boost::optional<SMBXMaskedImage*> maskImg = luabind::object_cast_nothrow<SMBXMaskedImage*>(overrideImg);
    if (maskImg != boost::none) {
        img->SetOverride(*maskImg);
        return;
    }

    boost::optional<LuaProxy::Graphics::LuaImageResource*> rgbaImg = luabind::object_cast_nothrow<LuaProxy::Graphics::LuaImageResource*>(overrideImg);
    if (rgbaImg != boost::none) {
        if (*rgbaImg != nullptr && (*rgbaImg)->img) {
            img->SetOverride((*rgbaImg)->img);
        }
        return;
    }
    luaL_error(L, "Invalid input for sprite override!");
}

luabind::object extractCurrentOverrideImage(SMBXMaskedImage* img, lua_State* L) {
    SMBXMaskedImage* maskOverride = img->GetMaskedOverride();
    if (maskOverride != nullptr)
    {
        return luabind::object(L, maskOverride);
    }

    std::shared_ptr<BMPBox> rgbaOverride = img->GetRGBAOverride();
    if (rgbaOverride)
    {
        return luabind::object(L, new LuaProxy::Graphics::LuaImageResource(rgbaOverride), luabind::adopt(luabind::result));
    }

    std::shared_ptr<BMPBox> loadedPng = img->GetLoadedPng();
    if (loadedPng)
    {
        return luabind::object(L, new LuaProxy::Graphics::LuaImageResource(loadedPng), luabind::adopt(luabind::result));
    }

    return luabind::object(L, img);
}


luabind::object LuaProxy::Graphics::__getHardcodedSpriteOverride(const std::string& name, lua_State* L)
{
    HDC mainHdc = nullptr, maskHdc = nullptr;
    if (name.find("hardcoded-") == 0)
    {
        // If non of the above applies, then try with the hardcoded ones:
        HardcodedGraphicsItem::GetHDCByName(name, &mainHdc, &maskHdc);
    }
    
    if (mainHdc == nullptr && maskHdc == nullptr) {
        luaL_error(L, "Failed to get hardcoded image!");
        return luabind::object();
    }

    return extractCurrentOverrideImage(SMBXMaskedImage::Get(mainHdc, maskHdc), L);
}


luabind::object LuaProxy::Graphics::__getSpriteOverride(const std::string& t, int index, lua_State* L)
{
    SMBXMaskedImage* img = SMBXMaskedImage::GetByName(t, index);
    if (img == nullptr)
    {
        luaL_error(L, "Graphics.sprite.%s[%d] does not exist", t.c_str(), index);
        return luabind::object();
    }
    return extractCurrentOverrideImage(img, L);
}
