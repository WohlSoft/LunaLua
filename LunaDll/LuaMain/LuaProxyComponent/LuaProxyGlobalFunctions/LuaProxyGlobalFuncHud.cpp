#include "../../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../../Rendering/Rendering.h"


void LuaProxy::Hud::activate(bool activate)
{
    gSkipSMBXHUD = !activate;
}

bool LuaProxy::Hud::loadImage(const char* filename, int resNumber, int transColor)
{
    return gLunaRender.LoadBitmapResource(utf8_decode(std::string(filename)), resNumber, transColor);
}


void LuaProxy::Hud::placeSprite(int type, int imgResource, int xPos, int yPos, const char *extra, int time)
{
    CSpriteRequest req;
    req.type = type;
    req.img_resource_code = imgResource;
    req.x = xPos;
    req.y = yPos;
    req.time = time;
    req.str = utf8_decode(std::string(extra));
    gSpriteMan.InstantiateSprite(&req, false);
}

void LuaProxy::Hud::placeSprite(int type, int imgResource, int xPos, int yPos, const char *extra)
{
    placeSprite(type, imgResource, xPos, yPos, extra, 0);
}


void LuaProxy::Hud::placeSprite(int type, int imgResource, int xPos, int yPos)
{
    placeSprite(type, imgResource, xPos, yPos, "");
}