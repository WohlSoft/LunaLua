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
#include "../SMBXInternal/Warp.h"
#include "../SMBXInternal/Water.h"
#include "../SMBXInternal/Layer.h"
#include "../SMBXInternal/SMBXEvents.h"



SMBXLevelFileBase::SMBXLevelFileBase() :
    m_isValid(false)
{}

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
        MessageBoxA(NULL, (outData.ERROR_info + "\nat line number" + std::to_string(outData.ERROR_linenum)).c_str(), "Error when parsing level file!", NULL);

        return;
    }
    

    FileFormats::smbx64LevelPrepare(outData);
    FileFormats::smbx64LevelSortBlocks(outData);
    FileFormats::smbx64LevelSortBGOs(outData);

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
    memcpy(GM_ORIG_LVL_BOUNDS, GM_LVL_BOUNDARIES, 6 * sizeof(double) * numOfSections);
    memcpy(GM_SEC_ORIG_BG_ID, GM_SEC_BG_ID, sizeof(WORD) * numOfSections);

    int numOfPlayers = outData.players.size();
    if (numOfPlayers > 2)
        numOfPlayers = 2;
    GM_PLAYERS_COUNT = numOfPlayers;
    for (int i = 0; i < numOfPlayers; i++) {
        Momentum* nextPlayerPos = &GM_PLAYER_POS[i];
        memset(nextPlayerPos, 0, sizeof(Momentum));
        const PlayerPoint& nextDataLevelPoint = outData.players[i];
        nextPlayerPos->x = static_cast<double>(nextDataLevelPoint.x);
        nextPlayerPos->y = static_cast<double>(nextDataLevelPoint.y);
        nextPlayerPos->width = static_cast<double>(nextDataLevelPoint.w);
        nextPlayerPos->height = static_cast<double>(nextDataLevelPoint.h);
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
        nextBlock->BlockType = static_cast<short>(nextDataLevelBlock.id);
        nextBlock->BlockType2 = static_cast<short>(nextDataLevelBlock.id);
        nextBlock->pDestroyEventName = nextDataLevelBlock.event_destroy;
        nextBlock->pHitEventName = nextDataLevelBlock.event_hit;
        nextBlock->pNoMoreObjInLayerEventName = nextDataLevelBlock.event_emptylayer;
        nextBlock->pLayerName = nextDataLevelBlock.layer;
        nextBlock->IsInvisible2 = COMBOOL(nextDataLevelBlock.invisible);
        nextBlock->ContentsID = static_cast<short>(nextDataLevelBlock.npc_id);
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
        nextBGO->id = static_cast<short>(nextDataLevelBGO.id);
        nextBGO->momentum.x = static_cast<double>(nextDataLevelBGO.x);
        nextBGO->momentum.y = static_cast<double>(nextDataLevelBGO.y);
        //IDEA: A way to customize BGO's sizes - while taking custom images,
        //      map their sizes into bgodef_width and bgodef_height arrays
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
        if ((npcID == 91) || (npcID == 96) || (npcID == 283) || (npcID == 284)) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.contents);
            nextNPC->unknown_DE = static_cast<short>(nextDataLevelNPC.special_data);
        }
        if ((npcID == 91) || (npcID == 288) || (npcID == 289)) {
            nextNPC->ai2 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_E0 = static_cast<short>(nextDataLevelNPC.special_data);
        }
        if (npc_isflying[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = static_cast<short>(nextDataLevelNPC.special_data);
        }
        if (npc_isWaterNPC[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = static_cast<short>(nextDataLevelNPC.special_data);
        }
        if (npcID == 260) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = static_cast<short>(nextDataLevelNPC.special_data);
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

    int numOfDoors = outData.doors.size();
    GM_WARP_COUNT = numOfDoors;
    for (unsigned int i = 0; i < outData.doors.size(); i++) {
        SMBX_Warp* nextDoor = SMBX_Warp::Get(i);
        memset(nextDoor, 0, sizeof(SMBX_Warp));
        const LevelDoor& nextDataLevelDoor = outData.doors[i];
        nextDoor->unknown_0E = -1;
        nextDoor->unknown_10 = -1;
        nextDoor->entrance.x = static_cast<double>(nextDataLevelDoor.ix);
        nextDoor->entrance.y = static_cast<double>(nextDataLevelDoor.iy);
        nextDoor->exit.x = static_cast<double>(nextDataLevelDoor.ox);
        nextDoor->exit.y = static_cast<double>(nextDataLevelDoor.oy);
        nextDoor->entranceDirection = static_cast<SMBX_EntranceDir>(nextDataLevelDoor.idirect);
        nextDoor->exitDirection = static_cast<SMBX_ExitDir>(nextDataLevelDoor.odirect);
        nextDoor->warpType = static_cast<SMBX_WarpType>(nextDataLevelDoor.type);
        nextDoor->warpToLevelFileName = nextDataLevelDoor.lname;
        nextDoor->toWarpIndex = static_cast<short>(nextDataLevelDoor.warpto);
        nextDoor->isLevelEntrance = COMBOOL(nextDataLevelDoor.lvl_i);
        nextDoor->isLevelExit = COMBOOL(nextDataLevelDoor.lvl_o);
        nextDoor->warpToWorldmapX = static_cast<short>(nextDataLevelDoor.world_x);
        nextDoor->warpToWorldmapY = static_cast<short>(nextDataLevelDoor.world_y);
        nextDoor->starsRequired = nextDataLevelDoor.star_num_hide;
        nextDoor->ptLayerName = nextDataLevelDoor.layer;
        nextDoor->isHidden = COMBOOL(nextDataLevelDoor.unknown);
        nextDoor->noYoshi = COMBOOL(nextDataLevelDoor.novehicles);
        nextDoor->allowCarryNPC = COMBOOL(nextDataLevelDoor.allownpc);
        nextDoor->entrance.width = 32.0;
        nextDoor->entrance.height = 32.0;
        nextDoor->exit.width = 32.0;
        nextDoor->exit.height = 32.0;
    }

    int numOfWater = outData.physez.size();
    GM_WATER_AREA_COUNT = numOfWater;
    for (int i = 0; i < numOfWater; i++) {
        SMBX_Water* nextWater = SMBX_Water::Get(i);
        memset(nextWater, 0, sizeof(SMBX_Water));
        const LevelPhysEnv& nextLevelPhysEnv = outData.physez[i];
        nextWater->momentum.x = static_cast<double>(nextLevelPhysEnv.x);
        nextWater->momentum.y = static_cast<double>(nextLevelPhysEnv.y);
        nextWater->momentum.width = static_cast<double>(nextLevelPhysEnv.w);
        nextWater->momentum.height = static_cast<double>(nextLevelPhysEnv.h);
        nextWater->unknown_08 = static_cast<float>(nextLevelPhysEnv.unknown);
        nextWater->isQuicksand = COMBOOL(nextLevelPhysEnv.env_type);
        nextWater->ptLayerName = nextLevelPhysEnv.layer;
    }

    int numOfLayers = outData.layers.size();
    memset(GM_LAYER_ARRAY_PTR, 0, sizeof(LayerControl) * 100);
    for (int i = 0; i < numOfLayers; i++) {
        LayerControl* nextLayer = LayerControl::Get(i);
        const LevelLayer& nextDataLevelLayer = outData.layers[i];
        nextLayer->ptLayerName = nextDataLevelLayer.name;
        nextLayer->isHidden = nextDataLevelLayer.hidden;
        if (nextLayer->isHidden) {
            short noSmoke = -1;
            native_hideLayer(&nextLayer->ptLayerName, &noSmoke);
        }
    }

    int numOfEvents = outData.events.size();
    memset(GM_EVENTS_PTR, 0, sizeof(SMBXEvent) * 100);
    for (int i = 0; i < numOfEvents; i++) {
        
        SMBXEvent* nextEvent = SMBXEvent::Get(i);
        const LevelSMBX64Event& nextDataEvent = outData.events[i];
        nextEvent->pName = nextDataEvent.name;
        nextEvent->pTextMsg = nextDataEvent.msg;
        nextEvent->SoundID = static_cast<short>(nextDataEvent.sound_id);
        nextEvent->EndGame = COMBOOL(nextDataEvent.end_game);
        
        int hideLayersNum = nextDataEvent.layers_hide.size();
        int showLayersNum = nextDataEvent.layers_show.size();
        int toggleLayersNum = nextDataEvent.layers_toggle.size();
        

        for (int i = 0; i < 21; i++) {
            if (i < hideLayersNum)
                nextEvent->pHideLayerTarg[i] = nextDataEvent.layers_hide[i];
            else
                nextEvent->pHideLayerTarg[i] = "";

            if (i < showLayersNum)
                nextEvent->pShowLayerTarg[i] = nextDataEvent.layers_show[i];
            else
                nextEvent->pShowLayerTarg[i] = "";

            if (i < toggleLayersNum)
                nextEvent->pToggleLayerTarg[i] = nextDataEvent.layers_toggle[i];
            else
                nextEvent->pToggleLayerTarg[i] = "";
        }
        
        
        int numOfSets = nextDataEvent.sets.size();
        if (numOfSets > 21)
            numOfSets = 21;

        for (int i = 0; i < numOfSets; i++) {
            const LevelEvent_Sets& nextDataEventSet = nextDataEvent.sets[i];
            nextEvent->SectionMusicID[i] = static_cast<short>(nextDataEventSet.music_id);
            nextEvent->SectionBackgroundID[i] = static_cast<short>(nextDataEventSet.background_id);
            nextEvent->SectionBounds[i].left = nextDataEventSet.position_left;
            nextEvent->SectionBounds[i].top = nextDataEventSet.position_top;
            nextEvent->SectionBounds[i].bottom = nextDataEventSet.position_bottom;
            nextEvent->SectionBounds[i].right = nextDataEventSet.position_right;
        }
        
        nextEvent->EventToTrigger = nextDataEvent.trigger;
        nextEvent->Delay = static_cast<float>(nextDataEvent.trigger_timer);
        nextEvent->NoSmoke = COMBOOL(nextDataEvent.nosmoke);
        
        nextEvent->ForceKeyboard.altJumpKeyState = COMBOOL(nextDataEvent.ctrl_altjump);
        nextEvent->ForceKeyboard.altRunKeyState = COMBOOL(nextDataEvent.ctrl_altrun);
        nextEvent->ForceKeyboard.downKeyState = COMBOOL(nextDataEvent.ctrl_down);
        nextEvent->ForceKeyboard.dropItemKeyState = COMBOOL(nextDataEvent.ctrl_drop);
        nextEvent->ForceKeyboard.jumpKeyState = COMBOOL(nextDataEvent.ctrl_jump);
        nextEvent->ForceKeyboard.leftKeyState = COMBOOL(nextDataEvent.ctrl_left);
        nextEvent->ForceKeyboard.pauseKeyState = COMBOOL(nextDataEvent.ctrl_start);
        nextEvent->ForceKeyboard.rightKeyState = COMBOOL(nextDataEvent.ctrl_right);
        nextEvent->ForceKeyboard.runKeyState = COMBOOL(nextDataEvent.ctrl_run);
        nextEvent->ForceKeyboard.upKeyState = COMBOOL(nextDataEvent.ctrl_up);
        
        nextEvent->AutoStart = COMBOOL(nextDataEvent.autostart);
        nextEvent->LayerToMove = nextDataEvent.movelayer;
        nextEvent->LayerHSpeed = nextDataEvent.layer_speed_x;
        nextEvent->LayerVSpeed = nextDataEvent.layer_speed_y;
        
        nextEvent->AutoscrollHSpeed = nextDataEvent.move_camera_x;
        nextEvent->AutoscrollVSpeed = nextDataEvent.move_camera_y;
        nextEvent->AutoscrollSecNum = static_cast<short>(nextDataEvent.scroll_section);
    }


    native_sort_finalize1();
    native_sort_bgo();
    native_sort_finalize2();

}
