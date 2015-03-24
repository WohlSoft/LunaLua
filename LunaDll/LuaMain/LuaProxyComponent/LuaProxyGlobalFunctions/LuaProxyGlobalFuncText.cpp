#include "../../LuaProxy.h"
#include "../../../GlobalFuncs.h"
#include "../../../Globals.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Misc/RuntimeHook.h"

void LuaProxy::Text::windowDebug(const char *debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}

void LuaProxy::Text::print(const char *text, int x, int y)
{
    LuaProxy::Text::print(text, 3, x, y);
}


void LuaProxy::Text::print(const char *text, int type, int x, int y)
{
    std::wstring txt = utf8_decode(std::string(text));
    if (type == 3)
        for (wstring::iterator it = txt.begin(); it != txt.end(); ++it)
            *it = towupper(*it);

    gLunaRender.SafePrint(txt, type, (float)x, (float)y);
}


void LuaProxy::Text::showMessageBox(const std::string &text)
{
    showSMBXMessageBox(text);
}