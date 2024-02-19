#include <string>
#include <algorithm>
#include "SMBXFileManager.h"
#include "../Misc/MiscFuncs.h"
#include "../Main.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Defines.h"
#include "../Rendering/ImageLoader.h"
#include "../Rendering/GL/GLEngine.h"

#include "../libs/PGE_File_Formats/file_formats.h"

#include "../SMBXInternal/Level.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/Blocks.h"
#include "../SMBXInternal/BGOs.h"
#include "../SMBXInternal/NPCs.h"
#include "../SMBXInternal/CollectedStarRecord.h"
#include "../SMBXInternal/Warp.h"
#include "../SMBXInternal/Water.h"
#include "../SMBXInternal/Layer.h"
#include "../SMBXInternal/SMBXEvents.h"

#include <DirManager/dirman.h>

SMBXLevelFileBase::SMBXLevelFileBase() :
    m_isValid(false)
{}

static void makeErrorLevel(LevelData &outData, const std::string &msg)
{
    PGESTRING metaPath = outData.meta.path;
    PGESTRING metaFilename = outData.meta.filename;

    FileFormats::CreateLevelData(outData);

    // Preserve the path/filename so things that depend on this will be happy.
    outData.meta.path = metaPath;
    outData.meta.filename = metaFilename;

    LevelSection &s = outData.sections[0];
    s.size_left = 0;
    s.size_top = 0;
    s.size_right = 800;
    s.size_bottom = 600;

    PlayerPoint player = FileFormats::CreateLvlPlayerPoint(1);
    player.x = 400;
    player.y = 300;
    outData.players.push_back(player);

    LevelSMBX64Event &levelStartEvent = outData.events[0];
    levelStartEvent.msg = msg;

    LevelNPC oneup = FileFormats::CreateLvlNpc();
    oneup.meta.array_id = outData.npc_array_id++;
    oneup.id = 90;
    oneup.x = 400;
    oneup.y = 300;
    outData.npc.push_back(oneup);
}

extern void LunaLua_loadLevelFile(LevelData &outData, std::wstring fullPath, bool isValid);

void SMBXLevelFileBase::ReadFile(const std::wstring& fullPath, LevelData &outData)
{
    FileFormats::CreateLevelData(outData);
    m_isValid = true; // Ensure that we are not valid right now
    std::wstring filePath = fullPath;
    std::replace(filePath.begin(), filePath.end(), L'/', L'\\');

    size_t findLastSlash = filePath.find_last_of(L"/\\");

    // Check if path has slash, if not then invalid
    if (m_isValid && (findLastSlash == std::wstring::npos))
    {
        m_isValid = false;
        makeErrorLevel(outData, "Can't load this level,    "
                                "because there is no slash "
                                "in the full path.");
    }

    // Append missing extension
    // Note: 1.3 always added .lvl if it didn't end with .lvl or .dat (though I've never seen a .dat level)
    std::wstring ext = getExtension(filePath);
    if (m_isValid && (ext != L".lvlx") && (ext != L".lvl") && (ext != L".dat"))
    {
        filePath.append(L".lvl");
    }

    if (!fileExists(filePath))
    {
        m_isValid = false;
        std::string msg = "Can't load this level,     "
                          "because the file does not  "
                          "exist:                     ";
        for (unsigned int i = 0; i < filePath.length(); i+=26)
        {
            msg += WStr2Str(filePath.substr(i, 26)) + " ";
        }
        makeErrorLevel(outData, msg);
    }

    // Check if Attributes is valid
    if (m_isValid && GetFileAttributesW(filePath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        m_isValid = false;
        std::string msg = "Can't load this level,     "
                          "because the file cannot be "
                          "opened:                    ";
        for (unsigned int i = 0; i < filePath.length(); i += 26)
        {
            msg += WStr2Str(filePath.substr(i, 26)) + " ";
        }
        makeErrorLevel(outData, msg);
    }

    if (m_isValid && !FileFormats::OpenLevelFile(utf8_encode(filePath), outData))
    {
        m_isValid = false;
        makeErrorLevel(outData, (" There was an error while  "
                                 "  parsing the level file!  "
                                 "                           ") +
                                outData.meta.ERROR_info + " at line " +
                                std::to_string(outData.meta.ERROR_linenum));
    }

    LunaLua_loadLevelFile(outData, filePath, m_isValid);
}


void  SMBXLevelFileBase::ReadFileMem(std::string &rawData, LevelData &outData, const std::wstring& fakePath)
{
    FileFormats::CreateLevelData(outData);
    m_isValid = true; // Ensure that we are not valid right now

    if (!FileFormats::OpenLevelRaw(rawData, utf8_encode(fakePath), outData))
    {
        m_isValid = false;
        makeErrorLevel(outData, (" There was an error while  "
                                 "  parsing the level file!  "
                                 "                           ") +
                                  outData.meta.ERROR_info + " at line " +
        std::to_string(outData.meta.ERROR_linenum));
    }

    LunaLua_loadLevelFile(outData, fakePath, m_isValid);
}

void SMBXWorldFileBase::PopulateEpisodeList()
{
    static const std::vector<std::string> wldExtensions({".wld", ".wldx"});
    std::string worldsPath = normalizeToBackslashAndResolvePath<std::string>(gAppPathUTF8 + "\\worlds");
    DirMan worldDir(worldsPath);

    // Reset episode count
    GM_EP_LIST_COUNT = 0;

    // Get list of (potential) episode folders
    std::vector<std::string> episodeDirs;
    if (!worldDir.getListOfFolders(episodeDirs))
    {
        // Couldn't read dir
        return;
    }

    // Walk all (potential) episode folders
    for (const std::string& epDirName : episodeDirs)
    {
        if (GM_EP_LIST_COUNT >= 100)
        {
            // Only fill up to episode 100 to avoid crashing.
            break;
        }

        std::string epPath = worldsPath + "\\" + epDirName;
        DirMan epDir(epPath);
        std::vector<std::string> wldFiles;
        if (!epDir.getListOfFiles(wldFiles, wldExtensions))
        {
            // Couldn't read dir
            continue;
        }

        // Walk all *.wld files in episode folder
        for (const std::string& wldName : wldFiles)
        {
            std::string wldPath = epPath + "\\" + wldName;
            WorldData wldData;

            if (!FileFormats::OpenWorldFileHeader(wldPath, wldData) || !wldData.meta.ReadFileValid)
            {
                // Couldn't read file
                continue;
            }

            if (wldData.meta.RecentFormat != WorldData::SMBX64)
            {
                // Wrong .wld format, we don't handle it right now
                continue;
            }

            // Make sure the .wld path is something we can handle
            std::wstring nonAnsiCharsFullPath = GetNonANSICharsFromWStr(Str2WStr(wldPath));
            if (!nonAnsiCharsFullPath.empty())
            {
                // The .wld path contains characters we can't currently deal with
                continue;
            }

            // Make new episode list entry
            int newIdx = GM_EP_LIST_COUNT;
            GM_EP_LIST_COUNT++;
            EpisodeListItem* ep = EpisodeListItem::GetRaw(newIdx);
            ep->episodeName = wldData.EpisodeTitle;
            ep->episodePath = epPath + "\\";
            ep->episodeWorldFile = wldName;
            for (size_t i = 0; i < 5; i++)
            {
                if (i < wldData.nocharacter.size())
                {
                    ep->blockChar[i] = COMBOOL(wldData.nocharacter[i]);
                }
                else
                {
                    ep->blockChar[i] = 0;
                }
            }
            ep->padding_16 = 0;

            // We're done after we get our first success per episode folder
            break;
        }
    }
}
