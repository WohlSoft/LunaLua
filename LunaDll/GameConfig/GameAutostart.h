#ifndef GameAutostart_hhhhh
#define GameAutostart_hhhhh

#include "../SMBXInternal/Menu.h"
#include <IniProcessor/ini_processing.h>
#include <memory>

class GameAutostart
{
private:
    //Settings
    std::string selectedEpisode;
    bool singleplayer;
    Characters firstCharacter;
    Characters secondCharacter;
    int saveSlot;

public:
    GameAutostart();
    ~GameAutostart();
    static GameAutostart createGameAutostartByIniConfig(IniProcessing& reader);
    static void ClearAutostartPatch();

    std::string getSelectedEpisode() const { return selectedEpisode; }
    void setSelectedEpisode(std::string val) { selectedEpisode = val; }
    bool getSingleplayer() const { return singleplayer; }
    void setSingleplayer(bool val) { singleplayer = val; }
    Characters getFirstCharacter() const { return firstCharacter; }
    void setFirstCharacter(Characters val) { firstCharacter = val; }
    Characters getSecondCharacter() const { return secondCharacter; }
    void setSecondCharacter(Characters val) { secondCharacter = val; }
    int getSaveSlot() const { return saveSlot; }
    void setSaveSlot(int val) { saveSlot = val; }

    bool applyAutostart();
};

#endif
