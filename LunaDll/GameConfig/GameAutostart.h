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
    static void ClearAutostartPatch();

    void setSelectedEpisode(std::string val) { selectedEpisode = val; }
    void setSaveSlot(int val) { saveSlot = val; }

    bool applyAutostart();
    
    //Settings
    std::string selectedEpisode;
    std::wstring selectedWldPath;
    bool singleplayer;
    Characters firstCharacter;
    Characters secondCharacter;
    int saveSlot;
};

#endif
