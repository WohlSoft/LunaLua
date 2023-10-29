#include <string>
#include "../../LuaProxy.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"

std::string LuaProxy::Native::getSMBXPath()
{
    return gAppPathUTF8;
}

std::string LuaProxy::Native::getEpisodePath()
{
    return GM_FULLDIR;
}

std::string GetSavesPath(); // from LoadFile_Save.cpp
std::string LuaProxy::Native::getSavesPath()
{
    return GetSavesPath();
}

void LuaProxy::Native::simulateError(short errcode)
{
    emulateVB6Error((int)errcode);
}
