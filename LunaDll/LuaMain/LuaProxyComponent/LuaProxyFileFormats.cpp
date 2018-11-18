#include "../LuaProxy.h"
#include "../../../Globals.h"
#include "../../../GlobalFuncs.h"
#include "../../libs/PGE_File_Formats/file_formats.h"

static std::string getFullPath(const std::string &p)
{
    if (!isAbsolutePath(p))
        return WStr2Str(getEpisodeFolderPath() + Str2WStr(p));
    else
        return p;
}

LevelData LuaProxy::Formats::openLevel(const std::string &filePath, lua_State *L)
{
    LevelData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenLevelFile(full_path, data);
    return data;
}


LevelData LuaProxy::Formats::openLevelHeader(const std::string &filePath, lua_State *L)
{
    LevelData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenLevelFileHeader(full_path, data);
    return data;
}


WorldData LuaProxy::Formats::openWorld(const std::string &filePath, lua_State *L)
{
    WorldData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenWorldFile(full_path, data);
    return data;
}


WorldData LuaProxy::Formats::openWorldHeader(const std::string &filePath, lua_State *L)
{
    WorldData data;
    std::string full_path = getFullPath(filePath);
    FileFormats::OpenWorldFileHeader(full_path, data);
    return data;
}


NPCConfigFile LuaProxy::Formats::openNpcConfig(const std::string &filePath, lua_State *L)
{
    NPCConfigFile data;
    std::string full_path = getFullPath(filePath);
    FileFormats::ReadNpcTXTFileF(full_path, data);
    return data;
}
