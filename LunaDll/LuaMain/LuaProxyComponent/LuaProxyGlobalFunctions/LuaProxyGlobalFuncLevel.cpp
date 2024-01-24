#include "../../LuaProxy.h"
#include "../../../Defines.h"
#include "../../../GlobalFuncs.h"

#include "../../../SMBXInternal/Reconstructed/EpisodeMain.h"

std::string LuaProxy::Level::filename()
{
    return (std::string)GM_LVLFILENAME_PTR;
}

std::string LuaProxy::Level::name()
{
    return (std::string)GM_LVLNAME_PTR;
}

void LuaProxy::Level::load(std::string filePath, int warpIdx)
{
    std::string filePathStrNoPath = splitPathFromFilename(filePath);
    EpisodeMain mainEpisodeFunc;
    mainEpisodeFunc.loadLevel(filePathStrNoPath, warpIdx, true);
}

void LuaProxy::Level::load(std::string filePath)
{
    LuaProxy::Level::load(filePath, 0);
}
