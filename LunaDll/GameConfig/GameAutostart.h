#ifndef GameAutostart_hhhhh
#define GameAutostart_hhhhh

#include "../SMBXInternal/Menu.h"
#include <IniProcessor/ini_processing.h>
#include <memory>

struct StartupEpisodeSettings;

class GameAutostart
{
public:
    GameAutostart();
    ~GameAutostart();
    static GameAutostart createGameAutostartByIniConfig(IniProcessing& reader);
    static GameAutostart createGameAutostartByStartupEpisodeSettings(const StartupEpisodeSettings& settings);
    static GameAutostart createGameAutostartByManualSettings(std::wstring wldPath, int players, int character1, int character2, int saveSlot);
    static void ClearAutostartPatch();

    void setSelectedEpisode(std::string val) { selectedEpisode = val; }
    void setSelectedEpisodePath(std::wstring val) { selectedWldPath = val; }
    void setSaveSlot(int val) { saveSlot = val; }
    void setPlayerCount(int val) { playerCount = val; }

    bool applyAutostart();
    
    //Settings
    std::string selectedEpisode;
    std::wstring selectedWldPath;
    bool singleplayer;
    int playerCount;
    Characters firstCharacter;
    Characters secondCharacter;
    int saveSlot;
};

#endif
