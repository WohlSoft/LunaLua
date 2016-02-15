#include <string>
#include "../../LuaProxy.h"
#include "../../../Misc/RuntimeHook.h"
#include "../../../GlobalFuncs.h"

std::string LuaProxy::Native::getSMBXPath()
{
    return WStr2Str(std::wstring(getModulePath()));
}

std::string LuaProxy::Native::getWorldPath()
{
	VB6StrPtr smbxWorldPath = (VB6StrPtr)GM_FULLDIR;
	std::string buf;

	buf = smbxWorldPath;

	return buf;
}

void LuaProxy::Native::simulateError(short errcode)
{
    emulateVB6Error((int)errcode);
}