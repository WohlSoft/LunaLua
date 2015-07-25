#include "../../LuaProxy.h"
#include "../../../GlobalFuncs.h"
#include "../../../Globals.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Misc/RuntimeHook.h"

void LuaProxy::Text::windowDebug(const char *debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}

void LuaProxy::Text::print(const luabind::object& toPrint, int x, int y)
{
    LuaProxy::Text::print(toPrint, 3, x, y);
}


void LuaProxy::Text::print(const luabind::object& toPrint, int type, int x, int y)
{
    lua_State* L = toPrint.interpreter();
    // FIXME: Use better method than calling the tostring function directly! (defensive programming?)
    std::wstring txt = utf8_decode(luabind::call_function<std::string>(L, "tostring", toPrint));
    
    if (type == 3)
        for (wstring::iterator it = txt.begin(); it != txt.end(); ++it)
            *it = towupper(*it);

    gLunaRender.SafePrint(txt, type, (float)x, (float)y);
}


void LuaProxy::Text::showMessageBox(const std::string &text)
{
    showSMBXMessageBox(text);
}