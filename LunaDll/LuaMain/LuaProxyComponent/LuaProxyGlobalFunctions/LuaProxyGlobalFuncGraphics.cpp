#include "../../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../Rendering/RenderOps/RenderBitmapOp.h"
#include "../../../Rendering/RenderOps/RenderStringOp.h"
#include "../../../SMBXInternal/CameraInfo.h"
#include "../../../Rendering/GLEngineProxy.h"
#include <luabind/adopt_policy.hpp>

// Stores reference to a loaded image
LuaProxy::Graphics::LuaImageResource::LuaImageResource(int imgResource) {
    this->imgResource = imgResource;
}

// Deconstructor for when a loaded image resource is no longer referenced by Lua
LuaProxy::Graphics::LuaImageResource::~LuaImageResource() {
    gLunaRender.DeleteImage(imgResource);
}

int LuaProxy::Graphics::LuaImageResource::GetWidth() {
    const auto bmpIt = gLunaRender.LoadedImages.find(imgResource);
    if (bmpIt == gLunaRender.LoadedImages.cend()) {
        return 0;
    }
    return bmpIt->second->m_W;
}

int LuaProxy::Graphics::LuaImageResource::GetHeight() {
    const auto bmpIt = gLunaRender.LoadedImages.find(imgResource);
    if (bmpIt == gLunaRender.LoadedImages.cend()) {
        return 0;
    }
    return bmpIt->second->m_H;
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
    int resNumber = gLunaRender.GetAutoImageResourceCode();
    if (resNumber == 0) return NULL;

    if (!gLunaRender.LoadBitmapResource(utf8_decode(filename), resNumber)) {
        // If image loading failed, return null
        return NULL;
    }

    // Allocate a LuaImageResource to allow us to automatically garbage collect the image when no longer referenced in Lua
    return new LuaProxy::Graphics::LuaImageResource(resNumber);
}

luabind::object LuaProxy::Graphics::loadAnimatedImage(const std::string& filename, int& smbxFrameTime, lua_State* L)
{
    luabind::object tLuaImageResources = luabind::newtable(L);
    std::vector<int> resCodes = gLunaRender.LoadAnimatedBitmapResource(utf8_decode(filename), &smbxFrameTime);
    for (unsigned int i = 0; i < resCodes.size(); i++){
        tLuaImageResources[i + 1] = luabind::object(L, new LuaProxy::Graphics::LuaImageResource(resCodes[i]), luabind::adopt(luabind::result));
    }
    return tLuaImageResources;
}



bool LuaProxy::Graphics::loadImage(const std::string& filename, int resNumber, int transColor)
{
    return gLunaRender.LoadBitmapResource(utf8_decode(filename), resNumber, transColor);
}


void LuaProxy::Graphics::placeSprite(int type, int imgResource, int xPos, int yPos, const std::string& extra, int time)
{
    CSpriteRequest req;
    req.type = type;
    req.img_resource_code = imgResource;
    req.x = xPos;
    req.y = yPos;
    req.time = time;
    req.str = utf8_decode(extra);
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
    placeSprite(type, img.imgResource, xPos, yPos, extra, time);
}

void LuaProxy::Graphics::placeSprite(int type, const LuaProxy::Graphics::LuaImageResource& img, int xPos, int yPos, const std::string& extra)
{
    placeSprite(type, img.imgResource, xPos, yPos, extra, 0);
}

void LuaProxy::Graphics::placeSprite(int type, const LuaProxy::Graphics::LuaImageResource& img, int xPos, int yPos)
{
    placeSprite(type, img.imgResource, xPos, yPos, "");
}


void LuaProxy::Graphics::unplaceSprites(const LuaImageResource& img, int xPos, int yPos)
{
    gSpriteMan.ClearSprites(img.imgResource, xPos, yPos);
}

void LuaProxy::Graphics::unplaceSprites(const LuaImageResource& img)
{
    gSpriteMan.ClearSprites(img.imgResource);
}

luabind::object LuaProxy::Graphics::getPixelData(const LuaImageResource& img, int& width, int& height, lua_State *L)
{
    luabind::object returnTable = luabind::newtable(L);
    const auto bmpIt = gLunaRender.LoadedImages.find(img.imgResource);
    if (bmpIt == gLunaRender.LoadedImages.cend()){
        luaL_error(L, "Internal error: Failed to find image resource!");
        return luabind::object();
    }

    BMPBox* imgBox = bmpIt->second;
    int i = 1;
    imgBox->forEachPixelValue([&returnTable, &i](BYTE nextPixelValue){returnTable[i++] = nextPixelValue; });
    width = imgBox->m_W;
    height = imgBox->m_H;

    return returnTable;
}


void LuaProxy::Graphics::drawImage(const LuaImageResource& img, int xPos, int yPos, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const LuaImageResource& img, int xPos, int yPos, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImage(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, false, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, int xPos, int yPos, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, int xPos, int yPos, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, false, priority, L);
}

void LuaProxy::Graphics::drawImageWP(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, false, priority, L);
}



void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, int xPos, int yPos, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, int xPos, int yPos, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToScene(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, float opacity, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, true, RENDEROP_DEFAULT_PRIORITY_RENDEROP, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, int xPos, int yPos, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, 1.0f, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, int xPos, int yPos, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, 0, 0, 0, 0, opacity, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, 1.0f, true, priority, L);
}

void LuaProxy::Graphics::drawImageToSceneWP(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, float opacity, double priority, lua_State* L)
{
    drawImageGeneric(img, xPos, yPos, sourceX, sourceY, sourceWidth, sourceHeight, opacity, true, priority, L);
}

void LuaProxy::Graphics::drawImageGeneric(const LuaImageResource& img, int xPos, int yPos, int sourceX, int sourceY, int sourceWidth, int sourceHeight, float opacity, bool sceneCoords, double priority, lua_State* L)
{
    if (priority < RENDEROP_PRIORITY_MIN || priority > RENDEROP_PRIORITY_MAX) {
        luaL_error(L, "Priority value is not valid (must be between %f and %f, got %f).", RENDEROP_PRIORITY_MIN, RENDEROP_PRIORITY_MAX, priority);
        return;
    }

    const auto bmpIt = gLunaRender.LoadedImages.find(img.imgResource);
    if (bmpIt == gLunaRender.LoadedImages.cend()){
        luaL_error(L, "Internal error: Failed to find image resource!");
        return;
    }


    BMPBox* imgBox = bmpIt->second;

    RenderBitmapOp* renderOp = new RenderBitmapOp();
    renderOp->img_resource_code = img.imgResource;
    renderOp->x = xPos;
    renderOp->y = yPos;
    renderOp->sx = (sourceX <= 0 ? 0 : sourceX);
    renderOp->sy = (sourceY <= 0 ? 0 : sourceY);
    renderOp->sw = (sourceWidth <= 0 ? imgBox->m_W : sourceWidth);
    renderOp->sh = (sourceHeight <= 0 ? imgBox->m_H : sourceHeight);
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
    

    int x, y;
    RENDER_TYPE type;
    double priority;
    RenderOp* renderOperation;
    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, x);
    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, y);
    LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, type);
    if (type == RTYPE_TEXT) 
    {
        priority = RENDEROP_DEFAULT_PRIORITY_TEXT;
        RenderStringOp* strRenderOp = new RenderStringOp();

        std::string text;
        int fontType;
        LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, text);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, fontType, 3);

        strRenderOp->m_String = utf8_decode(text);
        if (fontType == 3)
            for (std::wstring::iterator it = strRenderOp->m_String.begin(); it != strRenderOp->m_String.end(); ++it)
                *it = towupper(*it);
        strRenderOp->m_X = (int)x;
        strRenderOp->m_Y = (int)y;
        strRenderOp->m_FontType = fontType;
        renderOperation = strRenderOp;
    }
    else if (type == RTYPE_IMAGE) 
    {
        priority = RENDEROP_DEFAULT_PRIORITY_RENDEROP;
        RenderBitmapOp* bitmapRenderOp = new RenderBitmapOp();
        
        LuaImageResource* image;
        unsigned int sourceX;
        unsigned int sourceY;
        unsigned int sourceWidth;
        unsigned int sourceHeight;
        float opacity;
        bool isSceneCoordinates;

        LUAHELPER_GET_NAMED_ARG_OR_RETURN_VOID(namedArgs, image);
        const auto bmpIt = gLunaRender.LoadedImages.find(image->imgResource);
        if (bmpIt == gLunaRender.LoadedImages.cend()) {
            luaL_error(L, "Internal error: Failed to find image resource!");
            return;
        }
        BMPBox* imgBox = bmpIt->second;

        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceX, 0);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceY, 0);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceWidth, imgBox->m_W);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, sourceHeight, imgBox->m_H);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, opacity, 1.0f);
        LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, isSceneCoordinates, false);

        
        bitmapRenderOp->direct_img = bmpIt->second;
        bitmapRenderOp->sx = sourceX;
        bitmapRenderOp->sy = sourceY;
        bitmapRenderOp->sw = sourceWidth;
        bitmapRenderOp->sh = sourceHeight;
        bitmapRenderOp->x = x;
        bitmapRenderOp->y = y;
        renderOperation = bitmapRenderOp;
    }
    else
    {
        luaL_error(L, "No valid 'type'. Must be RTYPE_TEXT or RTYPE_IMAGE");
    }
    LUAHELPER_GET_NAMED_ARG_OR_DEFAULT_OR_RETURN_VOID(namedArgs, priority, priority);
 
    renderOperation->m_renderPriority = priority;
    gLunaRender.AddOp(renderOperation);
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
    if (img) {
        auto it = gLunaRender.LoadedImages.find(img->imgResource);
        if (it != gLunaRender.LoadedImages.end()) {
            bmp = it->second;
        }
    }

    auto obj = std::make_shared<GLEngineCmd_SetTexture>();
    obj->mBmp = bmp;
    obj->mColor = color;
    gLunaRender.GLCmd(obj);
}

extern "C" {
    __declspec(dllexport) float* __cdecl LunaLuaGlAllocCoords(size_t size) {
        return (float*)malloc(size * sizeof(float));
    }

    __declspec(dllexport) void __cdecl LunaLuaGlDrawTriangles(const float* vert, const float* tex, unsigned int count) {
        auto obj = std::make_shared<GLEngineCmd_Draw2DArray>();
        obj->mType = GL_TRIANGLES;
        obj->mVert = vert;
        obj->mTex = tex;
        obj->mCount = count;
        gLunaRender.GLCmd(obj);
    }
}

void LuaProxy::Graphics::__glInternalDraw(double renderPriority, const LuaImageResource* img, float r, float g, float b, float a, unsigned int vert, unsigned int tex, unsigned int color, unsigned int count)
{
    const BMPBox* bmp = NULL;
    if (img) {
        auto it = gLunaRender.LoadedImages.find(img->imgResource);
        if (it != gLunaRender.LoadedImages.end()) {
            bmp = it->second;
        }
    }

    auto obj = std::make_shared<GLEngineCmd_LuaDraw>();
    obj->mBmp = bmp;
    obj->mColor[0] = r;
    obj->mColor[1] = g;
    obj->mColor[2] = b;
    obj->mColor[3] = a;
    obj->mType = GL_TRIANGLES;
    obj->mVert = (const float*)vert;
    obj->mTex = (const float*)tex;
    obj->mVertColor = (const float*)color;
    obj->mCount = count;
    gLunaRender.GLCmd(obj, renderPriority);
}
