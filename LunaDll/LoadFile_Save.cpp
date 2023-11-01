#include <future>
#include <chrono>
#include <thread>
#include <fmt/fmt_format.h>

#include "../Defines.h"
#include "../Main.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Misc/MiscFuncs.h"
#include "../Misc/SaveData.h"

#include "../libs/PGE_File_Formats/file_formats.h"
#include "../../SMBXInternal/Overworld.h"
#include "../../SMBXInternal/WorldLevel.h"
#include "../../SMBXInternal/Path.h"
#include "../../SMBXInternal/Scenery.h"
#include "../../SMBXInternal/CollectedStarRecord.h"

PlayerMOB* getTemplateForCharacter(int id); // from RuntimeHookCharacterId
static void copyPlayerToTemplate(int characterId, int playerIdx) // Copy the specified player to their character's player template
{
    PlayerMOB* temp = getTemplateForCharacter(characterId);
    if (temp != nullptr) {
        PlayerMOB* player = &((PlayerMOB*)GM_PLAYERS_PTR)[playerIdx];
        memcpy(temp, player, sizeof(PlayerMOB));
    }
}
static void copyTemplateToPlayer(int characterId, int playerIdx)
{
    PlayerMOB* temp = getTemplateForCharacter(characterId);
    if (temp != nullptr) {
        PlayerMOB* player = &((PlayerMOB*)GM_PLAYERS_PTR)[playerIdx];
        memcpy(player, temp, sizeof(PlayerMOB));
    }
}

bool LunaLua_loadSaveFileFromPath_savx(std::wstring fullPath);
void LunaLua_writeSaveFileFromPath_savx(std::wstring fullPath);

// returns TRUE if twe should have a valid save slot here
bool EpisodeHasValidSavesPath() {
    return GM_CUR_SAVE_SLOT != 0;
}

std::wstring getLastDirectory(const std::wstring& fname) {
    size_t pos = fname.find_last_of(L"\\/");
    return (std::wstring::npos == pos) ? L"" : fname.substr(pos+1, fname.length()-(pos+1));
}
// returns the saves folder, eg. data/saves/the invasion 2/
std::wstring get_save_directory(bool ensurePath) {
    if (!EpisodeHasValidSavesPath())
    {
        return L"";
    }

    std::wstring savesDirectory = gAppPathWCHAR + L"\\saves";

    if (ensurePath) {
        // Verify saves directory exists
        if (!DirectoryExists(savesDirectory.c_str())) {
            CreateDirectoryW(savesDirectory.c_str(), NULL);
        }
    }

    // trim trailing slash of episode path and get episode name

    std::wstring episodeName = getLastDirectory(std::wstring(GM_FULLDIR).substr(0, std::wstring(GM_FULLDIR).length() - 1));
    savesDirectory += L"\\" + episodeName;
    if (ensurePath) {
        // Verify episode path within saves directory exists
        if (!DirectoryExists(savesDirectory.c_str())) {
            CreateDirectoryW(savesDirectory.c_str(), NULL);
        }
    }
    return savesDirectory;
}
// returns save file path, eg. data/saves/the invasion 2/save1.savx
std::wstring get_save_file_path(bool ensurePath) {
    auto dir = get_save_directory(ensurePath);
    if (dir.size() == 0)
    {
        return L"";
    }
    return dir + L"\\save" + Str2WStr(i2str(GM_CUR_SAVE_SLOT).c_str()) + L".savx";
}

std::string GetSavesPath() {
    return WStr2Str(get_save_directory(false));
}
std::wstring GetSavesPathW() {
    return get_save_directory(false);
}

void InitializeSavePath() {
    // make sure the save data directory is created
    get_save_directory(true);
}


void LunaLua_writeSaveFile_savx() {
    if (GM_CHEATED) return;
    if (!EpisodeHasValidSavesPath()) return;
    std::wstring savePath = get_save_file_path(true);
    if (savePath.size() > 0)
    {
        LunaLua_writeSaveFileFromPath_savx(savePath);
    }
}

// returns false if failed to load / doesn't exist - in which case, try to load legacy save
bool LunaLua_loadSaveFile_savx() {
    if (!EpisodeHasValidSavesPath()) return false;
    std::wstring savePath = get_save_file_path(false);
    if (savePath.size() == 0)
    {
        return false; // no save path
    }
    else
    {
        return LunaLua_loadSaveFileFromPath_savx(savePath);
    }
}

// handles some internal logic that isn't called when overriding base smbx' SaveGame
void LunaLua_preWriteSaveSavx() {
    // copy players to templates
    for (int i = GM_PLAYERS_COUNT; i >= 1; i--) {
        copyPlayerToTemplate(Player::Get(i)->Identity, i);
    }
    // clean up invalid stars...? idk if this code ever does anything in practice
    for (int i = GM_STAR_COUNT-1; i >= 0; i--) {
        auto obj = SMBX_CollectedStarRecord::Get(i);
        // if this star has a blank level name
        if (obj->levelFileName.length() == 0)
        {
            // drop the last star down into this slot, if we have another one
            if (GM_STAR_COUNT > 1) {
                auto lastStar = SMBX_CollectedStarRecord::Get(GM_STAR_COUNT - 1);
                obj->levelFileName = lastStar->levelFileName;
                obj->section = lastStar->section;
            }
            // subtract from star counter
            GM_STAR_COUNT = GM_STAR_COUNT - 1;
        }
    }
}

void LunaLua_writeSaveFileFromPath_savx(std::wstring fullPath) {
    LunaLua_preWriteSaveSavx();

    auto s = GamesaveData();
    s.lives = GM_PLAYER_LIVES;
    s.coins = GM_COINS;
    s.points = GM_SCORE; // this will always be 0 in x2, hudoverrides replaces score..
    s.totalStars = GM_STAR_COUNT;
    s.worldPosX = SMBXOverworld::get()->momentum.x;
    s.worldPosY = SMBXOverworld::get()->momentum.y;
    s.musicID = GM_CURRENT_WORLD_MUSIC;
    s.gameCompleted = GM_BEAT_THE_GAME != 0;
    // TODO: musicFile when custom music is implemented

    // last_hub_warp doesn't correspond to any 1.3 data, so is unused

    // VISIBLE LEVELS: ////////////////////////////
    for (int i = 0; i < WorldLevel::Count(); i++) {
        WorldLevel* obj = WorldLevel::Get(i);
        s.visibleLevels.push_back({ i, obj->visible });
    }
    // VISIBLE SCENES: ////////////////////////////
    for (int i = 0; i < SMBXScenery::Count(); i++) {
        SMBXScenery* obj = SMBXScenery::Get(i);
        s.visibleScenery.push_back({i, obj->visible});
    }
    // VISIBLE PATHS: ////////////////////////////
    for (int i = 0; i < SMBXPath::Count(); i++) {
        SMBXPath* obj = SMBXPath::Get(i);
        s.visiblePaths.push_back({ i, obj->visible });
    }
    // COLLECTED STARS: //////////////////////////
    for (int i = 0; i < SMBX_CollectedStarRecord::Count(); i++) {
        SMBX_CollectedStarRecord* obj = SMBX_CollectedStarRecord::Get(i);
        s.gottenStars.push_back({ obj->levelFileName, obj->section });
    }

    // Player's selected characters
    // we... don't actually use this, but it is part of the format?
    for (int i = 1; i <= GM_PLAYERS_COUNT; i++) {
        s.currentCharacter.push_back(Player::Get(i)->Identity);
    }

    // Player states:
    // Player states are only stored the 'intended' way for 1.3 characters, the rest are stored in lua SaveData
    for (int i = 1; i <= 5; i++) {
        auto temp = getTemplateForCharacter(i);
        saveCharState saved;
        saved.id = i;
        saved.state = temp->CurrentPowerup;
        saved.itemID = temp->PowerupBoxContents;
        saved.mountType = temp->MountType;
        saved.mountID = temp->MountColor;
        saved.health = temp->Hearts;
        s.characterStates.push_back(saved);
    }
    
    // Lua saved data
    auto luaUserData = LunaLuaGetSaveDataRaw();
    if (luaUserData != nullptr) {
        saveUserData::DataSection dataSection;
        dataSection.location = saveUserData::DataLocation::DATA_GLOBAL;
        dataSection.location_name = "";
        dataSection.name = "LuaSaveData";
        dataSection.data.push_back({ "chunk", luaUserData->data });
        s.userData.store.push_back(dataSection);
    }

    // write the file!
    FileFormats::WriteExtendedSaveFileF(WStr2Str(fullPath), s);
    
    // clean up
    LunaLuaFreeReturnedGameDataRaw(luaUserData);
}
bool LunaLua_loadSaveFileFromPath_savx(std::wstring fullPath) {
    auto s = GamesaveData();
    bool success = FileFormats::ReadExtendedSaveFileF(WStr2Str(fullPath), s);
    if (success)
    {
        // load savx data
        GM_PLAYER_LIVES = s.lives;
        GM_COINS = s.coins;
        GM_SCORE = s.points;
        //GM_STAR_COUNT = s.totalStars; // this doubles as the star entry array count and the in-game star counter, so we set it in the star-reading code below
        SMBXOverworld::get()->momentum.x = s.worldPosX;
        SMBXOverworld::get()->momentum.y = s.worldPosY;
        GM_CURRENT_WORLD_MUSIC = s.musicID;
        GM_BEAT_THE_GAME = COMBOOL(s.gameCompleted);
        // TODO: musicFile, once custom map music is implemented
        
        int visLevels = 0;
        // VISIBLE LEVELS: ////////////////////////////
        for (int i = 0; i < s.visibleLevels.size(); i++) {
            auto t = s.visibleLevels[i];
            auto obj = WorldLevel::Get(t.first);
            if (obj != NULL) obj->visible = COMBOOL(t.second);
        }
        // VISIBLE SCENES: ////////////////////////////
        for (int i = 0; i < s.visibleScenery.size(); i++) {
            auto t = s.visibleScenery[i];
            auto obj = SMBXScenery::Get(t.first);
            if (obj != NULL) obj->visible = COMBOOL(t.second);
        }
        // VISIBLE PATHS: ////////////////////////////
        for (int i = 0; i < s.visiblePaths.size(); i++) {
            auto t = s.visiblePaths[i];
            auto obj = SMBXPath::Get(t.first);
            if (obj != NULL) obj->visible = COMBOOL(t.second);
        }
        
        // COLLECTED STARS: //////////////////////////
        GM_STAR_COUNT = s.gottenStars.size();
        if (GM_STAR_COUNT >= 1000) {
            // exceeded max stars!
            GM_STAR_COUNT = 1000;
        }
        for (int i = 0; i < GM_STAR_COUNT; i++) {
            auto* obj = SMBX_CollectedStarRecord::Get(i);
            auto record = s.gottenStars[i];
            obj->levelFileName = record.first;
            obj->section = record.second;
        }

        // Player states:
        for (int i = 0; i < s.characterStates.size(); i++) {
            auto saved = s.characterStates[i];
            auto temp = getTemplateForCharacter(saved.id);
            if (temp != nullptr) {
                temp->CurrentPowerup = saved.state;
                temp->PowerupBoxContents = saved.itemID;
                temp->MountType = saved.mountType;
                temp->MountColor = saved.mountID;
                temp->Hearts = saved.health;
            }
        }

        // try reading out SaveData string
        for (int i = 0; i < s.userData.store.size(); i++) {
            auto item = s.userData.store[i];
            if (item.location == saveUserData::DataLocation::DATA_GLOBAL && item.name == "LuaSaveData") {
                for (int j = 0; j < item.data.size(); j++) {
                    auto data = item.data[j];
                    if (data.key == "chunk") {
                        const char* chunk = data.value.c_str();
                        LunaLuaSetSaveDataRaw(chunk, strlen(chunk));
                        break;
                    }
                }
                break;
            }
        }

        // Restore player's character states
        for (int i = 1; i <= GM_PLAYERS_COUNT; i++) {
            copyTemplateToPlayer(Player::Get(i)->Identity, i);
        }
    }

    return success;
}
void LunaLua_preLoadSaveFile() {
    // clear savedata
    LunaLuaSetSaveDataRaw("", 0);
}