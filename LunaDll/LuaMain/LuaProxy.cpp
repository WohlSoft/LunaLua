#include "LuaProxy.h"
#include "../Rendering.h"
#include "../Globals.h"
#include "../PlayerMOB.h"


std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

void LuaProxy::windowDebug(const char *debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}

void LuaProxy::print(const char *text, int x, int y)
{
    gLunaRender.SafePrint(utf8_decode(std::string(text)), 3, (float)x, (float)y);
}


void LuaProxy::print(const char *text, int type, int x, int y)
{
    gLunaRender.SafePrint(utf8_decode(std::string(text)), type, (float)x, (float)y);
}

LuaProxy::Player::Player()
{}

int LuaProxy::Player::section()
{
    return (int)::Player::Get(1)->CurrentSection;
}

void LuaProxy::Player::kill()
{
    int tempint = 1;
    ::Player::Kill(&tempint);
}

void LuaProxy::Player::harm()
{
    int tempint = 1;
    ::Player::Harm(&tempint);
}

RECT LuaProxy::Player::screen()
{
    return ::Player::GetScreenPosition(::Player::Get(1));
}

double LuaProxy::Player::x()
{
    return ::Player::Get(1)->CurXPos;
}

void LuaProxy::Player::setX(double x)
{
    ::Player::Get(1)->CurXPos = x;
}

double LuaProxy::Player::y()
{
    return ::Player::Get(1)->CurYPos;
}

void LuaProxy::Player::setY(double y)
{
    ::Player::Get(1)->CurYPos = y;
}

double LuaProxy::Player::speedX()
{
    return ::Player::Get(1)->CurXSpeed;
}

void LuaProxy::Player::setSpeedX(double speedX)
{
    ::Player::Get(1)->CurXSpeed = speedX;
}

double LuaProxy::Player::speedY()
{
    return ::Player::Get(1)->CurYSpeed;
}

void LuaProxy::Player::setSpeedY(double speedY)
{
    ::Player::Get(1)->CurXSpeed = speedY;
}


