#include "GameAutostart.h"


GameAutostart::GameAutostart() :
    selectedEpisode(""),
    singleplayer(true),
    firstCharacter(CHARACTER_MARIO),
    secondCharacter(CHARACTER_LUIGI),
    saveSlot(1)
{}


GameAutostart::~GameAutostart() {}



void GameAutostart::applyAutostart()
{
    if (selectedEpisode == "")
        return;

    //load all episodes
    native_loadWorldList();
    VB6StrPtr toSearchItem = selectedEpisode;
    std::vector<EpisodeListItem*> allEpisodes = EpisodeListItem::GetAll();
    for (unsigned int i = 0; i < allEpisodes.size(); ++i){
        EpisodeListItem* item = allEpisodes[i];
        if (item->episodeName == toSearchItem) {
            //Slot selection/Singleplayer
            GM_CUR_MENUTYPE = (singleplayer ? 10 : 20);
            //first Character
            GM_CUR_MENUPLAYER1 = static_cast<int>(firstCharacter);
            //second Character
            GM_CUR_MENUPLAYER2 = static_cast<int>(secondCharacter);
            //Load the selected episode
            GM_CUR_MENULEVEL = i + 1;
            
            //Load save states
            native_loadSaveStates();

            //First save slot
            GM_CUR_MENUCHOICE = saveSlot - 1;

            //Now simulate an VK_RETURN and enter the episode
            *(WORD*)0xB2D6D4 = -1;
            
            //We're done here
            return;
        }
    }
}


GameAutostart GameAutostart::createGameAutostartByIniConfig(INIReader& reader)
{
    GameAutostart autostarter;
    autostarter.setSelectedEpisode(reader.Get("autostart", "episode-name", ""));
    autostarter.setSingleplayer(reader.GetBoolean("autostart", "singleplayer", true));
    autostarter.setFirstCharacter(static_cast<Characters>(reader.GetInteger("autostart", "character-player1", 1)));
    autostarter.setSecondCharacter(static_cast<Characters>(reader.GetInteger("autostart", "character-player2", 2)));
    autostarter.setSaveSlot(reader.GetInteger("autostart", "save-slot", 1));
    return autostarter;
}
