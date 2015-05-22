#include "../../LuaProxy.h"
#include "../../../Defines.h"
#include "../../../GlobalFuncs.h"


void LuaProxy::Misc::npcToCoins()
{
    native_npcToCoins();
}

void LuaProxy::Misc::doPOW()
{
    native_doPow();
}


std::string LuaProxy::Misc::cheatBuffer()
{
    return *(VB6StrPtr*)&(GM_INPUTSTR_BUF_PTR);
}

void LuaProxy::Misc::cheatBuffer(const luabind::object &value, lua_State* L)
{
    LuaHelper::assignVB6StrPtr((VB6StrPtr*)&(GM_INPUTSTR_BUF_PTR), value, L);
    VB6StrPtr triggerStr = " ";
    native_updateCheatbuf(&triggerStr);
}


luabind::object LuaProxy::Misc::listFiles(const std::string& path, lua_State* L)
{
    luabind::object theList = luabind::newtable(L);
    std::string modulePath = path;
    if (!isAbsolutePath(path))
    {
        modulePath = utf8_encode(getModulePath());
        modulePath += "\\";
        modulePath += path;
    }
    std::vector<std::string> listedFiles = listFilesOfDir(modulePath);
    for (unsigned int i = 0; i < listedFiles.size(); ++i){
        theList[i + 1] = listedFiles[i];
    }
    return theList;

}


luabind::object LuaProxy::Misc::listLocalFiles(std::string path, lua_State* L)
{
    return listFiles(utf8_encode(getCustomFolderPath()) + path, L);
}
