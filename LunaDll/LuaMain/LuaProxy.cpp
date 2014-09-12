#include "LuaProxy.h"
#include <Windows.h>


void LuaProxy::windowDebug(const char *debugText){
    MessageBoxA(0, debugText, "Debug", 0);
}
