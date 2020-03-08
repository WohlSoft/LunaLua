#include <string>
#include <algorithm>
#include "SMBXFileManager.h"
#include "../Misc/MiscFuncs.h"
#include "../Main.h"
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
    LUNALOG("Loading level from file");

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

    size_t findLastDot = filePath.find_last_of(L".", findLastSlash);
    // Append missing extension
    if (m_isValid && (findLastDot == std::wstring::npos))
    {
        if(!hasSuffix(filePath, L".lvl") && !hasSuffix(filePath, L".lvlx"))
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

