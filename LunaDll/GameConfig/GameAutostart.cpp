#include "GameAutostart.h"
#include "../Misc/AsmPatch.h"

// Patch for making introLoop skip to the right place
static auto skipIntoPatch = PATCH(0x8CA6A4).JMP(0x8CD13C).NOP_PAD_TO_SIZE<7>();

GameAutostart::GameAutostart() :
    selectedEpisode(""),
    singleplayer(true),
    firstCharacter(CHARACTER_MARIO),
    secondCharacter(CHARACTER_LUIGI),
    saveSlot(1)
{}


GameAutostart::~GameAutostart() {}

bool GameAutostart::applyAutostart()
{
    if (selectedEpisode == "")
        return false;

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

            GM_FULLDIR = item->episodePath;

            // Apply patch to make introLoop immediately skip to loading the episode
            skipIntoPatch.Apply();

            //We're done here
            return true;
        }
    }
    return false;
}

GameAutostart GameAutostart::createGameAutostartByIniConfig(IniProcessing &reader)
{
    GameAutostart autostarter;
    reader.beginGroup("autostart");
    autostarter.setSelectedEpisode(reader.value("episode-name", "").toString());
    autostarter.setSingleplayer(reader.value("singleplayer", true).toBool());
    autostarter.setFirstCharacter(static_cast<Characters>(reader.value("character-player1", 1).toInt()));
    autostarter.setSecondCharacter(static_cast<Characters>(reader.value("character-player2", 2).toInt()));
    autostarter.setSaveSlot(reader.value("save-slot", 1).toInt());
    reader.endGroup();
    return autostarter;
}

/*static*/ void GameAutostart::ClearAutostartPatch()
{
    skipIntoPatch.Unapply();
}
