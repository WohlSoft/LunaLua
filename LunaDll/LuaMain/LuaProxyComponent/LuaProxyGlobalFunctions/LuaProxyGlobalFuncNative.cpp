#include "../../LuaProxy.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../GlobalFuncs.h"

std::string LuaProxy::Native::getSMBXPath()
{
    return utf8_encode(wstring(getModulePath()));
}

void LuaProxy::Native::simulateError(short errcode)
{
    emulateVB6Error((int)errcode);
}