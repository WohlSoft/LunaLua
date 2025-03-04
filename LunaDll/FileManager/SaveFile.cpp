#include "SaveFile.h"
#include "../libs/PGE_File_Formats/save_filedata.h"
#include "../libs/PGE_File_Formats/file_formats.h"
#include "../libs/PGE_File_Formats/smbx64.h"
#include "../libs/PGE_File_Formats/smbx64_macro.h"

#include "../SMBXInternal/Types.h"
#include "../SMBXInternal/Variables.h"
#include "../Misc/RuntimeHook.h"
#include "../Misc/SafeFPUControl.h"
#include "../GlobalFuncs.h"
#include "../Globals.h"
#include "../LuaMain/LunaPathValidator.h"

void __stdcall SMBXSaveFile::Save()
{
    SafeFPUControl fpuExceptionClear; // Make sure FPU exceptions are cleared when the function exits

    // Don't save if cheater set
    if (SMBX13::Vars::Cheater) return;

    // Hook for saving the game
    if (gLunaLua.isValid()) {
        std::shared_ptr<Event> saveGameEvent = std::make_shared<Event>("onSaveGame", false);
        saveGameEvent->setDirectEventName("onSaveGame");
        saveGameEvent->setLoopable(false);
        gLunaLua.callEvent(saveGameEvent);
    }

    // Save active characters to templates
    for (int i = SMBX13::Vars::numPlayers; i >= 1; i--)
    {
        runtimeHookCharacterIdCopyPlayerToTemplate(SMBX13::Vars::Player[i].Character, i);
    }

    // Clean up star list if blank level names?
    {
        using SMBX13::Vars::numStars;
        using SMBX13::Vars::Star;
        for (int i = numStars; i >= 1; i--)
        {
            if (Star[i].level.length() == 0)
            {
                if (numStars > i)
                {
                    Star[i] = Star[numStars];
                    Star[numStars].level = L"";
                    Star[numStars].Section = 0;
                }
                numStars--;
            }
        }
    }

    // Use PGE FL functions to help convert to string
    // It doesn't have SMBX64 format write function, so converting to GamesaveData wouldn't make
    // sense anyway.

    // Get output string to store data in
    PGESTRING rawStr;
    {
        PGE_FileFormats_misc::RawTextOutput out(&rawStr);
        using namespace SMBX13::Vars;
        out << SMBX64::WriteSInt(64);
        out << SMBX64::WriteFloat(Lives); // Yes, it's a float
        out << SMBX64::WriteSInt(Coins);
        out << SMBX64::WriteFloat(WorldPlayer[1].Location.X);
        out << SMBX64::WriteFloat(WorldPlayer[1].Location.Y);
        for (int i = 1; i <= 5; i++)
        {
            const auto& savedChar = SavedChar[i];
            out << SMBX64::WriteSInt(savedChar.State);
            out << SMBX64::WriteSInt(savedChar.HeldBonus);
            out << SMBX64::WriteSInt(savedChar.Mount);
            out << SMBX64::WriteSInt(savedChar.MountType);
            out << SMBX64::WriteSInt(savedChar.Hearts);
        }
        out << SMBX64::WriteSInt(curWorldMusic);
        out << SMBX64::WriteCSVBool(BeatTheGame);
        for (int i = 1; i <= numWorldLevels; i++)
        {
            out << SMBX64::WriteCSVBool(WorldLevel[i].Active);
        }
        out << "\"next\"\n";
        for (int i = 1; i <= numWorldPaths; i++)
        {
            out << SMBX64::WriteCSVBool(WorldPath[i].Active);
        }
        out << "\"next\"\n";
        for (int i = 1; i <= numScenes; i++)
        {
            out << SMBX64::WriteCSVBool(Scene[i].Active);
        }
        out << "\"next\"\n";
        for (int i = 1; i <= numStars; i++)
        {
            out << SMBX64::WriteStr(Star[i].level);
            out << SMBX64::WriteSInt(Star[i].Section);
        }
        out << "\"next\"\n";
        out << SMBX64::WriteSInt(MaxWorldStars);
    }

    // Use CRLF line endings
    rawStr = PGE_ReplSTRING(rawStr, "\n", "\r\n");

    // Write data to file in atomic fashion
    std::wstring worldPath = SMBX13::Vars::SelectWorld[SMBX13::Vars::selWorld].WorldPath;
    std::wstring saveFilePath = worldPath + L"save" + std::to_wstring(SMBX13::Vars::selSave) + L".sav";
    LunaPathValidator::Result* ret = LunaPathValidator::GetForThread().CheckPath(WStr2Str(saveFilePath).c_str());
    if (ret && ret->canWrite)
    {
        writeFileAtomic(saveFilePath, rawStr.c_str(), rawStr.size());
    }
}

//void __stdcall SMBXSaveFile::Load()
//{
//    // Get path
//    std::wstring saveFilePath = SMBX13::Vars::SelectWorld[SMBX13::Vars::selWorld].WorldPath + L"save" + std::to_wstring(SMBX13::Vars::selSave) + L".sav";
//
//    PGESTRING rawData;
//    if (!readFileToStr(saveFilePath, rawData))
//    {
//        // Failed to load
//        return;
//    }
//
//    GamesaveData data{};
//    data.meta.ERROR_info.clear();
//    PGE_FileFormats_misc::RawTextInput file;
//    if (!file.open(&rawData, WStr2Str(saveFilePath)))
//    {
//        // Failed to open
//        return;
//    }
//    if (!FileFormats::ReadSMBX64SavFile(file, data))
//    {
//        // Failed to parse
//        return;
//    }
//}
