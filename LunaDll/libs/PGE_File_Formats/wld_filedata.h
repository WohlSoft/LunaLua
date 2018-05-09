/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
 *  \file wld_filedata.h
 *  \brief Contains data structure definitions for a world map file data
 */

#pragma once
#ifndef WLD_FILEDATA_H
#define WLD_FILEDATA_H

#include "pge_file_lib_globs.h"
#include "meta_filedata.h"

#ifndef DEFAULT_LAYER_NAME
#define DEFAULT_LAYER_NAME "Default"
#endif

//////////////////////World file Data//////////////////////
/**
 * @brief Terrain tile
 */
struct WorldTerrainTile
{
    //! Position X
    long x = 0;
    //! Position Y
    long y = 0;
    //! ID of terrain tile
    unsigned long id = 0;
    //! 38A: Graphics extend x
    long gfx_dx = 0;
    //! 38A: Graphics extend y
    long gfx_dy = 0;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Scenery object
 */
struct WorldScenery
{
    //! Position X
    long x = 0;
    //! Position Y
    long y = 0;
    //! ID of scenery object
    unsigned long id = 0;
    //! 38A: Graphics extend x
    long gfx_dx = 0;
    //! 38A: Graphics extend y
    long gfx_dy = 0;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Path tile
 */
struct WorldPathTile
{
    //! Position X
    long x = 0;
    //! Position Y
    long y = 0;
    //! ID of path tile
    unsigned long id = 0;
    //! 38A: Graphics extend x
    long gfx_dx = 0;
    //! 38A: Graphics extend y
    long gfx_dy = 0;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Level entrance tile
 */
struct WorldLevelTile
{
    //! Position X
    long x = 0;
    //! PositionY
    long y = 0;
    //! 38A: Graphics extend x
    long gfx_dx = 0;
    //! 38A: Graphics extend y
    long gfx_dy = 0;
    //! ID of level entrance object
    unsigned long id = 0;
    //! Target level file to enter
    PGESTRING lvlfile;
    //! Visible title of the level
    PGESTRING title;
    //! Open top path on exit code
    int top_exit = -1;
    //! Open left path on exit code
    int left_exit = -1;
    //! Open bottom path on exit code
    int bottom_exit = -1;
    //! Open right path on exit code
    int right_exit = -1;

    struct OpenCondition
    {
        //! List of additional exit codes to open this path
        PGELIST<int>    exit_codes;
        //! Expression required to open paths on any direction
        PGESTRING       expression;
    };
    //! Additional conditions to open top path
    OpenCondition top_exit_extra;
    //! Additional conditions to open left path
    OpenCondition left_exit_extra;
    //! Additional conditions to open bottom path
    OpenCondition bottom_exit_extra;
    //! Additional conditions to open right path
    OpenCondition right_exit_extra;

    struct EnterCondition
    {
        PGESTRING condition;
        PGESTRING levelIndex;
    };
    //! Conditions to enter the level
    PGELIST<EnterCondition> enter_cond;
    //! Target Warp-ID to enter level
    unsigned long entertowarp = 0;
    //! Level is always shown on map even not opened
    bool alwaysVisible = false;
    //! Show path background image under main sprite
    bool pathbg = false;
    //! Is this level entrance point is initial position of player on game start
    bool gamestart = false;
    //! Teleport to X coordinate of the world map (-1 don't teleport)
    long gotox = -1;
    //! Teleport to Y coordinate of the world map (-1 don't teleport)
    long gotoy = -1;
    //! Show big path background image under main sprite
    bool bigpathbg = false;
    //! Forcedly start level on player touches the level entrance cell
    bool forceStart = false;
    //! Ignore star coins while counting stars
    bool disableStarCoinsCount = false;
    //! Close level entrance when it has been completed
    bool destroyOnCompleting = false;
    //! Level movement will be affected by touching of rectangular areas
    bool controlledByAreaRects = false;
    //! 38A: Level-ID
    long levelID = 0;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;
    /**
     * @brief Move entrance point to another position every failed attempt to complete this level
     */
    struct Movement
    {
        struct Node
        {
            long x = 0;
            long y = 0;
            long chance = 100;
        };
        struct Line
        {
            long node1 = 0;
            long node2 = 0;
        };
        //! Nodes
        PGELIST<Node> nodes;
        //! Path that connects nodes
        PGELIST<Line> paths;
    } movement;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Music box object
 */
struct WorldMusicBox
{
    //! Position X
    long x = 0;
    //! Position Y
    long y = 0;
    //! ID of starnard music box
    unsigned long id = 0;
    //! Custom music file to play on touch
    PGESTRING music_file;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Rectangular zone to catch events
 */
struct WorldAreaRect
{
    //! Position X
    long x = 0;
    //! Position Y
    long y = 0;
    //! Width size
    long w = 32;
    //! Height size
    long h = 32;
    //! ID of music to toggle on touch
    unsigned long   music_id = 0;
    //! Custom music file to play on touch
    PGESTRING       music_file;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;

    enum Flags
    {
        //! Area takes no settings
        SETUP_NOTHING = 0x00,
        //! Toggle music on touch
        SETUP_CHANGE_MUSIC = 0x01,
        //! Changes size of viewport
        SETUP_SET_VIEWPORT = 0x02,
        //! Set route area for self-moving object
        SETUP_OBJECTS_ROUTE= 0x04,
        //! Autowalking area by current direction
        SETUP_AUTOWALKING  = 0x08,
        //! Item-triggered events
        SETUP_ITEM_EVENTS  = 0x0F
    };
    //! Special markers of this area functionality
    uint32_t flags = SETUP_NOTHING;
    //! Touch event slot
    PGESTRING eventTouch;

    enum TouchPolicy{
        TOUCH_TRIGGER_EVERY_ENTER = 0,
        TOUCH_TRIGGER_ENTER_AND_LEVEL_COMPLETION = 1,
        TOUCH_TRIGGER_ONCE = 2
    };
    uint32_t  eventTouchPolicy = TOUCH_TRIGGER_EVERY_ENTER;
    //! Break action event slot
    PGESTRING eventBreak;
    //! Warp action item use event slot
    PGESTRING eventWarp;
    //! Anchor event slot
    PGESTRING eventAnchor;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/*!
 * \brief World map specific Layer entry structure
 */
struct WorldLayer
{
    //! Name of layer
    PGESTRING name;
    //! Is this layer hidden?
    bool hidden = false;
    //! Are all members of this layer are locked for modifying?
    bool locked = false;
    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

struct WorldEvent38A
{
    //! Name of the event
    PGESTRING name;

    //TODO: Implement other fields!!!

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/**
 * @brief Custom element settings (used by 38A)
 */
struct WorldItemSetup38A
{
    enum ItemType
    {
        UNKNOWN = -1,
        TERRAIN = 0,
        SCENERY = 1,
        LEVEL   = 2,
    } type = UNKNOWN;

    int64_t id = 0;
    struct Entry
    {
        int32_t key = 0;
        int64_t value = 0;
    };
    PGELIST<Entry> data;
};

/**
 * @brief World map data structure
 */
struct WorldData
{
    //! Helper meta-data
    FileFormatMeta meta;
    /*!
     * \brief File format
     */
    enum FileFormat
    {
        //! PGE-X WLDX file format
        PGEX=0,
        //! SMBX1...64 WLD file format
        SMBX64,
        //! SMBX-38A WLD file format
        SMBX38A
    };

    //! Title of the episode
    PGESTRING EpisodeTitle = "";
    //! Disable SMBX64 Character 1
    bool nocharacter1 = false;
    //! Disable SMBX64 Character 2
    bool nocharacter2 = false;
    //! Disable SMBX64 Character 3
    bool nocharacter3 = false;
    //! Disable SMBX64 Character 4
    bool nocharacter4 = false;
    //! Disable SMBX64 Character 5
    bool nocharacter5 = false;

    //! List of disabled playable characters (boolean array by ID of each playable character)
    PGELIST<bool > nocharacter;

    inline void charactersFromS64()
    {
        nocharacter.clear();
        nocharacter.push_back(nocharacter1);
        nocharacter.push_back(nocharacter2);
        nocharacter.push_back(nocharacter3);
        nocharacter.push_back(nocharacter4);
        nocharacter.push_back(nocharacter5);
    }

    inline void charactersToS64()
    {
        nocharacter1 = nocharacter.size() > 0 ? nocharacter[0] : false;
        nocharacter2 = nocharacter.size() > 1 ? nocharacter[1] : false;
        nocharacter3 = nocharacter.size() > 2 ? nocharacter[2] : false;
        nocharacter4 = nocharacter.size() > 3 ? nocharacter[3] : false;
        nocharacter5 = nocharacter.size() > 4 ? nocharacter[4] : false;
    }

    PGESTRING IntroLevel_file = "";
    PGESTRING GameOverLevel_file = "";
    bool HubStyledWorld = false;
    bool restartlevel = false;

    //! This episode can be played in the single player only
    bool restrictSinglePlayer = false;
    //! Disable ability to toggle a playabele character on the world map
    bool restrictCharacterSwitch = false;
    //! Use stronger securty on the game save files
    bool restrictSecureGameSave = false;
    //! Don't show entreance screen on each entering into the level
    bool disableEnterScreen = false;

    enum CheatsPolicy
    {
        CHEATS_DENY_IN_LIST = false,
        CHEATS_ALLOW_IN_LIST = true
    };
    //! If unchecked - allow all cheats except listed, If checked - deny all except listed
    bool cheatsPolicy = CHEATS_DENY_IN_LIST;
    //! List of cheat codes (granted or forbidden dependent on restrictNoCheats flag state)
    PGESTRINGList cheatsList;

    enum SaveMode{
        SAVE_RESUME_AT_INTRO = -1,
        SAVE_RESUME_AT_WORLD_MAP = 0,
        SAVE_RESUME_AT_RECENT_LEVEL = 1,
    };
    //! Policy where resume game on save load
    int     saveResumePolicy = SAVE_RESUME_AT_WORLD_MAP;
    //! Automatically save game on level completing
    bool    saveAuto = false;
    //! Enable save locker
    bool    saveLocker = false;
    //! Save locker expression
    PGESTRING saveLockerEx = "";
    //! Message box shown on save locking
    PGESTRING saveLockerMsg = "";
    //! Always show any closed cells (overwise closed cells are will be hidden until player will open them)
    bool    showEverything = false;
    //! Cached total number of available stars on this episode
    unsigned int    stars = 0;
    //! 38A Inventory limit
    unsigned long   inventoryLimit = 0;

    //! Episode credits (full text area)
    PGESTRING authors = "";
    //! Episode credits (SMBX64 single-line field 1)
    PGESTRING author1 = "";
    //! Episode credits (SMBX64 single-line field 2)
    PGESTRING author2 = "";
    //! Episode credits (SMBX64 single-line field 3)
    PGESTRING author3 = "";
    //! Episode credits (SMBX64 single-line field 4)
    PGESTRING author4 = "";
    //! Episode credits (SMBX64 single-line field 5)
    PGESTRING author5 = "";

    //! List of available terrain tiles
    PGELIST<WorldTerrainTile > tiles;
    unsigned int tile_array_id = 1;
    //! List of available sceneries
    PGELIST<WorldScenery > scenery;
    unsigned int scene_array_id = 1;
    //! List of available path cells
    PGELIST<WorldPathTile > paths;
    unsigned int path_array_id = 1;
    //! List of available level cells
    PGELIST<WorldLevelTile > levels;
    unsigned int level_array_id = 1;
    //! List of available music boxes
    PGELIST<WorldMusicBox > music;
    unsigned int musicbox_array_id = 1;
    //! List of avaiable special area rectangles
    PGELIST<WorldAreaRect>  arearects;
    unsigned int arearect_array_id = 1;

    //! Array of all presented layers in this world map
    PGELIST<WorldLayer > layers;
    //! Last used Layer's array ID
    unsigned int layers_array_id = 1;

    //! Array of all 38A-formatted events in this world map
    PGELIST<WorldEvent38A> events38A;
    //! Last used Event's array ID
    unsigned int events38A_array_id = 1;

    //! SMBX-38A specific custom configs
    PGELIST<WorldItemSetup38A> custom38A_configs;

    //! Meta-data: Position bookmarks, Auto-Script configuration, etc., Crash meta-data, etc.
    MetaData metaData;

    /*
     * Editor-only parameters which are not saving into file
     */
    int     CurSection = 0;
    bool    playmusic = false;
    int     currentMusic = 0;
};

#endif // WLD_FILEDATA_H
