#include "SMBXFileManager.h"
#include "MiscFuncs.h"
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

#define LIMIT_BLOCKS   20000
#define LIMIT_BGOS     8000
#define LIMIT_NPCS     5000
#define LIMIT_WARPS    2000
#define LIMIT_PHYSENV  450
#define LIMIT_LAYERS   100
#define LIMIT_EVENTS   100

// TODO: Return Error?
void SMBXLevelFileBase::ReadFile(const std::wstring& fullPath)
{
    m_isValid = false; // Ensure that we are not valid right now

    *(DWORD*)0xB2B9E4 = 0; // Unknown
    native_cleanupLevel();
    native_setupSFX();
    GM_FREEZWITCH_ACTIV = 0;
    GM_CHEAT_MONEYTREE_HAVEMONEY = 0;

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


    std::wstring dir = fullPath.substr(0U, findLastSlash);
    std::wstring filename = fullPath.substr(findLastSlash + 1);
    std::wstring levelname = RemoveExtension(filename);
    std::wstring customFolder = dir + levelname;

    

    std::wcout << L"Calc --> fullPath: \"" << fullPath << "\"" << std::endl
        << L"Calc --> path: \"" << dir << "\"" << std::endl
        << L"Calc --> filename: \"" << filename << "\"" << std::endl
        << L"Calc --> levelname: \"" << levelname << "\"" << std::endl
        << L"Calc --> custom folder: \"" << customFolder << "\"" << std::endl;
    

    LevelData outData;
    if (!FileFormats::OpenLevelFile(utf8_encode(fullPath), outData)) {
        // TODO: What to do on error?
        MessageBoxA(NULL, (outData.meta.ERROR_info + "\nat line number " + 
                           std::to_string(outData.meta.ERROR_linenum)).c_str(),
                            "Error when parsing level file!", NULL);
        return;
    }
    
    dir = utf8_decode(outData.meta.path + "/");
    replaceSubStrW(dir, L"/", L"\\");
    levelname = utf8_decode(outData.meta.filename);
    std::string customFolderU8 = outData.meta.path + "/" + outData.meta.filename + "/";
    replaceSubStr(customFolderU8, "/", "\\");
    //MessageBoxA(NULL, (customFolderU8.c_str()), "kkk", NULL);
    customFolder = Str2WStr(customFolderU8);    

    FileFormats::smbx64LevelPrepare(outData);
    FileFormats::smbx64LevelSortBlocks(outData);
    FileFormats::smbx64LevelSortBGOs(outData);

    // If we are successful then set the variables
    GM_LVLFILENAME_PTR = filename;
    GM_LVLNAME_PTR = levelname;
    GM_FULLPATH = fullPath;
    GM_FULLDIR = dir;

    OnLvlLoad();

    // Init Config-Txt
    VB6StrPtr customFolderVB6 = customFolder;
    native_loadNPCConfig(&customFolderVB6);

    // Load Episode GFX
    //native_loadLocalGfx();
    // Load GFX from custom folder
    // native_loadGraphicsFromFolder(&customFolderVB6);
    
    // We should clear textures periodically for video memory reasons. At this
    // point is probably good enough.
    g_GLEngine.ClearTextures();
    // In the past, we would call native_loadLocalGfx() here, but that is now
    // being replaced.
    ImageLoader::Run();

    // Total number of stars in the level
    GM_STAR_COUNT_LEVEL = outData.stars;

    int numOfSections = outData.meta.RecentFormatVersion > 7 ? 21 : 6; // If file format is over 7, then we have 21 sections
    for(int i = 0; i < numOfSections; i++)
    {
        LevelSection& nextDataLevelSection = outData.sections[i];
        Level::SetSectionBounds(i,
                                (double)nextDataLevelSection.size_left,
                                (double)nextDataLevelSection.size_top,
                                (double)nextDataLevelSection.size_right,
                                (double)nextDataLevelSection.size_bottom);
        GM_SEC_BG_ID[i] = nextDataLevelSection.background;
        GM_SEC_ISUNDERWATER[i] = COMBOOL(nextDataLevelSection.underwater);
        GM_SEC_ISWARP[i] = COMBOOL(nextDataLevelSection.wrap_h);
        GM_SEC_MUSIC_TBL[i] = nextDataLevelSection.music_id;
        GM_SEC_NOTURNBACK[i] = COMBOOL(nextDataLevelSection.lock_left_scroll);
        GM_SEC_OFFSCREEN[i] = COMBOOL(nextDataLevelSection.OffScreenEn);
        GM_MUSIC_PATHS_PTR[i] = nextDataLevelSection.music_file;
    }

    if(outData.meta.RecentFormatVersion <= 7)//Fill others with zeros
    {
        for(int i = 6; i < 21; i++)
        {
            Level::SetSectionBounds(i, 0.0, 0.0, 0.0, 0.0);
            GM_SEC_BG_ID[i] = 0;
            GM_SEC_ISUNDERWATER[i] = -1;
            GM_SEC_ISWARP[i] = -1;
            GM_SEC_MUSIC_TBL[i] = 0;
            GM_SEC_NOTURNBACK[i] = -1;
            GM_SEC_OFFSCREEN[i] = -1;
            GM_MUSIC_PATHS_PTR[i] = "";
        }
    }
    // Copy initial values for events
    memcpy(GM_ORIG_LVL_BOUNDS, GM_LVL_BOUNDARIES, 6 * sizeof(double) * numOfSections);
    memcpy(GM_SEC_ORIG_BG_ID, GM_SEC_BG_ID, sizeof(WORD) * numOfSections);

    //Fill with zeros
    for(int i=0; i < 2; i++)
    {
        Momentum* nextPlayerPos = &GM_PLAYER_POS[i];
        memset(nextPlayerPos, 0, sizeof(Momentum));
    }

    //GM_PLAYERS_COUNT = numOfPlayers;
    int numOfPlayers = outData.players.size();
    for (int i = 0; i < numOfPlayers; i++)
    {
        const PlayerPoint& nextDataLevelPoint = outData.players[i];
        if((nextDataLevelPoint.id > 2)||(nextDataLevelPoint.id < 0))//Skip invalid player points!
            continue;
        Momentum* nextPlayerPos = &GM_PLAYER_POS[nextDataLevelPoint.id - 1];
        //memset(nextPlayerPos, 0, sizeof(Momentum));
        nextPlayerPos->x = static_cast<double>(nextDataLevelPoint.x);
        nextPlayerPos->y = static_cast<double>(nextDataLevelPoint.y);
        nextPlayerPos->width = static_cast<double>(nextDataLevelPoint.w);
        nextPlayerPos->height = static_cast<double>(nextDataLevelPoint.h);
    }

    int numOfBlocks = outData.blocks.size();
    if(numOfBlocks > LIMIT_BLOCKS)
        numOfBlocks = LIMIT_BLOCKS;
    GM_BLOCK_COUNT = numOfBlocks;
    for (int i = 0; i < numOfBlocks; i++)
    {
        Block* nextBlock = Block::Get(i + 1);
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
        nextBlock->ContentsID = static_cast<short>((nextDataLevelBlock.npc_id > 0) ? nextDataLevelBlock.npc_id + 1000 : -1 * nextDataLevelBlock.npc_id);
        nextBlock->ContentIDRelated = nextBlock->ContentsID;
        nextBlock->Slippery = COMBOOL(nextDataLevelBlock.slippery);
    }

    int numOfBGO = outData.bgo.size();
    if(numOfBGO > LIMIT_BGOS)
        numOfBGO = LIMIT_BGOS;
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
    if(numOfNPC > LIMIT_NPCS)
        numOfNPC = LIMIT_NPCS;
    GM_NPCS_COUNT = numOfNPC;
    for (int i = 0; i < numOfNPC; i++) {
        NPCMOB* nextNPC = NPC::Get(i);
        memset(nextNPC, 0, sizeof(NPCMOB));
        const LevelNPC& nextDataLevelNPC = outData.npc[i];
        nextNPC->momentum.x = static_cast<double>(nextDataLevelNPC.x);
        nextNPC->momentum.y = static_cast<double>(nextDataLevelNPC.y);
        nextNPC->directionFaced = static_cast<float>(nextDataLevelNPC.direct);
        uint64_t npcID = nextDataLevelNPC.id;
        nextNPC->id = static_cast<short>(npcID);

        // Special rules by id:
        if ((npcID == 91) || (npcID == 96) || (npcID == 283) || (npcID == 284)) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.contents);
            nextNPC->unknown_DE = static_cast<short>(nextNPC->ai1);
        }
        if (((nextDataLevelNPC.contents == 288) && (npcID == 91))// Grass contains magic potion
            || (npcID == 288)  //Magic potion
            || (npcID == 289)) //Subspace door
        {
            nextNPC->ai2 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_E0 = static_cast<short>(nextNPC->ai2);
        }
        if (npc_isflying[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = static_cast<short>(nextNPC->ai1);
        }
        if (npc_isWaterNPC[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = static_cast<short>(nextNPC->ai1);
        }
        if (npcID == 260) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->unknown_DE = static_cast<short>(nextNPC->ai1);
        }

        nextNPC->isGenerator = COMBOOL(nextDataLevelNPC.generator);
        if (nextNPC->isGenerator) {
            nextNPC->directionToGenerate = nextDataLevelNPC.generator_direct;
            nextNPC->generatorType = nextDataLevelNPC.generator_type;
            nextNPC->generatorDelaySetting = static_cast<float>(nextDataLevelNPC.generator_period);
        }

        nextNPC->talkMsg = nextDataLevelNPC.msg;
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

        short curI = static_cast<short>(i + 1);
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
                nextNPC->unknown_DE = 1;
                if (npcID == 196) {
                    nextNPC->killFlag = 9;
                }
            }

        }
    }

    int numOfDoors = outData.doors.size();
    if(numOfDoors > LIMIT_WARPS)
        numOfDoors = LIMIT_WARPS;
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
    if(numOfDoors > LIMIT_PHYSENV)
        numOfDoors = LIMIT_PHYSENV;
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
    if(numOfLayers > LIMIT_LAYERS)
        numOfLayers = LIMIT_LAYERS;
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
    if(numOfEvents > LIMIT_EVENTS)
        numOfEvents = LIMIT_EVENTS;
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
        nextEvent->Delay = static_cast<double>(nextDataEvent.trigger_timer);
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
        nextEvent->LayerHSpeed = (float)nextDataEvent.layer_speed_x;
        nextEvent->LayerVSpeed = (float)nextDataEvent.layer_speed_y;
        
        nextEvent->AutoscrollHSpeed = (float)nextDataEvent.move_camera_x;
        nextEvent->AutoscrollVSpeed = (float)nextDataEvent.move_camera_y;
        nextEvent->AutoscrollSecNum = static_cast<short>(nextDataEvent.scroll_section);
    }

    native_sort_finalize1();
    native_sort_bgo();
    native_sort_finalize2();
    
    GM_WINNING = 0;

    if((GM_ISLEVELEDITORMODE==-1)||(GM_IS_EDITOR_TESTING_NON_FULLSCREEN==-1))
    {
        // Skip this shit
        //        DEFMEM(dword_B2DD60, WORD, 0x00B2DD60);
        //        WORD v126 = dword_B2DD60;
        //        if ( !dword_B2DD60 )
        //        {
        //            //004011A0 Call this function:
        //            //__fastcall();
        //           (v119)(&Pub_Obj_Inf22_wRefCount, &dword_B2DD60);
        //            v126 = dword_B2DD60;
        //        }
    }

    if(GM_ISLEVELEDITORMODE == -1)
    {
        // Skip this shit
    }

    int doorsCnt = -1;
    native_unkDoorsCount(doorsCnt, (int)IMP_vbaNew2, (int)IMP_vbaHresultCheckObj, (int)IMP_vbaInputFile);
    
    GM_WINNING = 0;

    /* Put here extra BGOs */

/*
    local_iterator_v285 = 1;
    while ( 1 )                             // Iterate through level section boundaries
    {
        local_j = local_iterator_v285;
        if ( (signed __int16)local_iterator_v285 > 20 )
        break;
        local_iterator_v286 = (signed __int16)local_iterator_v285;
        if ( (unsigned int)(signed __int16)local_iterator_v285 >= 0x15 )
        _vbaGenerateBoundsError();
        local_iterator_v287 = (signed __int16)local_iterator_v285;
        if ( (unsigned int)(signed __int16)local_iterator_v285 >= 0x15 )
        _vbaGenerateBoundsError();
        _EAX = LVL_Bounds;
        _ECX = 48 * local_iterator_v286;
        __asm { fld     qword ptr [ecx+eax+10h]; Load Real }
        _EDX = 48 * local_iterator_v287;
        __asm
        {
        fsub    qword ptr [edx+eax+8]; Subtract Real
        fnstsw  ax              ; Store Status Word (no wait)
        }
        if ( (unsigned __int8)_EAX & 0xD )
        goto LABEL_603;
        _vbaFpR8();
        __asm
        {
        fcomp   ds:dbl_402558   ; Compare Real and Pop
        fnstsw  ax              ; Store Status Word (no wait)
        }
        if ( HIBYTE(_AX) & 0x40 )
        {
        if ( (unsigned int)(signed __int16)local_j >= 0x15 )
            _vbaGenerateBoundsError();
        if ( (unsigned int)(signed __int16)local_j >= 0x15 )
            _vbaGenerateBoundsError();
        _ECX = LVL_Bounds;
        _EAX = 48 * (signed __int16)local_j;
        __asm
        {
            fld     qword ptr [eax+ecx+8]; Load Real
            fadd    ds:dbl_4012D0   ; Add Real
        }
        local_v294 = (signed __int16)local_j;
        __asm { fstp    qword ptr [edx+ecx+8]; Store Real and Pop }
        _ECX[local_v294].top = _RT1;
        __asm { fnstsw  ax              ; Store Status Word (no wait) }
        if ( _EAX & 0xD )
            goto LABEL_603;
        }
        local_iterator_v285 = (unsigned __int16)(local_j + 1);
        if ( __OFADD__((_WORD)local_j, 1) )
        goto OverflowShitHappen_604;
    }
*/



/*
        local_i = *(_DWORD *)&BGO_COUNT_word_B25958;
        local_doorsCount_v342 = doors_count;
        local_iterator_v296 = 1;
        while ( 2 )                             // Iterate through doors and put extra BGOs
        {
          local_j = local_iterator_v296;
          if ( (signed __int16)local_iterator_v296 > local_doorsCount_v342 )
            goto FinalizeLevelLoad;
          local_preIterator_v297 = (signed __int16)local_iterator_v296 - 1;
          if ( local_preIterator_v297 >= 0xC8 )
            _vbaGenerateBoundsError();
          if ( doorData[local_preIterator_v297].warpType != 2 )
            goto DoorsIterate_CheckStarsLogic;
          local_preIterator_v298 = (signed __int16)local_iterator_v296 - 1;
          if ( local_preIterator_v298 >= 0xC8 )
            _vbaGenerateBoundsError();
          if ( doorData[local_preIterator_v298].starsRequired <= (signed __int16)Star_Count )
          {
DoorsIterate_CheckStarsLogic:
            local_preIterator_v323 = (signed __int16)local_iterator_v296 - 1;
            if ( local_preIterator_v323 >= 0xC8 )
              _vbaGenerateBoundsError();
            if ( doorData[local_preIterator_v323].warpType == 2 )// Is this warp a Door
            {
              local_preIterator_v324 = (signed __int16)local_iterator_v296 - 1;
              if ( local_preIterator_v324 >= 0xC8 )
                _vbaGenerateBoundsError();
              if ( doorData[local_preIterator_v324].isLocked == -1 )
              {
                LOWORD(local_preIterator_v324) = local_i + 1;
                if ( __OFADD__(1, (_WORD)local_i) )
                  goto OverflowShitHappen_604;
                local_i = local_preIterator_v324;
                if ( __OFADD__(1, BGO_COUNT_LOCKS) )
                  goto OverflowShitHappen_604;
                local_doorId_v325 = (signed __int16)local_iterator_v296 - 1;
                ++BGO_COUNT_LOCKS;
                if ( local_doorId_v325 >= 0xC8 )
                  _vbaGenerateBoundsError();
                local_bgoId_v326 = (signed __int16)local_preIterator_v324 - 1;
                if ( local_bgoId_v326 >= 0x1F40 )
                  _vbaGenerateBoundsError();
                _vbaStrCopy(
                  (BSTR)&BackgroundData[local_bgoId_v326].ptLayerName,
                  doorData[local_doorId_v325].ptLayerName);
                local_doorIndex_v327 = (signed __int16)local_j - 1;
                if ( local_doorIndex_v327 >= 0xC8 )
                  _vbaGenerateBoundsError();
                if ( local_bgoId_v326 >= 0x1F40 )
                  _vbaGenerateBoundsError();
                BackgroundData[local_bgoId_v326].isHidden = doorData[local_doorIndex_v327].isHidden;
                local_doorIndex_v328 = (signed __int16)local_j - 1;
                if ( local_doorIndex_v328 >= 0xC8 )
                  _vbaGenerateBoundsError();
                if ( local_bgoId_v326 >= 0x1F40 )
                  _vbaGenerateBoundsError();
                _vbaCopyBytes(48, &BackgroundData[local_bgoId_v326].momentum, &doorData[local_doorIndex_v328].entrance);
                if ( local_bgoId_v326 >= 0x1F40 )
                  _vbaGenerateBoundsError();
                BackgroundData[local_bgoId_v326].id = 98;
                if ( local_bgoId_v326 >= 0x1F40 )
                  _vbaGenerateBoundsError();
                local_v329 = 7 * local_bgoId_v326;
                LODWORD(BackgroundData[8 * local_v329 / 0x38u].momentum.width) = 0;
                HIDWORD(BackgroundData[8 * local_v329 / 0x38u].momentum.width) = 1076887552;
                goto doorsIterate_Next;
              }
            }
          }
          else
          {
            LOWORD(local_preIterator_v298) = local_i + 1;
            if ( __OFADD__(1, (_WORD)local_i) )
              goto OverflowShitHappen_604;
            local_i = local_preIterator_v298;
            if ( __OFADD__(1, BGO_COUNT_LOCKS) )
              goto OverflowShitHappen_604;
            local_v299 = (signed __int16)local_iterator_v296 - 1;
            ++BGO_COUNT_LOCKS;
            if ( local_v299 >= 0xC8 )
              _vbaGenerateBoundsError();
            local_v300 = (signed __int16)local_preIterator_v298 - 1;
            if ( local_v300 >= 0x1F40 )
              _vbaGenerateBoundsError();
            _vbaStrCopy((BSTR)&BackgroundData[local_v300].ptLayerName, doorData[local_v299].ptLayerName);
            local_doorId_v301 = (signed __int16)local_j - 1;
            if ( local_doorId_v301 >= 0xC8 )
              _vbaGenerateBoundsError();
            if ( local_v300 >= 0x1F40 )
              _vbaGenerateBoundsError();
            BackgroundData[local_v300].isHidden = doorData[local_doorId_v301].isHidden;
            if ( local_v300 >= 0x1F40 )
              _vbaGenerateBoundsError();
            local_v302 = 7 * local_v300;
            LODWORD(BackgroundData[8 * local_v302 / 0x38u].momentum.width) = 0;
            HIDWORD(BackgroundData[8 * local_v302 / 0x38u].momentum.width) = 1077411840;
            if ( local_v300 >= 0x1F40 )
              _vbaGenerateBoundsError();
            local_v303 = 7 * local_v300;
            LODWORD(BackgroundData[8 * local_v303 / 0x38u].momentum.height) = 0;
            HIDWORD(BackgroundData[8 * local_v303 / 0x38u].momentum.height) = 1077411840;
            local_v304 = (signed __int16)local_j - 1;
            if ( local_v304 >= 0xC8 )
              _vbaGenerateBoundsError();
            if ( local_v300 >= 0x1F40 )
              _vbaGenerateBoundsError();
            _EAX = doorData;
            _ECX = 144 * local_v304;
            __asm { fld     qword ptr [ecx+eax+1Ch]; Load Real }
            _ECX = BackgroundData;
            _EDX = 7 * local_v300;
            __asm { fsub    qword ptr [ecx+edx*8+18h]; Subtract Real }
            local_v309 = 7 * local_v300;
            __asm { fstp    qword ptr [ecx+edx*8+10h]; Store Real and Pop }
            _ECX[8 * local_v309 / 0x38u].momentum.y = _RT1;
            __asm { fnstsw  ax              ; Store Status Word (no wait) }
            if ( (unsigned __int8)_EAX & 0xD )
              goto LABEL_603;
            local_v311 = (signed __int16)local_j - 1;
            if ( local_v311 >= 0xC8 )
              _vbaGenerateBoundsError();
            local_v312 = (signed __int16)local_j - 1;
            if ( local_v312 >= 0xC8 )
              _vbaGenerateBoundsError();
            if ( local_v300 >= 0x1F40 )
              _vbaGenerateBoundsError();
            _EAX = doorData;
            _ECX = 144 * local_v312;
            local_v315 = 9 * local_v311;
            __asm { fld     qword ptr [ecx+eax+2Ch]; Load Real }
            if ( useSaveFDiv )
              _EAX = (SMBX_Warp *)j__adj_fdiv_m64(SLODWORD(dbl_4012D8), SHIDWORD(dbl_4012D8));
            else
              __asm { fdiv    ds:dbl_4012D8   ; Divide Real }
            _ECX = BackgroundData;
            _EDX = 16 * local_v315;
            __asm { fadd    qword ptr [edx+eax+14h]; Add Real }
            local_v318 = 8 * local_v300;
            _EAX = 7 * local_v300;
            __asm { fld     qword ptr [ecx+eax*8+20h]; Load Real }
            if ( useSaveFDiv )
              j__adj_fdiv_m64(SLODWORD(dbl_4012D8), SHIDWORD(dbl_4012D8));
            else
              __asm { fdiv    ds:dbl_4012D8   ; Divide Real }
            __asm { fsubp   st(1), st       ; Subtract Real and Pop }
            local_v320 = local_v318 - local_v300;
            __asm { fstp    qword ptr [ecx+edx*8+8]; Store Real and Pop }
            *(&_ECX->momentum.x + local_v320) = _RT1;
            __asm { fnstsw  ax              ; Store Status Word (no wait) }
            if ( _AX & 0xD )
              goto LABEL_603;
            if ( local_v300 >= 0x1F40 )
              _vbaGenerateBoundsError();
            BackgroundData[local_v300].id = 160;
doorsIterate_Next:
            LOWORD(local_iterator_v296) = local_j;
          }
          HIWORD(local_v330) = 0;
          if ( __OFADD__((_WORD)local_iterator_v296, 1) )
            goto OverflowShitHappen_604;
          LOWORD(local_v330) = local_iterator_v296 + 1;
          local_iterator_v296 = local_v330;
          continue;
        }
      }    
*/

    /* Finalizing crap */
    
    //*(_WORD *)(unkSoundVolume + 24) = 100;
    GM_UNK_SOUND_VOLUME[12] = 100;//*(WORD*)(0x00B2C590 + 24) = 100;    
    //*(_QWORD *)&dbl_B2C690 = 0i64;
    *(double*)(0x00B2C690) = 0;
    //local_getTicksCount_v346 = DeclareKernel32_GetTickCount();
    int ticks = GetTickCount() + 1000;
    //*(_QWORD *)&dbl_B2C67C = 0i64;
    GM_ACTIVE_FRAMECT = 0; //*(double*)(0x00B2C67C) = 0;
    //g_transFrameCounter = 0;
    GM_TRANS_FRAMECT = 0;
    //*(_QWORD *)&dbl_B2D72C = 0i64;
    GM_LAST_FRAME_TIME = 0;//*(double*)(0x00B2D72C) = 0;   
}
