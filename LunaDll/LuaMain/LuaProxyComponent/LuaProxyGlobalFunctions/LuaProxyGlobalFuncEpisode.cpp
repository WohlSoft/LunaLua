#include "../../LuaProxy.h"

#include "../../../Defines.h"
#include "../../../GlobalFuncs.h"
#include "../../../Globals.h"

#include "../../../Misc/VB6StrPtr.h"
#include "../../../Misc/VB6Bool.h"

#include "../../../SMBXInternal/Types.h"
#include "../../../SMBXInternal/Variables.h"
#include "../../../SMBXInternal/Functions.h"

#include "../../../SMBXInternal/Reconstructed/EpisodeMain.h"

// luabind object that retrieves every episode from the world list, and creates a table from what was retrieved
static luabind::object getAllEpisodes(lua_State *L)
{
    using namespace SMBX13;

    luabind::object outData = luabind::newtable(L);
    {
        size_t counter = 0;

        for (int i = 1; i <= Vars::NumSelectWorld; i++)
        {
            luabind::object e = luabind::newtable(L);
            auto& ep = Vars::SelectWorld[i];
            e["episodeName"] = (std::string)ep.WorldName;
            e["episodePath"] = (std::string)ep.WorldPath;
            e["episodeWorldFile"] = (std::string)ep.WorldFile;
            outData[++counter] = e;
        }
    }
    
    return outData;
}

// returns a table of all the episodes in the SMBX2 world list. If there's more than 100 episodes in the worlds directory, some episodes will be missing (Until the episode list system gets recoded, but Rednaxela suggested otherwise)
luabind::object LuaProxy::Episode::list(lua_State *L)
{
    return getAllEpisodes(L);
}

// gets the currently running episode's ID, which can be used to retrieve the running episode's world info with Episode.list()[id]
int LuaProxy::Episode::id()
{
    using namespace SMBX13;

    return Vars::selWorld;
}

std::string LuaProxy::Episode::name()
{
    using namespace SMBX13;

    auto& ep = Vars::SelectWorld[Vars::selWorld];
    return ep.WorldName;
}

std::string LuaProxy::Episode::path()
{
    using namespace SMBX13;

    auto& ep = Vars::SelectWorld[Vars::selWorld];
    return ep.WorldPath;
}

std::string LuaProxy::Episode::filename()
{
    using namespace SMBX13;

    auto& ep = Vars::SelectWorld[Vars::selWorld];
    return ep.WorldFile;
}
