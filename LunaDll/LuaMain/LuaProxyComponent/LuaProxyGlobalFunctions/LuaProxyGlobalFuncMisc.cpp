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
luabind::object luabindResolveFile(std::string file, lua_State* L){
    std::vector<std::string> paths = {
        utf8_encode(getCustomFolderPath()),
        (std::string)GM_FULLDIR,
        utf8_encode(getModulePath()) + "\\LuaScriptsLib\\",
        utf8_encode(getModulePath()) + "\\"
    };


    for (std::string nextSearchPath : paths) {
        if (nextSearchPath == "" || nextSearchPath == "\\")
            continue;
        
        // If the file is a whole path, then check path!
        size_t nextPos;
        if ((nextPos = file.find_first_of("\\/")) != std::string::npos) {
            bool foundValid = true;
            nextSearchPath = nextSearchPath.substr(0, nextSearchPath.size() - 1); //remove the last slash of the root search paths
            do {
                std::vector<std::string> subPaths = listOfDir(nextSearchPath, FILE_ATTRIBUTE_DIRECTORY); //Get all dirs in the next directory
                
                std::string nextPathStrip = file.substr(0, nextPos); // Get the directory to be searched on
                nextSearchPath += "\\" + nextPathStrip; // Add the strip to the current one (for the next search)

                // Try to find a match
                foundValid = false;
                for (const std::string& nextSubPath : subPaths) {
                    foundValid = nextSubPath == nextPathStrip;
                    if (foundValid)
                        break;
                }
                if (!foundValid) break; //otherwise break the search

                file = file.substr(nextPos + 1);
                nextPos = file.find_first_of("\\/"); // try to find the next folder
            } while (nextPos != std::string::npos);

            if (!foundValid) //Go to the next result
                continue;

            nextSearchPath += "\\";
        }

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


