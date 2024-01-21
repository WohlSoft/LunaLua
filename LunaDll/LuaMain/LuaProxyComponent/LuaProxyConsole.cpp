#include "../LuaProxy.h"
#include "../../GlobalFuncs.h"

LuaProxy::Console::Console() :
    m_isDefaultConsole(true)
{}

void LuaProxy::Console::print(const std::string& outText)
{
    DebugPrint("%s", outText.c_str());
}

void LuaProxy::Console::println(const std::string& outText)
{
    DebugPrint("%s\n", outText.c_str());
}

void LuaProxy::Console::clear()
{
    HANDLE hStdout;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    DebugClear(hStdout);
}
