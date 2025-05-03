#ifndef GameAutostart_hhhhh
#define GameAutostart_hhhhh

#include "../SMBXInternal/Menu.h"
#include <IniProcessor/ini_processing.h>
#include <memory>

struct StartupEpisodeSettings;

class GameAutostart
{
private:
    //Settings
    std::string selectedEpisode;
    std::wstring selectedWldPath;
    bool singleplayer;
    int playerCount;
    Characters firstCharacter;
    Characters secondCharacter;
    int saveSlot;

public:
    GameAutostart();
    ~GameAutostart();
    static GameAutostart createGameAutostartByIniConfig(IniProcessing& reader);
    static GameAutostart createGameAutostartByStartupEpisodeSettings(const StartupEpisodeSettings& settings);
    static GameAutostart createGameAutostartByManualSettings(std::wstring wldPath, int players, int character1, int character2, int saveSlot);
    static void ClearAutostartPatch();

    void setSelectedEpisode(std::string val)
    {
        selectedEpisode = val;
    }
    void setSelectedEpisodePath(std::wstring val)
    {
        selectedWldPath = val;
    }
    void setPlayerCount(int val)
    {
        playerCount = val;
        if(val >= 2)
        {
            singleplayer = false;
        }
        else if(val <= 1)
        {
            singleplayer = true;
        }
    }
    void setFirstCharacter(int val)
    {
        firstCharacter = (Characters)val;
    }
    void setSecondCharacter(int val)
    {
        secondCharacter = (Characters)val;
    }
    void setSaveSlot(int val)
    {
        saveSlot = val;
    }

    bool applyAutostart();
};

#endif
