#ifndef GameAutostart_hhhhh
#define GameAutostart_hhhhh

#include "../SMBXInternal/Menu.h"
#include "../libs/ini-reader/INIReader.h"

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
    static GameAutostart createGameAutostartByIniConfig(INIReader& reader);


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


    void applyAutostart();

};


#endif
