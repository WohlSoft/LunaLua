#include "../LuaProxy.h"

#include <iostream>
LuaProxy::Console::Console() :
    m_isDefaultConsole(true)
{}

void LuaProxy::Console::print(const std::string& outText)
{
    std::cout << outText;
}

void LuaProxy::Console::println(const std::string& outText)
{
    print(outText + "\n");
}

