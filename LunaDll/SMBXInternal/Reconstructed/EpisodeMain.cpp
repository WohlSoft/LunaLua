#include <string>
#include <memory>
#include <iostream>
#include <fstream>

#include <luabind/adopt_policy.hpp>
#include <luabind/out_value_policy.hpp>

#include "EpisodeMain.h"

#include "../../version.h"
#include "../../Globals.h"
#include "../../GlobalFuncs.h"
#include "../../SMBXInternal/Level.h"
#include "../../Misc/MiscFuncs.h"
#include "../../SMBXInternal/PlayerMOB.h"
#include "../../SMBXInternal/NPCs.h"
#include "../../SMBXInternal/BGOs.h"
#include "../../SMBXInternal/Animation.h"
#include "../../SMBXInternal/Overworld.h"
#include "../../SMBXInternal/CollectedStarRecord.h"

#include "../../Rendering/FrameCapture.h"
#include "../../libs/PGE_File_Formats/file_formats.h"

#include "../../LuaMain/LuaHelper.h"
#include "../../LuaMain/LuaProxy.h"
#include "../../LuaMain/LuaProxyComponent/LuaProxyAudio.h"
#include "../../libs/luasocket/luasocket.h"
#include "../../libs/luasocket/mime.h"
#include "../../SdlMusic/MusicManager.h"
#include "../../Rendering/LunaImage.h"
#include "../../Rendering/ImageLoader.h"
#include "../../Misc/RuntimeHook.h"
#include "../../Input/MouseHandler.h"

#include "../../Misc/LoadScreen.h"
#include "../../LuaMain/LunaPathValidator.h"

#include "../../Misc/CollisionMatrix.h"
#include "../../FileManager/SMBXFileManager.h"

extern PlayerMOB* getTemplateForCharacterWithDummyFallback(int id);
extern "C" void __cdecl LunaLuaSetGameData(const char* dataPtr, int dataLen);

// Patch to allow exiting the pause menu. Apply when the vanilla pause/textbox
// should be instantly exited always. Unapply when this should not be the case.
static auto exitPausePatch = PATCH(0x8E6564).NOP().NOP().NOP().NOP().NOP().NOP();

EpisodeMain::EpisodeMain()
{}

EpisodeMain::~EpisodeMain() {}

// The big one. This will load an episode anywhere in the engine. This is also used when booting the engine.
void EpisodeMain::LaunchEpisode(std::wstring wldPathWS, int saveSlot, int playerCount, Characters firstCharacter, Characters secondCharacter, bool suppressSound)
{
    //--ElseIf .Jump = True Or .Start = True Or (GetKeyState(vbKeySpace) And KEY_PRESSED) Or (GetKeyState(vbKeyReturn) And KEY_PRESSED) Or MenuMouseClick = True Then (line 4945)--

    // replace all the forward slashes with backward slashes
    replaceSubStrW(wldPathWS, L"/", L"\\");

    // this is used for the FindSaves function
    EpisodeMain episodeMainFunc;

    // this is used for setting up loadEpisode from lua after first booting an episode
    GameAutostart GameAutostartFunc;

    // this needs to be set already so we can convert it to a std::string
    std::wstring fullPathWS = wldPathWS;

    /*
    --
        **string references**
    
        - world path with world file
        - resolved world path with world file
        - path with no world file
        - world file with no path
        - path with no world file, ending with a slash
    --
    */ 

    std::string wldPathS = WStr2Str(wldPathWS);
    std::string fullPathS = WStr2Str(fullPathWS);
    std::string fullPthNoWorldFileS = splitFilenameFromPath(fullPathS);
    std::string fullWorldFileNoPthS = splitPathFromFilename(fullPathS);
    std::string fullPthNoWorldFileWithEndSlashS = fullPthNoWorldFileS + "\\";

    /*
    --
        **wstring references**

        - resolved world path with world file (see above)
        - path with no world file
        - world file with no path
        - path with no world file, ending with a slash
    --
    */ 

    std::wstring fullPthNoWorldFileWS = Str2WStr(fullPthNoWorldFileS);
    std::wstring fullWorldFileNoPthWS = Str2WStr(fullWorldFileNoPthS);
    std::wstring fullPthNoWorldFileWithEndSlashWS = Str2WStr(fullPthNoWorldFileWithEndSlashS);

    /*
    --
        **visual basic 6 string ptr references**
    
        - world path with world file
        - world file with no path
        - path with no world file, ending with a slash
    --
    */ 

    VB6StrPtr fullPathVB6 = fullPathS;
    VB6StrPtr fullWorldFileNoPthVB6 = fullWorldFileNoPthS;
    VB6StrPtr fullPthNoWorldFileWithEndSlashVB6 = fullPthNoWorldFileWithEndSlashS;

    // FileFormats WorldData, saved for the FindSaves function and the world intro filename
    WorldData wldData;

    // create a tempLocation
    Momentum tempLocation;
    
    // make a bool for external episodes
    bool externalEpisode = false;

    // check to see if the wld file is valid, otherwise don't load the entire episode if booting
    if(fileExists(fullPathWS))
    {
        std::wstring nonAnsiCharsEpisode = GetNonANSICharsFromWStr(fullPathWS);
        if(!nonAnsiCharsEpisode.empty())
        {
            std::wstring path = L"The episode path has characters which are not compatible with the system default Windows ANSI code page. This is not currently supported. Please rename or move your episode folder.\n\nUnsupported characters: " + nonAnsiCharsEpisode + L"\n\nPath:\n" + fullPthNoWorldFileWS;
            MessageBoxW(0, path.c_str(), L"SMBX does not support episode path", MB_ICONERROR);
            _exit(1);
        }

        std::wstring nonAnsiCharsFullPath = GetNonANSICharsFromWStr(fullPathWS);
        if(!nonAnsiCharsFullPath.empty())
        {
            std::wstring path = L"The world map filename has characters which are not compatible with the system default Windows ANSI code page. This is not currently supported. Please rename your world map file.\n\nUnsupported characters: " + nonAnsiCharsFullPath + L"\n\nPath:\n" + fullPathWS;
            MessageBoxW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
            _exit(1);
        }

        if(!FileFormats::OpenWorldFileHeader(fullPathS, wldData) || !wldData.meta.ReadFileValid)
        {
            std::wstring path = L"The world map file header cannot be parsed.\n\nPath:\n" + fullPathWS;
            MessageBoxW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
            _exit(1);
        }

        if(wldData.meta.RecentFormat != WorldData::SMBX64)
        {
            std::wstring path = L"The world map file is in the wrong format. It must be saved in SMBX64 format.\n\nPath:\n" + fullPathWS;
            MessageBoxW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
            _exit(1);
        }
    }
    else
    {
        std::wstring path = L"SMBX could not find the world map file \"" + fullPathWS + L"\"";
        MessageBoxW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
        _exit(1);
    }

    // when the episode has loaded successfully after boot, we'll need to do some extra things in order for this to work
    if(gEpisodeLoadedOnBoot)
    {
        // force-unpause the game
        exitPausePatch.Apply();

        // exit lua
        gLunaLua.exitContext();

        // get rid of any additional custom graphics loaded
        gCachedFileMetadata.purge();
    }
    
    // make a worldName string and VB6StrPtr
    std::string worldNameS = "";
    VB6StrPtr worldNameVB6 = worldNameS;
    
    // make an idx int variable
    int externalEpisodeIdx = 0;

    // build the episode list
    SMBXWorldFileBase::PopulateEpisodeList();
    
    // check to see if the episode is external. if so, we'll need to change some things.
    if(checkIfWorldIsInWorldPath(fullPathS))
    {
        worldNameS = findNameFromEpisodeWorldPath(wldPathS);
    }
    else
    {
        // toggle this on
        externalEpisode = true;

        // make the episode name the episode's actual name
        worldNameS = wldData.EpisodeTitle;
        worldNameVB6 = worldNameS;

        // this code, from here, sets the new episode
        externalEpisodeIdx = episodeMainFunc.WriteEpisodeEntry(worldNameVB6, fullPthNoWorldFileWithEndSlashVB6, fullWorldFileNoPthVB6, wldData, true);
    }

    // reset cheat status
    GM_CHEATED = COMBOOL(false);

    // reset checkpoints
    GM_STR_CHECKPOINT = "";

    // set GM_FULLDIR, otherwise the Loadscreen won't have write access to the episode we're loading to
    GM_FULLDIR = fullPthNoWorldFileWithEndSlashWS;

    // show loadscreen
    LunaLoadScreenStart();

    // setup SFXs
    native_setupSFX();

    // specify the menu level
    if(!externalEpisode)
    {
        if(GM_EP_LIST_COUNT < 100)
        {
            GM_CUR_MENULEVEL = findEpisodeIDFromWorldFileAndPath(fullPathS); // this NEEDS to be set, otherwise the engine will just crash loading the episode
        }
        else
        {
            int additionalEpIdx = 0;
            additionalEpIdx = episodeMainFunc.WriteEpisodeEntry(worldNameVB6, fullPthNoWorldFileWithEndSlashVB6, fullWorldFileNoPthVB6, wldData, false);
            GM_CUR_MENULEVEL = additionalEpIdx;
        }
    }
    else if(externalEpisode)
    {
        GM_CUR_MENULEVEL = externalEpisodeIdx;
    }

    // clear gamedata
    LunaLuaSetGameData(0, 0);

    //--BEGIN MAIN RECODE--

    // play the world loaded sfx if suppressSound is false
    if(!suppressSound)
    {
        if(gStartupSettings.epSettings.canPlaySFXOnStartup)
        {
            short soundID = 29;
            native_playSFX(&soundID); //--PlaySound 29 (line 4946)--
        }
    }

    // implement player count
    GM_PLAYERS_COUNT = playerCount; //--numPlayers = MenuMode / 10 (line 4947)--

    // apply templates (SavedChar)
    For(i, 1, GM_MAX_CHARACTERS) //--For A = 1 To numCharacters (line 4948)--
    {
        PlayerMOB* playerTemplate = &((PlayerMOB*)GM_PLAYERS_TEMPLATE)[i]; //--SavedChar(A) = blankPlayer (line 4949)--
        memset(playerTemplate, 0, sizeof(PlayerMOB));

        playerTemplate->Identity = (Characters)i; //--SavedChar(A).Character = A (line 4950)--
        playerTemplate->CurrentPowerup = 1; //--SavedChar(A).State = 1 (line 4951)--
    }

    // use the character variables that were specified
    For(i, 1, GM_PLAYERS_COUNT)
    {
        auto p = Player::Get(i);

        // implement missing player values before loading the save file
        p->CurrentPowerup = 1; //--Player(1/2).State (line 4953/4964)--
        p->MountType = 0; //--Player(1/2).Mount (line 4954/4965)--
        p->Identity = static_cast<Characters>(1); //--Player(1/2).Character = 1 (line 4955/4966)--
        p->PowerupBoxContents = 0; //--Player(1/2).HeldBonus = 0 (line 4956/4967)--
        p->TakeoffSpeed = 0; //--Player(1/2).CanFly = False (line 4957/4968)--
        p->CanFly = 0; //--Player(1/2).CanFly2 = False (line 4958/4969)--
        p->TailswipeTimer = 0; //--Player(1/2).TailCount = 0 (line 4959/4970)--
        p->YoshiHasEarthquake = 0; //--Player(1/2).YoshiBlue = False (line 4960/4971)--
        p->YoshiHasFireBreath = 0; //--Player(1/2).YoshiRed = False (line 4961/4972)--
        p->YoshiHasFlight = 0; //--Player(1/2).YoshiYellow = False (line 4962/4973)--
        p->Hearts = 0; //--Player(1/2).Hearts = 0 (line 4963/4974)--
    }

    // get the first player only
    auto p1 = Player::Get(1);

    // implement the 1st player's character (lines 4975-4978)
    if(GM_PLAYERS_COUNT >= 1)
    {
        // checks to make sure that the character can be selected or not
        if(firstCharacter >= static_cast<Characters>(1) && firstCharacter <= static_cast<Characters>(GM_MAX_CHARACTERS))
        {
            p1->Identity = firstCharacter;
        }
        else
        {
            p1->Identity = static_cast<Characters>(1);
        }
    }

    // implement the 2nd player's character (lines 4979-4982)
    if(GM_PLAYERS_COUNT >= 2)
    {
        For(i, 2, GM_PLAYERS_COUNT)
        {
            // get any player above 2
            auto p2 = Player::Get(i);

            // checks to make sure that the character can be selected or not
            if(secondCharacter >= static_cast<Characters>(1) && secondCharacter <= static_cast<Characters>(GM_MAX_CHARACTERS))
            {
                p2->Identity = secondCharacter;
            }
            else
            {
                p2->Identity = static_cast<Characters>(2);
            }
        }
    }

    // we'll probably get more than 3 players loading on boot if specified on the command prompt, so this needs to exist (index starts at 2 on the for loop to simulate supermario# cheats)
    if(GM_PLAYERS_COUNT >= 3)
    {
        For(i, 2, GM_PLAYERS_COUNT)
        {
            auto p = Player::Get(i);
            p->Identity = Player::Get(1)->Identity;
        }
    }
    
    if(gEpisodeLoadedOnBoot) // do this too if an episode is already loaded
    {
        // restore characters if booted already
        For(i, 1, GM_PLAYERS_COUNT)
        {
            auto p = Player::Get(i);

            // restore this player's character
            p->Identity = gPlayerStoredCharacters[min(i, 4)-1];
        }
    }

    // if we have any blocked characters, don't use them and instead specify whatever is not blocked (Not compatible with X2 characters, but they're a mess in basegame so oh well)
    For(i, 1, GM_PLAYERS_COUNT)
    {
        checkBlockedCharacterFromWorldAndReplaceCharacterIfSo(i);
    }

    // replicating code from 1.3 cause why not
    GM_CUR_MENUCHOICE = saveSlot - 1;

    // implement missing values before loading the save file
    GM_CUR_SAVE_SLOT = GM_CUR_MENUCHOICE + 1; //--selSave = MenuCursor + 1 (line 4983)--
    GM_STAR_COUNT = 0; //--numStars = 0 (4984)--
    GM_COINS = 0; //--Coins = 0 (line 4985)--
    GM_UNK_B2C8E4 = 0; //--Score = 0 (line 4986)--
    GM_PLAYER_LIVES = 3; //--Lives = 3 (line 4987)--
    
    // set that we're on map
    GM_EPISODE_MODE = COMBOOL(true); //--LevelSelect = True (line 4988)--
    GM_LEVEL_MODE = COMBOOL(false); //--GameMenu = False (line 4989)--

    /*
        skipping these cause lunadll handles this stuff instead

        --
            BitBlt myBackBuffer, 0, 0, ScreenW, ScreenH, 0, 0, 0, vbWhiteness (line 4990)
            BitBlt frmMain.hdc, 0, 0, frmMain.ScaleWidth, frmMain.ScaleHeight, 0, 0, 0, vbWhiteness (line 4991)
        --
    */

    // stop music
    native_stopMusic(); //--StopMusic (line 4992)--
    
    /*
        skipping these cause lunadll handles this stuff instead

        --
            DoEvents (line 4993)
            Sleep 500 (line 4994)
        --
    */
    // load the world
    native_loadWorld(&fullPathVB6); //--OpenWorld SelectWorld(selWorld).WorldPath & SelectWorld(selWorld).WorldFile (line 4995)--

    // load the save file data
    if (episodeMainFunc.FindSaves(fullPthNoWorldFileWithEndSlashS, GM_CUR_SAVE_SLOT) >= 0) //--If SaveSlot(selSave) >= 0 Then (line 4996)--
    {
        // blank out intro filename if the episode already has a save file and the intro was already played
        if(GM_HUB_STYLED_EPISODE == 0) //--If NoMap = False Then StartLevel = "" (line 4997)--
        {
            GM_WORLD_INTRO_FILENAME = "";
        }

        native_loadGame(); //--LoadGame (line 4998)--
    } //--End If (line 4999)--

    // get if the illparkwhereiwant cheat is active
    if(GM_WORLD_UNLOCK == -1) //--If WorldUnlock = True Then (line 5000)--
    {
        // get all paths
        For(i, 1, GM_PATH_COUNT) //--For A = 1 To numWorldPaths (line 5001)--
        {
            tempLocation = SMBXPath::Get(i)->momentum; //--tempLocation = WorldPath(A).Location (line 5002)--

            //--With tempLocation (line 5003)--
            tempLocation.x = tempLocation.x + 4; //--.X = .X + 4 (line 5004)--
            tempLocation.y = tempLocation.y + 4; //--.Y = .Y + 4 (line 5005)--
            tempLocation.width = tempLocation.width - 8; //--.Width = .Width - 8 (line 5006)--
            tempLocation.height = tempLocation.height - 8; //--.Height = .Height - 8 (line 5007)--
            //--End With (line 5008)--

            // set to active
            SMBXPath::Get(i)->visible = COMBOOL(true); //--WorldPath(A).Active = True (line 5009)--

            // now get sceneries
            For(j, 1, GM_SCENERY_COUNT) //--For B = 1 To numScenes (line 5010)--
            {
                // check the collision of paths and sceneries
                if(CheckCollision(tempLocation, SMBXScenery::Get(j)->momentum)) //--If CheckCollision(tempLocation, Scene(B).Location) Then Scene(B).Active = False (line 5011)--
                {
                    // make any scenery if collided invisible if true
                    SMBXScenery::Get(j)->field_32 = COMBOOL(false);
                }
            } //--Next B (line 5012)--
        } //--Next A (line 5013)--

        // now get world levels
        For(i, 1, GM_LEVEL_COUNT) //--For A = 1 To numWorldLevels (line 5014)--
        {
            // make them visible
            WorldLevel::Get(i)->visible = COMBOOL(true); //--WorldLevel(A).Active = True (line 5015)--
        } //--Next A (line 5016)--
    } //--End If (line 5017)--

    // init SetupPlayers
    native_initLevelEnv(); //--SetupPlayers (line 5018)--

    // load the autoboot level if there's no save file, or the hub level if set
    if((GM_WORLD_INTRO_FILENAME != GM_STR_NULL && !saveFileExists()) || (GM_HUB_STYLED_EPISODE == -1)) //--If StartLevel <> "" Then-- (line 5019)
    {
        // make the strings, wstrings, and visual basic 6 string ptr's for the world intro filename
        std::string fullPathAndAutobootLvlS = fullPthNoWorldFileWithEndSlashS + (std::string)GM_WORLD_INTRO_FILENAME;
        std::wstring fullPathAndAutobootLvlWS = Str2WStr(fullPathAndAutobootLvlS);
        VB6StrPtr fullPathAndAutobootLvlVB6 = fullPathAndAutobootLvlS;

        // check to see if the autoboot level exists
        if(fileExists(fullPathAndAutobootLvlS))
        {
            // load the autoboot level from the episode if we're starting it for the first time, or the hub level if it's a hub-styled episode
            //--PlaySound 28 (line 5020)--
            //--SoundPause(26) = 200 (line 5021)--
            GM_EPISODE_MODE = COMBOOL(false); //--LevelSelect = False (line 5022)--

            //--(line 5023) [left blank]--

            //--GameThing (line 5024)--
            native_cleanupLevel(); //--ClearLevel (line 5025)--

            //--(line 5026) [left blank]--

            //--Sleep 1000 (line 5027)--
            native_loadLevel(&fullPathAndAutobootLvlVB6); //--OpenLevel SelectWorld(selWorld).WorldPath & StartLevel (line 5028)
        } //--End If (line 5029)--
        //--Exit Sub (line 5030)--
        // if it doesn't exist and there's no hub, error and boot the map instead after clicking "OK"
        else if(!fileExists(fullPathAndAutobootLvlS) && GM_HUB_STYLED_EPISODE == 0)
        {
            std::wstring path = L"The level autoboot file can not be loaded. Does it even exist?\n\nAutoboot level:\n" + fullPathWS;
            MessageBoxW(0, path.c_str(), L"SMBX could not load the autoboot level", MB_ICONERROR);

            // boot the map
            GM_EPISODE_MODE = COMBOOL(true);
        }
        // else if it doesn't exist and there IS a hub, error and exit instead after clicking "OK"
        else if(!fileExists(fullPathAndAutobootLvlS) && GM_HUB_STYLED_EPISODE == -1)
        {
            std::wstring path = L"The level hub file can not be loaded. Does it even exist?\n\nAutoboot level:\n" + fullPathWS + L"\n\nBecause there is no hub level, the game will now close after clicking OK. Please put in a valid hub level file in the episode before loading it.";
            MessageBoxW(0, path.c_str(), L"SMBX could not load the hub level", MB_ICONERROR);
            _exit(1);
        }
    }

    // make sure that lunadll knows the game loaded on boot, so that loadEpisode can know
    if(!gEpisodeLoadedOnBoot)
    {
        gEpisodeLoadedOnBoot = true;
    }

    exitPausePatch.Unapply();

    // hide loadscreen
    LunaLoadScreenKill();
    
    //--End If (line 5031)--

    //--END MAIN RECODE--
}

int EpisodeMain::FindSaves(std::string worldPathS, int saveSlot)
{
    // this is used for this function
    EpisodeMain episodeMainFunc;

    // FileFormats SaveData, used for getting the save slot data
    GamesaveData saveData;
    
    //--BEGIN MAIN RECODE--
    if(saveSlot >= 0) // is the save slot greater than or equal to 0?
    {
        /*
        --
            **string references**
        
            - world path to the .sav file
            - world path to the .savx file
        --
        */

        std::string saveFileS = worldPathS + "save" + std::to_string(saveSlot) + ".sav"; //--If Dir(SelectWorld(selWorld).WorldPath & "save" & A & ".sav") <> "" Then (line 7700)--
        std::string saveFileXtraS = worldPathS + "save" + std::to_string(saveSlot) + ".savx";

        /*
        --
            **wstring references**

            - world path to the .sav file
            - world path to the .savx file
        --
        */ 

        std::wstring saveFileWS = Str2WStr(saveFileS);
        std::wstring saveFileXtraWS = Str2WStr(saveFileXtraS);

        // if the .sav file exists, and FileFormats successfully reads the sav file as a SMBX 1.3 save file, continue
        if(fileExists(saveFileS) && FileFormats::ReadSMBX64SavFileF(saveFileS, saveData)) //--Open SelectWorld(selWorld).WorldPath & "save" & A & ".sav" For Input As #1 (line 7701)--
        {
            // so basically the original FindSaves loops all over the save files reading things in a way that is only supported on VB6, but since we've got FileFormats we can breeze through this without relying on old methods of reading save files!
            int curActive = 0;
            int maxActive = 0;

            // the game beat flag
            maxActive++;
            if(saveData.gameCompleted)
            {
                curActive++;
            }

            // how much paths are open
            maxActive += (int)saveData.visiblePaths.size();
            for(auto &p : saveData.visiblePaths)
            {
                if(p.second)
                {
                    curActive++;
                }
            }

            // how much levels are opened
            maxActive += (int)saveData.visibleLevels.size();
            for(auto &p : saveData.visibleLevels)
            {
                if(p.second)
                {
                    curActive++;
                }
            }

            // how many stars are collected
            maxActive += (int(saveData.totalStars) * 4);
            curActive += (int(saveData.gottenStars.size()) * 4);

            // calculate the progress number
            if(maxActive > 0)
            {
                return int((float(curActive) / float(maxActive)) * 100);
            }
            else
            {
                return 100;
            }
        }
        // we don't support savx files yet, so this part will be commented out
        /*else if(fileExists(saveFileXtraS) && FileFormats::ReadExtendedSaveFileF(saveFileXtraS, saveData))
        {
            
        }*/
        else
        {
            // -1 means the data wasn't found, which is new in the case of detecting if the file wasn't found
            return -1;
        }
    }
    else
    {
        // -2 means the save data isn't valid, which is new for this case
        return -2;
    }

    //--End If (line 7763)--

    //--END MAIN RECODE--
}

int EpisodeMain::WriteEpisodeEntry(VB6StrPtr worldNameVB6, VB6StrPtr worldPathVB6, VB6StrPtr worldFileVB6, WorldData wldData, bool isNewEpisode)
{
    int newIdx = 0;
    if(GM_EP_LIST_COUNT < 100)
    {
        if(isNewEpisode)
        {
            GM_EP_LIST_COUNT++;
            newIdx = GM_EP_LIST_COUNT - 1;
        }
    }
    EpisodeListItem* item = EpisodeListItem::GetRaw(newIdx);
    item->episodeName = worldNameVB6;
    item->episodePath = worldPathVB6;
    item->episodeWorldFile = worldFileVB6;
    
    for (size_t i = 0; i < 5; i++)
    {
        if (i < wldData.nocharacter.size())
        {
            item->blockChar[i] = COMBOOL(wldData.nocharacter[i]);
        }
        else
        {
            item->blockChar[i] = 0;
        }
    }
    item->padding_16 = 0;

    return newIdx;
}
