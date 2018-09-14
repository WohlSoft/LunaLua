#include "../Defines.h"
#include "../Main.h"
#include "../Misc/MiscFuncs.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
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

#define LIMIT_BLOCKS   20000
#define LIMIT_BGOS     8000
#define LIMIT_NPCS     5000
#define LIMIT_WARPS    2000
#define LIMIT_PHYSENV  450
#define LIMIT_LAYERS   100
#define LIMIT_EVENTS   100
#define LIMIT_SECTIONS 21

void LunaLua_loadLevelFile(LevelData &outData,
    std::wstring fullPath)
{
    if (fullPath.empty())
        fullPath = gAppPathWCHAR + L"/untitled.lvlx";

    FileFormats::smbx64LevelPrepare(outData);
    FileFormats::smbx64LevelSortBlocks(outData);
    FileFormats::smbx64LevelSortBGOs(outData);

    size_t findLastSlash = fullPath.find_last_of(L"/\\");

    std::wstring dir = fullPath.substr(0U, findLastSlash);
    std::wstring filename = fullPath.substr(findLastSlash + 1);
    std::wstring levelname = RemoveExtension(filename);
    std::wstring customFolder = dir + levelname;

    std::wcout << L"Calc --> fullPath: \"" << fullPath << "\"" << std::endl
        << L"Calc --> path: \"" << dir << "\"" << std::endl
        << L"Calc --> filename: \"" << filename << "\"" << std::endl
        << L"Calc --> levelname: \"" << levelname << "\"" << std::endl
        << L"Calc --> custom folder: \"" << customFolder << "\"" << std::endl;

    dir = utf8_decode(outData.meta.path + "/");
    replaceSubStrW(dir, L"/", L"\\");
    levelname = utf8_decode(outData.meta.filename);
    std::string customFolderU8 = outData.meta.path + "/" + outData.meta.filename + "/";
    replaceSubStr(customFolderU8, "/", "\\");
    //MessageBoxA(NULL, (customFolderU8.c_str()), "kkk", NULL);
    customFolder = Str2WStr(customFolderU8);

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

    // If we are successful then set the variables
    GM_LVLFILENAME_PTR = filename;
    GM_LVLNAME_PTR = levelname;
    GM_FULLPATH = fullPath;
    GM_FULLDIR = dir;

    /* Must be called to trigger Lua events and initialize some other stuff */
    OnLvlLoad();

    // Init Config-Txt
    VB6StrPtr customFolderVB6 = customFolder;
    /* TODO: Implement custom NPCtxt parser over PGE File Library */
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

    int numOfSections = outData.sections.size();
    if (numOfSections > LIMIT_SECTIONS)
        numOfSections = LIMIT_SECTIONS;
    for (int i = 0; i < numOfSections; i++)
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

    //Fill others with zeros
    for (int i = numOfSections; i < LIMIT_SECTIONS; i++)
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

    // Copy initial values for events
    memcpy(GM_ORIG_LVL_BOUNDS, GM_LVL_BOUNDARIES, 6 * sizeof(double) * numOfSections);
    memcpy(GM_SEC_ORIG_BG_ID, GM_SEC_BG_ID, sizeof(WORD) * numOfSections);

    //Fill with zeros
    for (int i = 0; i < 2; i++)
    {
        Momentum* nextPlayerPos = &GM_PLAYER_POS[i];
        memset(nextPlayerPos, 0, sizeof(Momentum));
    }

    //GM_PLAYERS_COUNT = numOfPlayers;
    int numOfPlayers = outData.players.size();
    for (int i = 0; i < numOfPlayers; i++)
    {
        const PlayerPoint& nextDataLevelPoint = outData.players[i];
        if ((nextDataLevelPoint.id > 2) || (nextDataLevelPoint.id < 0))//Skip invalid player points!
            continue;
        Momentum* nextPlayerPos = &GM_PLAYER_POS[nextDataLevelPoint.id - 1];
        //memset(nextPlayerPos, 0, sizeof(Momentum));
        nextPlayerPos->x = static_cast<double>(nextDataLevelPoint.x);
        nextPlayerPos->y = static_cast<double>(nextDataLevelPoint.y);
        nextPlayerPos->width = static_cast<double>(nextDataLevelPoint.w);
        nextPlayerPos->height = static_cast<double>(nextDataLevelPoint.h);
    }

    int numOfBlocks = outData.blocks.size();
    if (numOfBlocks > LIMIT_BLOCKS)
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
    if (numOfBGO > LIMIT_BGOS)
        numOfBGO = LIMIT_BGOS;
    GM_BGO_COUNT = numOfBGO;
    for (int i = 0; i < numOfBGO; i++) {
        SMBX_BGO* nextBGO = SMBX_BGO::Get(i);
        memset(nextBGO, 0, sizeof(SMBX_BGO));
        const LevelBGO& nextDataLevelBGO = outData.bgo[i];
        nextBGO->id = static_cast<short>(nextDataLevelBGO.id);
        nextBGO->momentum.x = static_cast<double>(nextDataLevelBGO.x);
        nextBGO->momentum.y = static_cast<double>(nextDataLevelBGO.y);
        // pre-200 BGO IDs are allowed, all others are unneeded for this data
        if (nextBGO->id <= 200)
        {
            nextBGO->momentum.width = static_cast<double>(bgodef_width[nextBGO->id]);
            nextBGO->momentum.height = static_cast<double>(bgodef_height[nextBGO->id]);
        }
        nextBGO->ptLayerName = nextDataLevelBGO.layer;
    }

    int numOfNPC = outData.npc.size();
    if (numOfNPC > LIMIT_NPCS)
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
        // TODO: Check ID is in range.
        // TODO: Optionally consider special id handling if input is 38A.

        nextNPC->id = static_cast<short>(npcID);

        // Special rules by id:
        if ((npcID == 91) || (npcID == 96) || (npcID == 283) || (npcID == 284)) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.contents);
            nextNPC->ai1_initial = static_cast<short>(nextNPC->ai1);
        }
        if (((nextDataLevelNPC.contents == 288) && (npcID == 91))// Grass contains magic potion
            || (npcID == 288)  //Magic potion
            || (npcID == 289)) //Subspace door
        {
            nextNPC->ai2 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->ai2_initial = static_cast<short>(nextNPC->ai2);
        }
        if (npc_isflying[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->ai1_initial = static_cast<short>(nextNPC->ai1);
        }
        if (npc_isWaterNPC[npcID]) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->ai1_initial = static_cast<short>(nextNPC->ai1);
        }
        if (npcID == 260) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->ai1_initial = static_cast<short>(nextNPC->ai1);
        }

        if (npcID >= 293) {
            nextNPC->ai1 = static_cast<double>(nextDataLevelNPC.contents);
            nextNPC->ai1_initial = static_cast<short>(nextNPC->ai1);
            nextNPC->ai2 = static_cast<double>(nextDataLevelNPC.special_data);
            nextNPC->ai2_initial = static_cast<short>(nextNPC->ai2);
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

        nextNPC->spawnID = static_cast<short>(npcID);
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
                nextNPC->ai1_initial = 1;
                if (npcID == 196) {
                    nextNPC->killFlag = 9;
                }
            }

        }
    }

    unsigned int numOfDoors = outData.doors.size();
    if (numOfDoors > LIMIT_WARPS)
        numOfDoors = LIMIT_WARPS;
    GM_WARP_COUNT = numOfDoors;

    std::vector<SMBX_Warp*> twoWayWarps;
    for (unsigned int i = 0; i < numOfDoors; i++) {
        SMBX_Warp* nextDoor = SMBX_Warp::Get(i);
        memset(nextDoor, 0, sizeof(SMBX_Warp));
        const LevelDoor& nextDataLevelDoor = outData.doors[i];
        if (nextDataLevelDoor.two_way)
            twoWayWarps.push_back(nextDoor);
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
        nextDoor->starsRequired = static_cast<short>(nextDataLevelDoor.stars);
        nextDoor->ptLayerName = nextDataLevelDoor.layer;
        nextDoor->isLocked = COMBOOL(nextDataLevelDoor.locked);
        nextDoor->isHidden = COMBOOL(nextDataLevelDoor.unknown);
        nextDoor->noYoshi = COMBOOL(nextDataLevelDoor.novehicles);
        nextDoor->allowCarryNPC = COMBOOL(nextDataLevelDoor.allownpc);
        nextDoor->entrance.width = 32.0;
        nextDoor->entrance.height = 32.0;
        nextDoor->exit.width = 32.0;
        nextDoor->exit.height = 32.0;
    }

    if (!twoWayWarps.empty())
    {
        for (unsigned int i = 0; i < twoWayWarps.size(); i++)
        {
            if (GM_WARP_COUNT >= LIMIT_WARPS)
                break;//No more warp entries can be placed
            SMBX_Warp* prevDoor = twoWayWarps[i];
            SMBX_Warp* nextDoor = SMBX_Warp::Get(GM_WARP_COUNT++);
            memcpy(nextDoor, prevDoor, sizeof(SMBX_Warp));
            // Safely copy strings from original
            nextDoor->ptLayerName.ptr = 0;
            nextDoor->ptLayerName = prevDoor->ptLayerName;
            nextDoor->warpToLevelFileName.ptr = 0;
            nextDoor->warpToLevelFileName = prevDoor->warpToLevelFileName;
            // Swap entrance and exit
            nextDoor->entrance.x = prevDoor->exit.x;
            nextDoor->entrance.y = prevDoor->exit.y;
            nextDoor->entranceDirection = static_cast<SMBX_EntranceDir>(prevDoor->exitDirection);
            nextDoor->exit.x = prevDoor->entrance.x;
            nextDoor->exit.y = prevDoor->entrance.y;
            nextDoor->exitDirection = static_cast<SMBX_ExitDir>(prevDoor->entranceDirection);
        }
    }

    int numOfWater = outData.physez.size();
    if (numOfWater > LIMIT_PHYSENV)
        numOfWater = LIMIT_PHYSENV;
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
    if (numOfLayers > LIMIT_LAYERS)
        numOfLayers = LIMIT_LAYERS;
    memset(GM_LAYER_ARRAY_PTR, 0, sizeof(LayerControl) * LIMIT_LAYERS);
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
    if (numOfEvents > LIMIT_EVENTS)
        numOfEvents = LIMIT_EVENTS;
    memset(GM_EVENTS_PTR, 0, sizeof(SMBXEvent) * LIMIT_EVENTS);
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

        for (int i = 0; i < LIMIT_SECTIONS; i++) {
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
        if (numOfSets > LIMIT_SECTIONS)
            numOfSets = LIMIT_SECTIONS;

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
        nextEvent->LayerHSpeed = static_cast<float>(nextDataEvent.layer_speed_x);
        nextEvent->LayerVSpeed = static_cast<float>(nextDataEvent.layer_speed_y);

        nextEvent->AutoscrollHSpeed = static_cast<float>(nextDataEvent.move_camera_x);
        nextEvent->AutoscrollVSpeed = static_cast<float>(nextDataEvent.move_camera_y);
        nextEvent->AutoscrollSecNum = static_cast<short>(nextDataEvent.scroll_section);
    }

    native_sort_finalize1();
    native_sort_bgo();
    native_sort_finalize2();

    GM_WINNING = 0;

    if ((GM_ISLEVELEDITORMODE == -1) || (GM_IS_EDITOR_TESTING_NON_FULLSCREEN == -1))
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

    if (GM_ISLEVELEDITORMODE == -1)
    {
        // Skip this shit
    }

    native_unkDoorsCount();

    GM_WINNING = 0;

    /* Put here extra BGOs */

    // Decoded loop from 0x8DC04A
    // If a section is exactly 608.0 tall, it is changed to 600.0 tall.
#if 1 //May be disabled as it's not so important
    for (int i = 0; i < LIMIT_SECTIONS; i++)
    {
        // Bounds Check i < 21
        double tmp = GM_LVL_BOUNDARIES[i].bottom - GM_LVL_BOUNDARIES[i].top;
        // Check for FPU error
        if (tmp > 600.0 && tmp < 610.0)
            GM_LVL_BOUNDARIES[i].top = GM_LVL_BOUNDARIES[i].bottom - 600;
    }
#endif

    // Initialize starlocks and locks
    GM_BGO_LOCKS_COUNT = 0;
    for (unsigned int i = 0; i < GM_WARP_COUNT; i++)
    {
        SMBX_Warp* nextWarp = SMBX_Warp::Get(i);

        if (nextWarp->warpType == 2)
        {
            if ((nextWarp->starsRequired > GM_STAR_COUNT))
            {
                SMBX_BGO *nextBGO = SMBX_BGO::GetRaw(GM_BGO_COUNT + GM_BGO_LOCKS_COUNT);
                GM_BGO_LOCKS_COUNT++;
                memset(nextBGO, 0, sizeof(SMBX_BGO));
                nextBGO->ptLayerName = nextWarp->ptLayerName;
                nextBGO->isHidden = nextWarp->isHidden;
                nextBGO->momentum = nextWarp->entrance;
                nextBGO->momentum.width = 24;
                nextBGO->momentum.height = 24;
                nextBGO->momentum.x = nextWarp->entrance.x + (nextWarp->entrance.width / 2) - (nextBGO->momentum.width / 2);
                nextBGO->momentum.y = nextWarp->entrance.y - nextBGO->momentum.height;
                nextBGO->id = 160; // Setup a Starlock
            }
            else
            if (nextWarp->isLocked == -1)
            {
                SMBX_BGO *nextBGO = SMBX_BGO::GetRaw(GM_BGO_COUNT + GM_BGO_LOCKS_COUNT);
                GM_BGO_LOCKS_COUNT++;
                memset(nextBGO, 0, sizeof(SMBX_BGO));
                nextBGO->ptLayerName = nextWarp->ptLayerName;
                nextBGO->isHidden = nextWarp->isHidden;
                nextBGO->momentum = nextWarp->entrance;
                nextBGO->id = 98; // Setup a locker
                nextBGO->momentum.width = 16;
                nextBGO->momentum.height = 32;
            }
        }
    }


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
