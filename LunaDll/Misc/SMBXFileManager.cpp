#include "SMBXFileManager.h"
#include "MiscFuncs.h"
#include "../GlobalFuncs.h"
#include "../Defines.h"

#include "../libs/PGE_File_Formats/file_formats.h"

#include "../SMBXInternal/Level.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../SMBXInternal/Blocks.h"
#include "../SMBXInternal/BGOs.h"
#include "../SMBXInternal/NPCs.h"
#include "../SMBXInternal/CollectedStarRecord.h"


SMBXLevelFileBase::SMBXLevelFileBase() :
    m_isValid(false)
{}

#include <iostream>

// TODO: Return Error?
void SMBXLevelFileBase::ReadFile(const std::wstring& fullPath)
{
    m_isValid = false; // Ensure that we are not valid right now

    *(DWORD*)0xB2B9E4 = 0; // Unknown
    native_cleanupLevel();
    native_setupSFX();
    GM_FREEZWITCH_ACTIV = 0;

    // Reset counters (values where reset by native_cleanupLevel)
    GM_BLOCK_COUNT = 0;
    GM_BGO_COUNT = 0;
    GM_NPCS_COUNT = 0;
    GM_WARP_COUNT = 0;
    GM_WATER_AREA_COUNT = 0;

    // Check if Attributes is valid
    if (GetFileAttributesW(fullPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        return;

    size_t findLastSlash = fullPath.find_last_of(L"/\\");

    // Check if path has slash, if not then invalid
    if (findLastSlash == std::wstring::npos)
        return;

    std::wstring dir = fullPath.substr(0U, findLastSlash + 1);
    std::wstring filename = fullPath.substr(findLastSlash + 1);
    std::wstring levelname = RemoveExtension(filename);
    std::wstring customFolder = dir + levelname;


    std::wcout << L"Calc --> fullPath: \"" << fullPath << "\"" << std::endl
        << L"Calc --> path: \"" << dir << "\"" << std::endl
        << L"Calc --> filename: \"" << filename << "\"" << std::endl
        << L"Calc --> levelname: \"" << levelname << "\"" << std::endl
        << L"Calc --> custom folder: \"" << customFolder << "\"" << std::endl;


    LevelData outData;
    if (!FileFormats::OpenLevelFile(utf8_encode(fullPath), outData)){
        // TODO: What to do on error?
        MessageBoxA(NULL, (outData.ERROR_info + "\n at line number" + std::to_string(outData.ERROR_linenum)).c_str(), "Error when parsing level file!", NULL);

        return;
    }

    // If we are successful then set the variables
    GM_LVLFILENAME_PTR = filename;
    GM_LVLNAME_PTR = levelname;
    GM_FULLPATH = fullPath;
    GM_FULLDIR = dir;

    // Init Config-Txt
    VB6StrPtr customFolderVB6 = customFolder;
    native_loadNPCConfig(&customFolderVB6);

    // Load Episode GFX
    native_loadLocalGfx();

    // Load GFX from custom folder
    native_loadGraphicsFromFolder(&customFolderVB6);

    // Total number of stars in the level
    GM_STAR_COUNT_LEVEL = outData.stars;

    int numOfSections = outData.RecentFormatVersion > 7 ? 21 : 6; // If file format is over 7, then we have 21 sections
    for (int i = 0; i < numOfSections; i++) {
        LevelSection& nextDataLevelSection = outData.sections[i];
        Level::SetSectionBounds(i, nextDataLevelSection.size_left, nextDataLevelSection.size_top, nextDataLevelSection.size_right, nextDataLevelSection.size_bottom);
        GM_SEC_BG_ID[i] = nextDataLevelSection.background;
        GM_SEC_ISUNDERWATER[i] = COMBOOL(nextDataLevelSection.underwater);
        GM_SEC_ISWARP[i] = COMBOOL(nextDataLevelSection.wrap_h);
        GM_SEC_MUSIC_TBL[i] = nextDataLevelSection.music_id;
        GM_SEC_NOTURNBACK[i] = COMBOOL(nextDataLevelSection.lock_left_scroll);
        GM_SEC_OFFSCREEN[i] = nextDataLevelSection.OffScreenEn;
        GM_MUSIC_PATHS_PTR[i] = nextDataLevelSection.music_file;
    }
    // Copy initial values for events
    memcpy(&GM_ORIG_LVL_BOUNDS, &GM_LVL_BOUNDARIES, 6 * sizeof(double) * numOfSections);
    memcpy(&GM_SEC_ORIG_BG_ID, &GM_SEC_BG_ID, sizeof(WORD) * numOfSections);


    int numOfPlayers = outData.players.size();
    if (numOfPlayers > 2)
        numOfPlayers = 2;
    GM_PLAYERS_COUNT = numOfPlayers;
    for (int i = 0; i < numOfPlayers; i++) {
        PlayerMOB* nextPlayer = Player::Get(i);
        memset(nextPlayer, 0, sizeof(nextPlayer));
        const PlayerPoint& nextDataLevelPoint = outData.players[i];
        nextPlayer->FacingDirection = nextDataLevelPoint.direction;
        nextPlayer->momentum.x = static_cast<double>(nextDataLevelPoint.x);
        nextPlayer->momentum.y = static_cast<double>(nextDataLevelPoint.y);
        nextPlayer->momentum.width = static_cast<double>(nextDataLevelPoint.w);
        nextPlayer->momentum.height = static_cast<double>(nextDataLevelPoint.h);
    }

    int numOfBlocks = outData.blocks.size();
    GM_BLOCK_COUNT = numOfBlocks;
    for (int i = 0; i < numOfBlocks; i++)
    {
        Block* nextBlock = Block::Get(i);
        memset(nextBlock, 0, sizeof(Block));
        const LevelBlock& nextDataLevelBlock = outData.blocks[i];
        nextBlock->momentum.x = static_cast<double>(nextDataLevelBlock.x);
        nextBlock->momentum.y = static_cast<double>(nextDataLevelBlock.y);
        nextBlock->momentum.width = static_cast<double>(nextDataLevelBlock.w);
        nextBlock->momentum.height = static_cast<double>(nextDataLevelBlock.h);
        nextBlock->BlockType = nextDataLevelBlock.id;
        nextBlock->BlockType2 = nextDataLevelBlock.id;
        nextBlock->pDestroyEventName = nextDataLevelBlock.event_destroy;
        nextBlock->pHitEventName = nextDataLevelBlock.event_hit;
        nextBlock->pNoMoreObjInLayerEventName = nextDataLevelBlock.event_emptylayer;
        nextBlock->pLayerName = nextDataLevelBlock.layer;
        nextBlock->IsHidden = COMBOOL(nextDataLevelBlock.invisible);
        nextBlock->IsInvisible2 = COMBOOL(nextDataLevelBlock.invisible);
        nextBlock->IsInvisible3 = COMBOOL(nextDataLevelBlock.invisible);
        nextBlock->ContentsID = nextDataLevelBlock.npc_id;
        // Special rules for special npcs
        auto& contentIDOfBlock = nextBlock->ContentsID;
        if (contentIDOfBlock == 100)
            contentIDOfBlock = 1009;
        if (contentIDOfBlock == 102)
            contentIDOfBlock = 1014;
        if (contentIDOfBlock == 103)
            contentIDOfBlock = 1034;
        if (contentIDOfBlock == 105)
            contentIDOfBlock = 1095;
        nextBlock->ContentIDRelated = contentIDOfBlock;
        nextBlock->Slippery = COMBOOL(nextDataLevelBlock.slippery);
    }

    int numOfBGO = outData.bgo.size();
    GM_BGO_COUNT = numOfBGO;
    for (int i = 0; i < numOfBGO; i++) {
        SMBX_BGO* nextBGO = SMBX_BGO::Get(i);
        memset(nextBGO, 0, sizeof(SMBX_BGO));
        const LevelBGO& nextDataLevelBGO = outData.bgo[i];
        nextBGO->id = nextDataLevelBGO.id;
        nextBGO->momentum.x = static_cast<double>(nextDataLevelBGO.x);
        nextBGO->momentum.y = static_cast<double>(nextDataLevelBGO.y);
        nextBGO->momentum.width = static_cast<double>(bgodef_width[nextBGO->id]);
        nextBGO->momentum.height = static_cast<double>(bgodef_height[nextBGO->id]);
        nextBGO->ptLayerName = nextDataLevelBGO.layer;
    }

    int numOfNPC = outData.npc.size();
    GM_NPCS_COUNT = numOfNPC;
    for (int i = 0; i < numOfNPC; i++) {
        NPCMOB* nextNPC = NPC::Get(i);
        memset(nextNPC, 0, sizeof(NPCMOB));
        const LevelNPC& nextDataLevelNPC = outData.npc[i];
        nextNPC->momentum.x = static_cast<double>(nextDataLevelNPC.x);
        nextNPC->momentum.y = static_cast<double>(nextDataLevelNPC.y);
        nextNPC->directionFaced = static_cast<float>(nextDataLevelNPC.direct);
        int npcID = nextDataLevelNPC.id;
        nextNPC->id = npcID;

        // Special rules by id:
        if (npcID == 91 || npcID == 96 || npcID == 283 || npcID == 284) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = nextDataLevelNPC.special_data;
        }
        if (npcID == 91 || npcID == 288 || npcID == 289) {
            nextNPC->ai2 = static_cast<double>(nextDataLevelNPC.special_data2);
            nextNPC->unknown_E0 = nextDataLevelNPC.special_data2;
        }
        if (npc_isflying[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = nextDataLevelNPC.special_data;
        }
        if (npc_isWaterNPC[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = nextDataLevelNPC.special_data;
        }
        if (npcID == 260) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = nextDataLevelNPC.special_data;
        }

        nextNPC->isGenerator = COMBOOL(nextDataLevelNPC.generator);
        if (nextNPC->isGenerator) {
            nextNPC->directionToGenerate = nextDataLevelNPC.generator_direct;
            nextNPC->generatorType = nextDataLevelNPC.generator_type;
            nextNPC->generatorDelaySetting = static_cast<float>(nextDataLevelNPC.generator_period);
        }

        nextNPC->talkMsg = nextDataLevelNPC.event_talk;
        nextNPC->friendly = COMBOOL(nextDataLevelNPC.friendly);
        nextNPC->dontMove = COMBOOL(nextDataLevelNPC.nomove);
        nextNPC->dontMove2 = nextNPC->dontMove;
        nextNPC->legacyBoss = COMBOOL(nextDataLevelNPC.is_boss);
        
        nextNPC->layerName = nextDataLevelNPC.layer;
        nextNPC->activateEventLayerName = nextDataLevelNPC.event_activate;
        nextNPC->deathEventName = nextDataLevelNPC.event_die;
        nextNPC->talkEventName = nextDataLevelNPC.event_talk;
        nextNPC->noMoreObjInLayerEventName = nextDataLevelNPC.event_emptylayer;
        nextNPC->attachedLayerName = nextDataLevelNPC.attach_layer;

        nextNPC->spawnID = npcID;
        nextNPC->momentum.width = static_cast<double>(npc_width[npcID]);
        nextNPC->momentum.height = static_cast<double>(npc_height[npcID]);
        memcpy(&nextNPC->spawnMomentum, &nextNPC->momentum, sizeof(Momentum));
        nextNPC->spawnDirection = nextNPC->directionFaced;
        nextNPC->offscreenCountdownTimer = 1;
        nextNPC->unknown_124 = -1;
        nextNPC->unknown_14C = 1;

        short curI = static_cast<short>(i);
        native_updateNPC(&curI);

        if (npcID == 97 || npcID == 196) {
            bool markStarAsGotten = false;
            // Search for the star in the star database
            for (int j = 0; j < GM_STAR_COUNT; j++) {
                SMBX_CollectedStarRecord* record = SMBX_CollectedStarRecord::Get(j);
                if (record->levelFileName == GM_LVLFILENAME_PTR) {
                    if (record->section == nextNPC->currentSection || record->section == -1) {
                        markStarAsGotten = true;
                        break;
                    }
                }
            }

            if (markStarAsGotten) 
            {
                nextNPC->ai1 = 1.0;
                nextNPC->unknown_DE;
                if (npcID == 196) {
                    nextNPC->killFlag = 9;
                }
            }

        }

    }



}
