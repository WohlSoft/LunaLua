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
    return (std::string)GM_INPUTSTR_BUF_PTR;
}

void LuaProxy::Misc::cheatBuffer(const luabind::object &value, lua_State* L)
{
    LuaHelper::assignVB6StrPtr(&GM_INPUTSTR_BUF_PTR, value, L);
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

template<const DWORD FILTER>
luabind::object luabindResolveFile(const std::string& file, lua_State* L){
    std::vector<std::string> paths = {
        utf8_encode(getCustomFolderPath()),
        (std::string)GM_FULLDIR,
        utf8_encode(getModulePath()) + "\\LuaScriptsLib\\",
        utf8_encode(getModulePath()) + "\\"
    };


    for (const std::string& nextSearchPath : paths) {
        if (nextSearchPath == "" || nextSearchPath == "\\")
            continue;

        std::vector<std::string> listOfFiles = listOfDir(nextSearchPath, FILTER);
        for (const std::string& nextFoundFile : listOfFiles) {
            if (nextFoundFile == file) {
                return luabind::object(L, nextSearchPath + nextFoundFile);
            }
        }
    }

    return luabind::object();
}

luabind::object LuaProxy::Misc::resolveFile(const std::string& file, lua_State* L)
{
    return luabindResolveFile<~FILE_ATTRIBUTE_DIRECTORY>(file, L);
}

luabind::object LuaProxy::Misc::resolveDirectory(const std::string& directory, lua_State* L)
{
    return luabindResolveFile<FILE_ATTRIBUTE_DIRECTORY>(directory, L);
}


