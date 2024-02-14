#include "GameAutostart.h"
#include "../Misc/AsmPatch.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Misc/MiscFuncs.h"
#include "../FileManager/SMBXFileManager.h"
#include "../libs/PGE_File_Formats/file_formats.h"

// Patch for making introLoop skip to the right place
static auto skipIntoPatch = PATCH(0x8CA6A4).JMP(0x8CD13C).NOP_PAD_TO_SIZE<7>();

static bool wstrEqualIgnoreCase(std::wstring a1, std::wstring a2)
{
    std::transform(a1.begin(), a1.end(), a1.begin(), towlower);
    std::transform(a2.begin(), a2.end(), a2.begin(), towlower);
    return a1 == a2;
}

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
    if ((selectedEpisode == "") && (selectedWldPath == L""))
        return false;

    // If we have a selected wld path, get the path and filename seperately
    bool usingWldPath = false;
    std::wstring wldPath = L"";
    std::wstring wldFile = L"";
    if (selectedWldPath != L"")
    {
        // Get the full path if necessary
        std::wstring fullPath = resolveCwdOrWorldsPath(selectedWldPath);
        if (!fileExists(fullPath))
        {
            fullPath = L"";
        }
        if (fullPath.length() == 0)
        {
            // Invalid level name
            std::wstring path = L"SMBX could not find the world map file \"" + selectedWldPath + L"\"";
            LunaMsgBox::ShowW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
            _exit(1);
        }

        std::wstring::size_type lastSlash = fullPath.rfind(L'\\');
        if (lastSlash != std::wstring::npos)
        {
            wldPath = fullPath.substr(0, lastSlash+1);
            wldFile = fullPath.substr(lastSlash+1);
            usingWldPath = true;
        }

        std::wstring nonAnsiCharsEpisode = GetNonANSICharsFromWStr(wldPath);
        if (!nonAnsiCharsEpisode.empty())
        {
            std::wstring path = L"The episode path has characters which are not compatible with the system default Windows ANSI code page. This is not currently supported. Please rename or move your episode folder.\n\nUnsupported characters: " + nonAnsiCharsEpisode + L"\n\nPath:\n" + wldPath;
            LunaMsgBox::ShowW(0, path.c_str(), L"SMBX does not support episode path", MB_ICONERROR);
            _exit(1);
        }

        std::wstring nonAnsiCharsFullPath = GetNonANSICharsFromWStr(fullPath);
        if (!nonAnsiCharsFullPath.empty())
        {
            std::wstring path = L"The world map filename has characters which are not compatible with the system default Windows ANSI code page. This is not currently supported. Please rename your world map file.\n\nUnsupported characters: " + nonAnsiCharsFullPath + L"\n\nPath:\n" + fullPath;
            LunaMsgBox::ShowW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
            _exit(1);
        }

        WorldData wldData;
        if (!FileFormats::OpenWorldFileHeader(WStr2Str(fullPath), wldData) || !wldData.meta.ReadFileValid)
        {
            std::wstring path = L"The world map file header cannot be parsed.\n\nPath:\n" + fullPath;
            LunaMsgBox::ShowW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
            _exit(1);
        }

        if (wldData.meta.RecentFormat != WorldData::SMBX64)
        {
            std::wstring path = L"The world map file is in the wrong format. It must be saved in SMBX64 format.\n\nPath:\n" + fullPath;
            LunaMsgBox::ShowW(0, path.c_str(), L"SMBX could not load world map", MB_ICONERROR);
            _exit(1);
        }
    }

    //load all episodes
    SMBXWorldFileBase::PopulateEpisodeList();
    VB6StrPtr toSearchItem = selectedEpisode;
    std::vector<EpisodeListItem*> allEpisodes = EpisodeListItem::GetAll();
    bool foundEpisode = false;
    unsigned int epIdx = 0;
    for (unsigned int i = 0; i < allEpisodes.size(); ++i) {
        EpisodeListItem* item = allEpisodes[i];

        // Match by name
        if (!usingWldPath && (item->episodeName == toSearchItem))
        {
            epIdx = i;
            foundEpisode = true;
            break;
        }

        // Match by filename
        if (usingWldPath &&
            wstrEqualIgnoreCase(item->episodePath, wldPath) &&
            wstrEqualIgnoreCase(item->episodeWorldFile, wldFile)
            )
        {
            epIdx = i;
            foundEpisode = true;
            break;
        }
    }

    // Make new entry if needed
    if (!foundEpisode && usingWldPath)
    {
        if (GM_EP_LIST_COUNT < 100)
        {
            // We can add an entry at the end
            epIdx = GM_EP_LIST_COUNT;
            GM_EP_LIST_COUNT++;
        }
        else
        {
            // We have to overwite and entry
            epIdx = 0;
        }
        EpisodeListItem* item = EpisodeListItem::Get(epIdx);

        item->episodeName = L"External Episode";
        item->episodePath = wldPath;
        item->episodeWorldFile = wldFile;
        for (int i = 0; i < 5; i++)
        {
            item->blockChar[i] = 0;
        }
        item->padding_16 = 0;

        foundEpisode = true;
    }

    if (foundEpisode)
    {
        EpisodeListItem* item = EpisodeListItem::Get(epIdx);

        //Slot selection/Singleplayer
        GM_CUR_MENUTYPE = (singleplayer ? 10 : 20);
        //first Character
        GM_CUR_MENUPLAYER1 = static_cast<int>(firstCharacter);
        //second Character
        GM_CUR_MENUPLAYER2 = static_cast<int>(secondCharacter);
        //Load the selected episode
        GM_CUR_MENULEVEL = epIdx + 1;

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

    return false;
}

GameAutostart GameAutostart::createGameAutostartByIniConfig(IniProcessing &reader)
{
    GameAutostart autostarter;
    reader.beginGroup("autostart");
    autostarter.selectedWldPath = L"";
    autostarter.selectedEpisode = reader.value("episode-name", "").toString();
    autostarter.singleplayer    = reader.value("singleplayer", true).toBool();
    autostarter.playerCount     = reader.value("players", 1).toInt();
    autostarter.firstCharacter  = static_cast<Characters>(reader.value("character-player1", 1).toInt());
    autostarter.secondCharacter = static_cast<Characters>(reader.value("character-player2", 2).toInt());
    autostarter.saveSlot        = reader.value("save-slot", 1).toInt();
    reader.endGroup();
    return autostarter;
}

GameAutostart GameAutostart::createGameAutostartByStartupEpisodeSettings(const StartupEpisodeSettings& settings)
{
    GameAutostart autostarter;
    autostarter.selectedWldPath = settings.wldPath;
    autostarter.selectedEpisode = "";
    autostarter.singleplayer = (settings.players == 1);
    autostarter.playerCount = settings.players;
    autostarter.firstCharacter = static_cast<Characters>(settings.character1);
    autostarter.secondCharacter = static_cast<Characters>(settings.character2);
    autostarter.saveSlot = settings.saveSlot;
    return autostarter;
}

GameAutostart GameAutostart::createGameAutostartByManualSettings(std::wstring wldPath, int players, int character1, int character2, int saveSlot)
{
    GameAutostart autostarter;
    autostarter.selectedWldPath = wldPath;
    autostarter.selectedEpisode = "";
    autostarter.singleplayer = (players == 1);
    autostarter.playerCount = players;
    autostarter.firstCharacter = static_cast<Characters>(character1);
    autostarter.secondCharacter = static_cast<Characters>(character2);
    autostarter.saveSlot = saveSlot;
    return autostarter;
}

/*static*/ void GameAutostart::ClearAutostartPatch()
{
    skipIntoPatch.Unapply();
}
