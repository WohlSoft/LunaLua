#include <string>
#include <memory>
#include <iostream>
#include <fstream>

#include "EpisodeMain.h"

#include "../../Globals.h"
#include "../../GlobalFuncs.h"

#include "../../libs/PGE_File_Formats/file_formats.h"

#include "../../LuaMain/LuaHelper.h"
#include "../../LuaMain/LuaProxy.h"
#include "../../LuaMain/LunaPathValidator.h"

#include "../../GameConfig/GameAutostart.h"

#include "../../Misc/LoadScreen.h"
#include "../../Misc/MiscFuncs.h"
#include "../../Misc/ResourceFileMapper.h"
#include "../../Misc/RuntimeHook.h"

#include "../../FileManager/SMBXFileManager.h"

#include "../Functions.h"
#include "../Types.h"
#include "../Variables.h"

extern PlayerMOB* getTemplateForCharacterWithDummyFallback(int id);
extern "C" void __cdecl LunaLuaSetGameData(const char* dataPtr, int dataLen);

EpisodeMain gEpisodeMain;

EpisodeMain::EpisodeMain()
{}

EpisodeMain::~EpisodeMain() {}

// The big one. This will load an episode anywhere in the engine. This is also used when booting the engine.
void EpisodeMain::LaunchEpisode(std::wstring wldPathWS, int saveSlot, int playerCount, Characters firstCharacter, Characters secondCharacter)
{
    using namespace SMBX13;

    //--ElseIf .Jump = True Or .Start = True Or (GetKeyState(vbKeySpace) And KEY_PRESSED) Or (GetKeyState(vbKeyReturn) And KEY_PRESSED) Or MenuMouseClick = True Then (line 4945)--

    // replace all the forward slashes with backward slashes
    replaceSubStrW(wldPathWS, L"/", L"\\");

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
        - path with no world file, ending with a slash
    --
    */ 

    std::wstring fullPthNoWorldFileWS = Str2WStr(fullPthNoWorldFileS);
    std::wstring fullPthNoWorldFileWithEndSlashWS = Str2WStr(fullPthNoWorldFileWithEndSlashS);

    /*
    --
        **visual basic 6 string ptr references**
    
        - world path with world file
        - world file with no path
        - path with no world file, ending with a slash
        - a blank string variable
    --
    */ 

    VB6StrPtr fullPathVB6 = fullPathS;
    VB6StrPtr fullWorldFileNoPthVB6 = fullWorldFileNoPthS;
    VB6StrPtr fullPthNoWorldFileWithEndSlashVB6 = fullPthNoWorldFileWithEndSlashS;
    VB6StrPtr blankString = "";

    // set FileNamePath, otherwise the Loadscreen won't have write access to the episode we're loading to
    Vars::FileNamePath = fullPthNoWorldFileWithEndSlashWS;

    // FileFormats WorldData, saved for the FindSaves function and the world intro filename
    WorldData wldData;

    // create a tempLocation
    Types::Location_t tempLocation;
    
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
        externalEpisodeIdx = gEpisodeMain.WriteEpisodeEntry(worldNameVB6, fullPthNoWorldFileWithEndSlashVB6, fullWorldFileNoPthVB6, wldData, false);
    }

    // reset cheat status
    Vars::Cheater = false;

    // reset checkpoints
    Vars::Checkpoint = "";

    // show loadscreen
    LunaLoadScreenStart();

    // setup SFXs
    Functions::InitSound();

    // clear gamedata
    LunaLuaSetGameData(0, 0);

    // specify the menu level
    if(!externalEpisode)
    {
        if(Vars::NumSelectWorld < 100)
        {
            // this NEEDS to be set, otherwise the engine will just crash loading the episode
            Vars::selWorld = findEpisodeIDFromWorldFileAndPath(fullPathS);
        }
        else
        {
            // make a new variable
            int additionalEpIdx = 0;

            // overwrite an episode entry at idx 1
            additionalEpIdx = gEpisodeMain.WriteEpisodeEntry(worldNameVB6, fullPthNoWorldFileWithEndSlashVB6, fullWorldFileNoPthVB6, wldData, false);

            // set the world selection to 1 (This needs to be set, or the engine will crash)
            Vars::selWorld = additionalEpIdx;
        }
    }
    else if(externalEpisode)
    {
        Vars::selWorld = externalEpisodeIdx;
    }

    //--BEGIN MAIN RECODE--

    // implement player count
    Vars::numPlayers = playerCount; //--numPlayers = MenuMode / 10 (line 4947)--

    // make variables for important players
    auto& p1 = Vars::Player[1];

    // apply templates (SavedChar)
    SMBX13::Types::Player_t blankPlayer;

    for (int i = 1; i <= Types::numCharacters; i++) //--For A = 1 To numCharacters (line 4948)--
    {
        Vars::SavedChar[i] = blankPlayer; //--SavedChar(A) = blankPlayer (line 4949)--

        Vars::SavedChar[i].Character = i; //--SavedChar(A).Character = A (line 4950)--
        Vars::SavedChar[i].State = 1; //--SavedChar(A).State = 1 (line 4951)--
    }

    // use the character variables that were specified
    for (int i = 1; i <= Vars::numPlayers; i++)
    {
        auto& p = Vars::Player[i];

        // implement missing player values before loading the save file
        p.State = 1; //--Player(1/2).State (line 4953/4964)--
        p.Mount = 0; //--Player(1/2).Mount (line 4954/4965)--
        p.Character = 1; //--Player(1/2).Character = 1 (line 4955/4966)--
        p.HeldBonus = 0; //--Player(1/2).HeldBonus = 0 (line 4956/4967)--
        p.CanFly = 0; //--Player(1/2).CanFly = False (line 4957/4968)--
        p.CanFly2 = 0; //--Player(1/2).CanFly2 = False (line 4958/4969)--
        p.TailCount = 0; //--Player(1/2).TailCount = 0 (line 4959/4970)--
        p.YoshiBlue = 0; //--Player(1/2).YoshiBlue = False (line 4960/4971)--
        p.YoshiRed = 0; //--Player(1/2).YoshiRed = False (line 4961/4972)--
        p.YoshiYellow = 0; //--Player(1/2).YoshiYellow = False (line 4962/4973)--
        p.Hearts = 0; //--Player(1/2).Hearts = 0 (line 4963/4974)--
    }

    // implement the 1st player's character (lines 4975-4978)
    if(Vars::numPlayers >= 1)
    {
        // checks to make sure that the character can be selected or not
        if(firstCharacter >= static_cast<Characters>(1) && firstCharacter <= static_cast<Characters>(Types::numCharacters))
        {
            p1.Character = static_cast<int>(firstCharacter);
        }
        else
        {
            p1.Character = 1;
        }
    }

    // implement the 2nd player's character (lines 4979-4982)
    if(Vars::numPlayers >= 2)
    {
        for (int i = 2; i <= Vars::numPlayers; i++)
        {
            // get any player above 2
            auto& p2 = Vars::Player[i];

            // checks to make sure that the character can be selected or not
            if(secondCharacter >= static_cast<Characters>(1) && secondCharacter <= static_cast<Characters>(Types::numCharacters))
            {
                p2.Character = static_cast<int>(secondCharacter);
            }
            else
            {
                p2.Character = 2;
            }
        }
    }

    // we'll probably get more than 3 players loading on boot if specified on the command prompt, so this needs to exist (index starts at 2 on the for loop to simulate supermario# cheats)
    if(Vars::numPlayers >= 3)
    {
        for (int i = 2; i <= Vars::numPlayers; i++)
        {
            auto& p = Vars::Player[i];
            p.Character = Vars::Player[1].Character;
        }
    }
    
    if(gEpisodeLoadedOnBoot) // do this too if an episode is already loaded
    {
        // restore characters if booted already
        for (int i = 1; i <= Vars::numPlayers; i++)
        {
            auto p = Vars::Player[i];

            // restore this player's character
            p.Character = static_cast<int>(gPlayerStoredCharacters[min(i, 4)-1]);
        }
    }

    // if we have any blocked characters, don't use them and instead specify whatever is not blocked (Not compatible with X2 characters, but they're a mess in basegame so oh well)
    for (int i = 1; i <= Vars::numPlayers; i++)
    {
        checkBlockedCharacterFromWorldAndReplaceCharacterIfSo(i);
    }

    // replicating code from 1.3 cause why not
    Vars::MenuCursor = saveSlot - 1;

    // implement missing values before loading the save file
    Vars::selSave = Vars::MenuCursor + 1; //--selSave = MenuCursor + 1 (line 4983)--
    Vars::numStars = 0; //--numStars = 0 (4984)--
    Vars::Coins = 0; //--Coins = 0 (line 4985)--
    Vars::Score = 0; //--Score = 0 (line 4986)--
    Vars::Lives = 3; //--Lives = 3 (line 4987)--
    
    // set that we're on map
    Vars::LevelSelect = true; //--LevelSelect = True (line 4988)--
    Vars::GameMenu = false; //--GameMenu = False (line 4989)--

    /*
        skipping these cause lunadll handles this stuff instead

        --
            BitBlt myBackBuffer, 0, 0, ScreenW, ScreenH, 0, 0, 0, vbWhiteness (line 4990)
            BitBlt frmMain.hdc, 0, 0, frmMain.ScaleWidth, frmMain.ScaleHeight, 0, 0, 0, vbWhiteness (line 4991)
        --
    */

    // stop music
    Functions::StopMusic(); //--StopMusic (line 4992)--
    
    /*
        skipping these cause lunadll handles this stuff instead

        --
            DoEvents (line 4993)
            Sleep 500 (line 4994)
        --
    */
    // load the world
    Functions::OpenWorld(fullPathVB6); //--OpenWorld SelectWorld(selWorld).WorldPath & SelectWorld(selWorld).WorldFile (line 4995)--

    // load the save file data
    if (gEpisodeMain.FindSaves(fullPthNoWorldFileWithEndSlashS, Vars::selSave) >= 0) //--If SaveSlot(selSave) >= 0 Then (line 4996)--
    {
        // blank out intro filename if the episode already has a save file and the intro was already played
        if(!Vars::NoMap) //--If NoMap = False Then StartLevel = "" (line 4997)--
        {
            Vars::StartLevel = "";
        }

        Functions::LoadGame(); //--LoadGame (line 4998)--
    } //--End If (line 4999)--

    // get if the illparkwhereiwant cheat is active
    if(Vars::WorldUnlock) //--If WorldUnlock = True Then (line 5000)--
    {
        // get all paths
        for (int i = 1; i <= Vars::numWorldPaths; i++) //--For A = 1 To numWorldPaths (line 5001)--
        {
            tempLocation = Vars::WorldPath[i].Location; //--tempLocation = WorldPath(A).Location (line 5002)--

            //--With tempLocation (line 5003)--

            tempLocation.X = tempLocation.X + 4; //--.X = .X + 4 (line 5004)--
            tempLocation.Y = tempLocation.Y + 4; //--.Y = .Y + 4 (line 5005)--
            tempLocation.Width = tempLocation.Width - 8; //--.Width = .Width - 8 (line 5006)--
            tempLocation.Height = tempLocation.Height - 8; //--.Height = .Height - 8 (line 5007)--

            //--End With (line 5008)--

            // set to active
            Vars::WorldPath[i].Active = true; //--WorldPath(A).Active = True (line 5009)--

            // now get sceneries
            for (int j = 1; j <= Vars::numScenes; j++) //--For B = 1 To numScenes (line 5010)--
            {
                // check the collision of paths and sceneries
                if(gEpisodeMain.CheckCollision(tempLocation, Vars::Scene[j].Location)) //--If CheckCollision(tempLocation, Scene(B).Location) Then Scene(B).Active = False (line 5011)--
                {
                    // make any scenery if collided invisible if true
                    Vars::Scene[j].Active = false;
                }
            } //--Next B (line 5012)--
        } //--Next A (line 5013)--

        // now get world levels
        for (int i = 1; i <= Vars::numWorldLevels; i++) //--For A = 1 To numWorldLevels (line 5014)--
        {
            // make them visible
             Vars::WorldLevel[i].Visible = true; //--WorldLevel(A).Active = True (line 5015)--

        } //--Next A (line 5016)--

    } //--End If (line 5017)--

    // init SetupPlayers
    Functions::SetupPlayers(); //--SetupPlayers (line 5018)--

    // load the autoboot level if there's no save file, or the hub level if set
    if((Vars::StartLevel != blankString && !saveFileExists()) || (Vars::NoMap)) //--If StartLevel <> "" Then-- (line 5019)
    {
        // make the strings, wstrings, and visual basic 6 string ptr's for the world intro filename
        std::string fullPathAndAutobootLvlS = fullPthNoWorldFileWithEndSlashS + (std::string)Vars::StartLevel;
        std::wstring fullPathAndAutobootLvlWS = Str2WStr(fullPathAndAutobootLvlS);
        VB6StrPtr fullPathAndAutobootLvlVB6 = fullPathAndAutobootLvlS;

        // check to see if the autoboot level exists
        if(fileExists(fullPathAndAutobootLvlS))
        {
            // load the autoboot level from the episode if we're starting it for the first time, or the hub level if it's a hub-styled episode

            //--PlaySound 28 (line 5020)--

            //--SoundPause(26) = 200 (line 5021)--

            Vars::LevelSelect = false; //--LevelSelect = False (line 5022)--

            //--(line 5023) [left blank]--

            //--GameThing (line 5024)--

            Functions::ClearLevel(); //--ClearLevel (line 5025)--

            //--(line 5026) [left blank]--

            //--Sleep 1000 (line 5027)--

            Functions::OpenLevel(fullPathAndAutobootLvlVB6); //--OpenLevel SelectWorld(selWorld).WorldPath & StartLevel (line 5028)

        } //--End If (line 5029)--

        //--Exit Sub (line 5030)--

        // if it doesn't exist and there's no hub, error and boot the map instead after clicking "OK"
        else if(!fileExists(fullPathAndAutobootLvlS) && !Vars::NoMap)
        {
            std::wstring path = L"The level autoboot file can not be loaded. Does it even exist?\n\nAutoboot level:\n" + fullPathWS;
            MessageBoxW(0, path.c_str(), L"SMBX could not load the autoboot level", MB_ICONERROR);

            // boot the map
            Vars::LevelSelect = true;
        }
        // else if it doesn't exist and there IS a hub, error and exit instead after clicking "OK"
        else if(!fileExists(fullPathAndAutobootLvlS) && Vars::NoMap)
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

    // hide loadscreen
    LunaLoadScreenKill();
    
    //--End If (line 5031)--

    //--END MAIN RECODE--
}

int EpisodeMain::FindSaves(std::string worldPathS, int saveSlot)
{
    using namespace SMBX13;

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
    using namespace SMBX13;

    int newIdx = 0;

    // if there's at least/more than 100 episodes, use episode idx 1 and overwrite the entry
    if(Vars::NumSelectWorld >= 100)
    {
        if(isNewEpisode)
        {
            newIdx = 1;
        }
        else
        {
            newIdx = 1;
        }

        // also set the world count to 100, since the count was bigger than what is expected
        Vars::NumSelectWorld = 100;
    }
    // if there's less than 100 episodes, add a new episode into the list
    else if(Vars::NumSelectWorld < 100)
    {
        if(isNewEpisode)
        {
            // increase the episode count
            Vars::NumSelectWorld++;

            // set the new idx
            newIdx = Vars::NumSelectWorld;
        }
        else
        {
            newIdx = 1;
        }
    }

    // set the world name, path, and world filename
    auto& item = Vars::SelectWorld[newIdx];

    item.WorldName = worldNameVB6;
    item.WorldPath = worldPathVB6;
    item.WorldFile = worldFileVB6;

    // set characters that are blocked according to the wld file itself
    for (size_t i = 1; i <= 5; i++)
    {
        if (i < wldData.nocharacter.size())
        {
            item.blockChar[i] = wldData.nocharacter[i - 1];
        }
        else
        {
            item.blockChar[i] = false;
        }
    }

    return newIdx;
}

bool EpisodeMain::CheckCollision(SMBX13::Types::Location_t momentumA, SMBX13::Types::Location_t momentumB)
{
    return  ((momentumA.Y + momentumA.Height >= momentumB.Y) &&
            (momentumA.Y <= momentumB.Y + momentumB.Height) &&
            (momentumA.X <= momentumB.X + momentumB.Width) &&
            (momentumA.X + momentumA.Width >= momentumB.X));
}
