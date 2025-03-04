#include "../../LuaProxy.h"
#include "../../../GlobalFuncs.h"
#include "../../../Globals.h"
#include "../../../Rendering/Rendering.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../Rendering/RenderOps/RenderStringOp.h"

// Function to clear pending keyboard events before creating a dialog box
// This is to prevent a pending keyboard event from affecting the message box
static void clearKeyboardEvents()
{
    MSG msg;
    while (PeekMessageA(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE | PM_NOYIELD | PM_QS_INPUT) != 0)
    {
    }
}

void LuaProxy::Text::windowDebug(const std::string& debugText, lua_State* L)
{
    clearKeyboardEvents();

    int resultCode = LunaMsgBox::ShowA(0, debugText.c_str(), "Debug", MB_OKCANCEL);
    if (resultCode == IDCANCEL)
        luaL_error(L, "Pressed cancel on windowDebug!");
}

void LuaProxy::Text::windowDebugSimple(const std::string& debugText)
{
    clearKeyboardEvents();

    LunaMsgBox::ShowA(0, debugText.c_str(), "Debug", MB_OK);
}


void LuaProxy::Text::print(const luabind::object& toPrint, int x, int y)
{
    LuaProxy::Text::print(toPrint, 3, x, y);
}


void LuaProxy::Text::print(const luabind::object& toPrint, int type, int x, int y)
{
    printWP(toPrint, type, x, y, RENDEROP_DEFAULT_PRIORITY_TEXT);
}

void LuaProxy::Text::printWP(const luabind::object & toPrint, int x, int y, double priority)
{
    printWP(toPrint, 3, x, y, priority);
}

void LuaProxy::Text::printWP(const luabind::object & toPrint, int type, int x, int y, double priority)
{
    lua_State* L = toPrint.interpreter();
    
    if (priority < RENDEROP_PRIORITY_MIN || priority > RENDEROP_PRIORITY_MAX) {
        luaL_error(L, "Priority value is not valid (must be between %f and %f, got %f).", RENDEROP_PRIORITY_MIN, RENDEROP_PRIORITY_MAX, priority);
        return;
    }
    // FIXME: Use better method than calling the tostring function directly! (defensive programming?)
    std::wstring txt = Str2WStr(luabind::call_function<std::string>(L, "tostring", toPrint));

    if (type == 3)
        for (std::wstring::iterator it = txt.begin(); it != txt.end(); ++it)
            *it = towupper(*it);

    RenderStringOp* printTextOp = new RenderStringOp(txt, type, (float)x, (float)y);
    printTextOp->m_renderPriority = priority;
    Renderer::Get().AddOp(printTextOp);
}


void LuaProxy::Text::showMessageBox(const std::string &text)
{
    if (!gLunaLua.didOnStartRun())
    {
        // Not valid before onStart
        return;
    }
    showSMBXMessageBox(text);
}