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
 *  \file lvl_filedata.h
 *  \brief Contains data structure definitions for a level file data
 */

#pragma once
#ifndef LVL_FILEDATA_H
#define LVL_FILEDATA_H

#include "pge_file_lib_globs.h"
#include "meta_filedata.h"
#include "pge_ff_units.h"

#ifndef DEFAULT_LAYER_NAME
#define DEFAULT_LAYER_NAME "Default"
#endif

//////////////////////Level file Data//////////////////////
/*!
 * \brief Level specific Section entry structure. Defines prererences of one section
 */
struct LevelSection
{
    //! ID of section (starts from 0)
    int id = 0;
    //! Y-Position of top side of section
    long size_top = 0;
    //! Y-Position of bottom side of section
    long size_bottom = 0;
    //! X-Position of left side of section
    long size_left = 0;
    //! X-Position of right side of section
    long size_right = 0;
    //! ID of default music in this section (starts from 1, 0 is silence)
    unsigned int music_id = 0;
    //! [UNUSED] RGBA defines color of background if image is not defined.
    long bgcolor = 16291944;
    //! Enables horisontal wrap
    bool wrap_h = false;
    //! Enables vertical wrap
    bool wrap_v = false;
    //! Level will be exited when playable character will exit out of screen
    bool OffScreenEn = false;
    //! ID of background image of this section
    unsigned int background = 0;
    //! Enable lock of walking to left direction
    bool lock_left_scroll = false;
    //! Enable lock of walking to right direction
    bool lock_right_scroll = false;
    //! Enable lock of walking to up direction
    bool lock_up_scroll = false;
    //! Enable lock of walking to down direction
    bool lock_down_scroll = false;
    //! Sets default physical environment of this section is - water
    bool underwater = false;
    //! Custom music file which will be playd if music ID defined to "Custom" music id
    PGESTRING music_file;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Recent camera X position
    long PositionX = -10;
    //! Recent camera Y position
    long PositionY = -10;
};

/*!
 * \brief Level specific Player spawn point entry definition structure.
 */
struct PlayerPoint
{
    //! Defined ID of player
    unsigned int id = 0;
    //! X-position of player spawn point
    long x = 0;
    //! Y-position of player spawn point
    long y = 0;
    //! Height of player spawn point (used to calculate position of bottom to place playable character correctly)
    long h = 32;
    //! Wodth of player spawn point (used to calculate position of bottom to place playable character correctly)
    long w = 24;
    //! Initial direction of playable character (-1 is left, 1 is right, 0 is right by default)
    int direction = 1;
    //! User data pointer, Useful in the editors to have direct pointer to pre-placed elements
    void *userdata = nullptr;
};

/*!
 * \brief Level specific Block entry structure. Defines preferences of each block
 */
struct LevelBlock
{
    //! X position of block
    long x = 0;
    //! Y position of block
    long y = 0;
    //! Height of sizable block (takes no effect on non-sizable blocks in PGE Engine,
    //! but takes effect in the SMBX Engine)
    long h = 0;
    //! Width of sizable block (takes no effect on non-sizable blocks in PGE Engine,
    //! but takes effect in the SMBX Engine)
    long w = 0;
    //! Automatically scale cropping of the block size (if false, sprite will be cropped to fit into defined size)
    bool autoscale = true;
    //! ID of block type defined in the lvl_blocks.ini
    unsigned long id = 0;
    //! ID of the included NPC (0 - empty, <0 - number of coins, >0 - NPC-ID of included NPC)
    long npc_id = 0;
    //! Block is invizible until player will impacted to it at bottom side
    bool invisible = false;
    //! Block has a splippery surface
    bool slippery = false;
    //! Use the special AI to make block be movable
    uint32_t    motion_ai_id = 0;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING   layer = DEFAULT_LAYER_NAME;
    //! 38A: Custom graphic file base name (default is empty)
    PGESTRING   gfx_name;
    //! 38A: Graphics extend x
    long gfx_dx = 0;
    //! 38A: Graphics extend y
    long gfx_dy = 0;
    //! Trigger event on destroying of this block
    PGESTRING event_destroy;
    //! Trigger event on hiting of this block
    PGESTRING event_hit;
    //! Trigger event on destroying of this block and at sametime parent layer has no more other objects
    PGESTRING event_emptylayer;
    //! Trigger eveny when block entering into vizible screen area
    PGESTRING event_on_screen;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
    //! Array-ID is an unique key value identificates each unique block object.
    //! Re-counts on each file reloading
    //unsigned int array_id;
    //! Recent array index where block was saved (used to speed-up settings synchronization)
    //unsigned int index;
    //! User data pointer, Useful in the editors to have direct pointer to pre-placed elements
    //void* userdata;
};


/*!
 * \brief Level specific Background object entry structure. Defines preferences of each Background object
 */
struct LevelBGO
{
    /*
     * SMBX64
     */
    //! X position of Background Object
    long x = 0;
    //! Y position of Background Object
    long y = 0;
    //! ID of background object type defined in the lvl_bgo.ini
    unsigned long id =0 ;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;

    /*
     * Extended
     */
    //! 38A: Graphics extend x
    long gfx_dx = 0;
    //! 38A: Graphics extend y
    long gfx_dy = 0;
    /*!
     * \brief Z order Modes of background objects
     */
    enum zmodes
    {
        //! Background-2 (under sizable blocks)
        Background2 = -2,
        //! Background-1 (over sizable blocks, under regular blocks)
        Background1 = -1,
        //! Use config default Z-Mode (which defined in the lvl_bgo.ini)
        ZDefault = 0,
        //! Foreground-1 (over regular blocks, but under lava blocks and foreground NPC's)
        Foreground1 = 1,
        //! Foreground-2 (over everything)
        Foreground2 = 2
    };

    //! Z-Mode of displaying of BGO
    int    z_mode = ZDefault;
    //! Z-Offset relative to current of Z-value
    double z_offset = 0.0;

    /*! SMBX64 Order priority. Defines priority for ordering of entries in array of BGO's before writing into a SMBX-lvl file. \
        -1 - use system default order priority */
    long smbx64_sp = -1;
    /*
     * Editor-only parameters which are not saving into file
     */
    //! Automatically calculated value of SMBX64 Order priority
    long smbx64_sp_apply = -1;
    //! Helper meta-data
    ElementMeta meta;
};


/*!
 * \brief Level specific NPC entry structure. Defines preferences of each NPC
 */
struct LevelNPC
{
    //! X position of NPC
    long x = 0;
    //! Y position of NPC
    long y = 0;
    //! Initial direction of NPC (-1 left, 1 right, 0 left or right randomly)
    int direct = -1;
    //! ID of NPC type defined in the lvl_npc.ini
    uint64_t  id = 0;
    //! 38A: Custom graphic file base name (default is empty)
    PGESTRING gfx_name;
    //! 38A: Graphics extend x
    long gfx_dx = 0;
    //! 38A: Graphics extend y
    long gfx_dy = 0;
    //! <Container-NPC specific!> Contained NPC in this container.
    long contents = 0;
    //! User-data integer #1 used for configuring some NPC-AI's (kept for SMBX64)
    long special_data = 0;
    //! User-data integer #2 used for configuring some NPC-AI's (kept for SMBX64)
    long special_data2 = 0;
    //! Makes a generator of this NPC
    bool generator = false;
    /*!
     * \brief List of NPC Generator directions
     */
    enum GeneratorDirection
    {
        //! Custom NPC Generator direction
        NPC_GEN_CENTER = 0,
        //! Up NPC Generator direction
        NPC_GEN_UP = 1,
        //! Left NPC Generator direction
        NPC_GEN_LEFT = 2,
        //! Down NPC Generator direction
        NPC_GEN_DOWN = 3,
        //! Right NPC Generator direction
        NPC_GEN_RIGHT = 4,
        //! Up-Left NPC Generator direction
        NPC_GEN_UP_LEFT = 9,
        //! Left-Down NPC Generator direction
        NPC_GEN_LEFT_DOWN = 10,
        //! Down-Right NPC Generator direction
        NPC_GEN_DOWN_RIGHT = 11,
        //! Right-Up NPC Generator direction
        NPC_GEN_RIGHT_UP = 12
    };
    //! Generator direction [1] up, [2] left, [3] down, [4] right
    int generator_direct = NPC_GEN_UP;
    /*!
     * \brief NPC Generator types list
     */
    enum GeneratorTypes
    {
        NPC_GENERATOR_APPEAR = 0,
        NPC_GENERATOR_WARP = 1,
        NPC_GENERATPR_PROJECTILE = 2,
        NPC_GENERATPR_CUSTOM1 = 3,
        NPC_GENERATPR_CUSTOM2 = 4
    };
    //! Generator type 0 - instant appearence, 1 - warp, 2 - projectile shoot
    int generator_type = NPC_GENERATOR_WARP;
    //! Original unit type
    PGE_FileLibrary::TimeUnit generator_period_orig_unit = PGE_FileLibrary::TimeUnit::Decisecond;
    //! Generator's delay between each shoot in deci-seconds
    int  generator_period = 20;
    //! Generator's delay between each shoot in original units
    long generator_period_orig = 130;
    //! Generator's custom angle in degrees (applying if generator direction is equal to 0 [center])
    double generator_custom_angle = 0.0;
    //! Generator's branches (how much need spawn at one loop) (applying if generator direction is equal to 0 [center])
    int generator_branches = 1;
    //! Generator's range of multi-shooting (how much need spawn at one loop) (applying if generator direction is equal to 0 [center])
    double generator_angle_range = 360.0;
    //! Generator's initial NPC's speed (how much need spawn at one loop) (applying if generator direction is equal to 0 [center])
    double generator_initial_speed = 10.0;
    //! Talking message. If not empty, NPC will be talkable
    PGESTRING msg;
    //! NPC will not communicate and will not collide with playable characters
    bool friendly = false;
    //! NPC will stay idle and will always keep look to the playable character
    bool nomove = false;
    //! Enables some boss-specific NPC-AI features (Used by NPC-AI)
    bool is_boss = false;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;
    //! Trigger event on actiovation of this NPC (on appearence on screen)
    PGESTRING event_activate;
    //! Trigger event on death of this NPC
    PGESTRING event_die;
    //! Trigger event on player's attempt to talk with this NPC
    PGESTRING event_talk;
    //! Trigger event on destroying of this block and at sametime parent layer has no more other objects
    PGESTRING event_emptylayer;
    //! Trigger event when player will grab this NPC
    PGESTRING event_grab;
    //! Trigger event every game logic loop
    PGESTRING event_nextframe;
    //! Trigger event when player will touch this NPC
    PGESTRING event_touch;
    //! Attach layer to this NPC. All memberes of this layer are will follow to motion of this NPC.
    PGESTRING attach_layer;
    //! Variable name where NPC-ID will be written
    PGESTRING send_id_to_variable;

    /*
     * Editor-only parameters which are not saving into file
     */
    //!< Is this NPC a star (Copying from lvl_npc.ini config on file read). Stars are special bonus which required by player to be able enter into some doors/warps
    bool is_star = false;
    //! Helper meta-data
    ElementMeta meta;
};

/*!
 * \brief Level specific Warp entry structure. Defines preferences of each Warp entry
 */
struct LevelDoor
{
    //! X position of Entrance point
    long ix = 0;
    //! Y position of Entrance point
    long iy = 0;
    //! [Editing only, is not saving into file] is entrance point placed to the level
    bool isSetIn = false;

    //! X position of Exit point
    long ox = 0;
    //! Y position of Exit point
    long oy = 0;
    //! [Editing only, is not saving into file] is exit point placed to the level
    bool isSetOut = false;
    /*!
     * \brief List of possible entrance directions
     */
    enum EntranceDirectopn
    {
        ENTRANCE_UP = 1,
        ENTRANCE_LEFT = 2,
        ENTRANCE_DOWN = 3,
        ENTRANCE_RIGHT = 4
    };
    //! Direction of entrance point: [3] down, [1] up, [2] left, [4] right (not same as exit!)
    int idirect = ENTRANCE_UP;
    /*!
     * \brief List of possible exit direction values
     */
    enum ExitDirectopn
    {
        EXIT_DOWN = 1,
        EXIT_RIGHT = 2,
        EXIT_UP = 3,
        EXIT_LEFT = 4
    };
    //! Direction of exit point: [1] down [3] up [4] left [2] right (not same as entrance!)
    int odirect = EXIT_DOWN;
    /*!
     * \brief Type of warp: Instant teleport, pipe or door
     */
    enum WarpType
    {
        WARP_INSTANT = 0,
        WARP_PIPE = 1,
        WARP_DOOR = 2,
        WARP_PORTAL = 3
    };
    //! Warp type: [1] pipe, [2] door, [0] instant (zero velocity-X after exit), [3] portal (instant with Keeping of velocities)
    int type = WARP_INSTANT;
    enum WarpTransitEffect
    {
        TRANSIT_NONE,
        TRANSIT_SCROLL,
        TRANSIT_FADE,
        TRANSIT_CIRCLE_FADE
    };
    //! Transition effect
    int transition_effect = TRANSIT_NONE;
    //! Target level filename (Exit from this leven and enter to target level)
    PGESTRING lname;
    //! Warp Array-ID in the target level (if 0 - enter into target level at spawn point)
    long warpto = 0;
    //! Level entrance (this point can be used only as entrance point where player will enter into level)
    bool lvl_i = false;
    //! Level exit (entering into this warp will trigger exiting of level)
    bool lvl_o = false;
    //! Target World map X coordinate
    long world_x = -1;
    //! Target World map Н coordinate
    long world_y = -1;
    //! Stars/Leeks required to be allowed for enter into this warp
    int stars = 0;
    //! Message if player has no necessary stars/leeks collected
    PGESTRING stars_msg;
    //! Don't show stars number in the target level
    bool star_num_hide = false;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;
    //! [Unused] resrved boolean flag, always false
    bool unknown = false;
    //! Unmount all vehicles when player tried to enter into this warp. (all vehucles are will be returned back on exiting from level)
    bool novehicles = false;
    //! Allows player to move through this warp carried NPC's
    bool allownpc = false;
    //! Player need to carry a key to be allowed to enter into this warp
    bool locked = false;
    //! Player need to explode lock with a bomb to enter into this warp
    bool need_a_bomb = false;
    //! Hide the entry scene
    bool hide_entering_scene = false;
    //! Allows player to move through this warp carried NPC's to another level
    bool allownpc_interlevel = false;
    //! Required special state of playable character allowed to enter this warp
    bool special_state_required = false;
    //! Length of entrance zone: How wide will be entrance point
    unsigned int length_i = 32u;
    //! Length of exit zone: How wide will be exit point
    unsigned int length_o = 32u;
    //! Trigger event on enter
    PGESTRING event_enter;
    //! Is this warp a two-way (possible to enter from both sides)
    bool two_way = false;
    //! Cannon shoot warp exit
    bool  cannon_exit = false;
    //! Cannon shoot projectile speed (pixels per 1/65 seconds)
    double cannon_exit_speed = 10.0;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
    //! User data pointer for entrance, Useful in the editors to have direct pointer to pre-placed elements
    void *userdata_enter = nullptr;
    //! User data pointer for exit, Useful in the editors to have direct pointer to pre-placed elements
    void *userdata_exit = nullptr;
};

/*!
 * \brief  Level specific Physical Environment entry structure. Defines preferences of each Physical Environment entry
 */
struct LevelPhysEnv
{
    //! X position of physical environment zone
    long x = 0;
    //! Y position of physical environment zone
    long y = 0;
    //! Height of physical environment zone
    long h = 0;
    //! Width of physical environment zone
    long w = 0;
    //! [Unused] reserved long integer value, always 0
    long unknown = 0;

    enum EnvTypes
    {
        ENV_WATER                   = 0,
        ENV_QUICKSAND               = 1,
        ENV_CUSTOM_LIQUID           = 2,
        ENV_GRAVITATIONAL_FIELD     = 3,
        ENV_TOUCH_EVENT_ONCE_PLAYER = 4,
        ENV_TOUCH_EVENT_PLAYER      = 5,
        ENV_TOUCH_EVENT_ONCE_NPC    = 6,
        ENV_TOUCH_EVENT_NPC         = 7,
        ENV_CLICK_EVENT             = 8,
        ENV_COLLISION_SCRIPT        = 9,
        ENV_CLICK_SCRIPT            = 10,
        ENV_COLLISION_EVENT         = 11,
        ENV_AIR                     = 12,
        ENV_TOUCH_EVENT_ONCE_NPC1   = 13,
        ENV_TOUCH_EVENT_NPC1        = 14,
        ENV_NPC_HURTING_FIELD       = 15
    };
    //! Enable quicksand physical environment, overwise water physical environment
    int env_type = ENV_WATER;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer = DEFAULT_LAYER_NAME;
    //! Custom liquid friction (works with "custom liquid" type)
    double friction = 0.5;
    //! Acceleration direction (works with "custom liquid" type)
    double accel_direct = -1.0;
    //! Acceleration (works with "custom liquid" type)
    double accel = 0.0;
    //! Max velocity (works with "custom liquid" type)
    double max_velocity = 0.0;
    //! Touch event (or script) name
    PGESTRING touch_event;
    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/*!
 * \brief Level specific Layer entry structure
 */
struct LevelLayer
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

/*!
 * \brief Level events specific settings set per each section
 */
struct LevelEvent_Sets
{
    enum SetActions
    {
        LESet_Nothing = -1,
        LESet_ResetDefault = -2,
    };

    //!ID of section
    long id = -1;

    //! Set new Music ID in this section (-1 - do nothing, -2 - reset to defaint, >=0 - set music ID)
    long music_id = LESet_Nothing;
    //! Set new Custom Music File path
    PGESTRING music_file;
    //! Set new Background ID in this section (-1 - do nothing, -2 - reset to defaint, >=0 - set background ID)
    long background_id = LESet_Nothing;

    //! Change section borders if not (-1 - do nothing, -2 set default, any other values - set X position of left section boundary)
    long position_left = LESet_Nothing;
    //! Change Y position of top section boundary
    long position_top = 0;
    //! Change Y position of bottom section boundary
    long position_bottom = 0;
    //! Change X position of right section boundary
    long position_right = 0;

    //! Ariphmetical expression calculates position X
    PGESTRING expression_pos_x;
    //! Ariphmetical expression calculates position Y
    PGESTRING expression_pos_y;
    //! Ariphmetical expression calculates width of section
    PGESTRING expression_pos_w;
    //! Ariphmetical expression calculates height of section
    PGESTRING expression_pos_h;

    //! Enable autoscroll for this section
    bool  autoscrol = false;
    //! X speed of autoscrool
    float autoscrol_x = 0.0;
    //! Y speed of autoscrool
    float autoscrol_y = 0.0;

    //! Ariphmetical expression calculates autoscrool X
    PGESTRING expression_autoscrool_x;
    //! Ariphmetical expression calculates autoscrool y
    PGESTRING expression_autoscrool_y;
};

/*!
 * \brief Movable layer configuration
 */
struct LevelEvent_MoveLayer
{
    //! Name of moving layer
    PGESTRING name = "";
    //! Speed X of layer
    double speed_x = 0.0;
    //! Speed Y of layer
    double speed_y = 0.0;

    //! Expression for X speed/coordinate
    PGESTRING expression_x;

    //! Expression for Y speed/coordinate
    PGESTRING expression_y;

    //! List of available layer motion ways
    enum LayerMotionWay
    {
        //! Set moving speed
        LM_Speed = 0,
        //! Move to coordinate offset relative to initial position
        LM_Coordinate = 1
    };
    //! Way to do layer motion
    int way = LM_Speed;
};

/*!
 * \brief Spawn effect task declaration
 */
struct LevelEvent_SpawnEffect
{
    //! Effect ID to spawn
    long id = 0;
    //! Spawn effect at X
    long x = 0;
    //! Spawn effect at Y
    long y = 0;
    //! Expression for X position
    PGESTRING expression_x;
    //! Expression for Y position
    PGESTRING expression_y;
    //! Initial speed X (pixels per 1/65 second)
    double speed_x = 0.0;
    //! Initial speed Y (pixels per 1/65 second)
    double speed_y = 0.0;
    //! Expression for X speed
    PGESTRING expression_sx;
    //! Expression for Y speed
    PGESTRING expression_sy;
    //! Spawn effect with gravity (to decide whether the effects are affected by gravity)
    bool gravity = false;
    //! Frame speed of spawned effect
    int fps = -1;//Default FPS
    //! Life time of effect (1/65 seconds) (effect existed over this time will be destroyed.)
    int max_life_time = -1;//Default life time
};

/*!
 * \brief Declaration of spawn NPC command
 */
struct LevelEvent_SpawnNPC
{
    //! Spawn NPC-ID
    long id = 0;
    //! Spawn NPC at X position
    long x = 0;
    //! Spawn NPC at Y position
    long y = 0;
    //! Spawn NPC with initial keenetic speed X
    double speed_x = 0.0;
    //! Spawn NPC with initial keenetic speed Y
    double speed_y = 0.0;
    //! Expression for X position
    PGESTRING expression_x;
    //! Expression for Y position
    PGESTRING expression_y;
    //! Expression for X speed
    PGESTRING expression_sx;
    //! Expression for Y speed
    PGESTRING expression_sy;
    //! Additional special parameter: advanced settings of generated npc
    long special = 0;
};

/*!
 * \brief Declaration of variable update command
 */
struct LevelEvent_UpdateVariable
{
    //! Variable name to update
    PGESTRING name;
    //! Variable value to update
    PGESTRING newval;
};

struct LevelEvent_SetTimer
{
    //! Enable timer
    bool  enable = false;
    //! Time left (ticks)
    long  count = 0;
    //! Lenght of every tick (miliseconds per every tick)
    double interval = 1000.0;
    enum CountDirection
    {
        DIR_REVERSE = 0,
        DIR_FORWARD = 1
    };
    //! Count direction
    int count_dir = DIR_REVERSE;
    //! Show timer
    bool show = false;
};

/*!
 * \brief Level specific Classic Event Entry structure
 */
struct LevelSMBX64Event
{
    //! Name of event
    PGESTRING name;
    //! Message box to spawn if not empty
    PGESTRING msg;
    //! Sound ID to play if not zero
    long sound_id = 0;
    //! Trigger end of game and go to credits screen if not zero
    long end_game = 0;

    //! Don't show smoke effect on show/hide/toggle layer visibility
    bool nosmoke = false;
    //! List of layers to hide
    PGESTRINGList layers_hide;
    //! List of layers to show
    PGESTRINGList layers_show;
    //! List of layers to toggle (hide visible and show invisible)
    PGESTRINGList layers_toggle;

    //! List of section settings per each section to apply
    PGELIST<LevelEvent_Sets > sets;
    //! Trigger another event if not empty
    PGESTRING trigger;
    //! Original unit type
    PGE_FileLibrary::TimeUnit trigger_timer_unit = PGE_FileLibrary::TimeUnit::Decisecond;
    //! Trigger another event after time in deci-seconds
    long trigger_timer = 0;
    //! Trigger another event after time in original units
    long trigger_timer_orig = 0;
    //! Hold key controllers holding
    bool ctrls_enable = false;
    /*!
     * \brief Check is one of control keys pressed
     * \return true if one of keys is pressed
     */
    bool ctrlKeyPressed();
    //! Hold "Up" key controllers
    bool ctrl_up = false;
    //! Hold "Down" key controllers
    bool ctrl_down = false;
    //! Hold "Left" key controllers
    bool ctrl_left = false;
    //! Hold "Right" key controllers
    bool ctrl_right = false;
    //! Hold "Jump" key controllers
    bool ctrl_jump = false;
    //! Hold "Alt-jump" key controllers
    bool ctrl_altjump = false;
    //! Hold "Run" key controllers
    bool ctrl_run = false;
    //! Hold "Alt-run" key controllers
    bool ctrl_altrun = false;

    //! Hold "Start" key controllers
    bool ctrl_start = false;
    //! Hold "Drop" key controllers
    bool ctrl_drop = false;

    //! Temporary lock player controllers input
    bool ctrl_lock_keyboard = false;

    enum AutoStartCond
    {
        AUTO_None           = 0,
        AUTO_LevelStart     = 1,
        AUTO_MatchAll       = 3,
        AUTO_CallAndMatch   = 4
    };
    //! Trigger event automatically on level startup
    int autostart = AUTO_None;
    //! Conditional expression for event autostart
    PGESTRING autostart_condition;
    //! Array of extra moving layers
    PGELIST<LevelEvent_MoveLayer >  moving_layers;
    //! Effects to spawn;
    PGELIST<LevelEvent_SpawnEffect> spawn_effects;
    //! NPC's to spawn;
    PGELIST<LevelEvent_SpawnNPC>    spawn_npc;
    //! Variables to update;
    PGELIST<LevelEvent_UpdateVariable> update_variable;
    //! Timer definition
    LevelEvent_SetTimer timer_def;
    //! Trigger script by name
    PGESTRING   trigger_script;
    //! Trigger API function by ID (SMBX-38A)
    int         trigger_api_id = 0;

    //! Install layer motion settings for layer if is not empt
    PGESTRING movelayer;
    //! Set layer X motion in pixels per 1/65
    double layer_speed_x = 0.0;
    //! Set layer Y motion in pixels per 1/65
    double layer_speed_y = 0.0;
    //! Setup autoscrool X speed in pixels per 1/65
    double move_camera_x = 0.0;
    //! Setup autoscrool Y speed in pixels per 1/65
    double move_camera_y = 0.0;
    //! Setup autoscrool for section ID (starts from 0)
    long scroll_section = 0;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! Helper meta-data
    ElementMeta meta;
};

/*!
 * \brief Level Variable entry
 */
struct LevelVariable
{
    PGESTRING name;
    PGESTRING value;
};

/*!
 * \brief Level Script entry
 */
struct LevelScript
{
    enum ScriptLangs
    {
        LANG_LUA = 0,
        LANG_AUTOCODE = 1,
        LANG_TEASCRIPT
    };
    PGESTRING name;
    PGESTRING script;
    int       language = LANG_LUA;
};

/**
 * @brief Custom element settings (used by 38A)
 */
struct LevelItemSetup38A
{
    enum ItemType
    {
        UNKNOWN = -1,
        BLOCK = 0,
        BGO = 1,
        EFFECT = 2,
    } type = UNKNOWN;

    int64_t id = 0;
    struct Entry
    {
        int32_t key = 0;
        int64_t value = 0;
    };
    PGELIST<Entry> data;
};

/*!
 * \brief Level data structure. Contains all available settings and element lists on the level.
 */
struct LevelData
{
    /*
     * Level header
     */
    //! Total number of stars on the level
    int stars = 0;

    //!Helper meta-data
    FileFormatMeta meta;

    /*!
     * \brief File format
     */
    enum FileFormat
    {
        //! PGE-X LVLX File Format
        PGEX = 0,
        //! SMBX1...64 LVL File format
        SMBX64,
        //! SMBX-38A LVL File Format
        SMBX38A
    };

    //! Understandable name of the level
    PGESTRING LevelName;
    //! If not empty, start this level when player was failed
    PGESTRING open_level_on_fail;
    //! Target WarpID (0 - regular entrance, >=1 - WarpID of entrance)
    unsigned int open_level_on_fail_warpID = 0;

    struct MusicOverrider
    {
        enum Type {
            LEVEL   = 0,
            SPECIAL = 1
        };
        Type        type = Type::LEVEL;
        uint32_t    id = 0;
        PGESTRING   fileName;
    };

    //! Override default musics
    PGELIST<MusicOverrider > music_overrides;
    //! Override default sound effects
    PGELIST<MusicOverrider > sound_overrides;

    /*
     * Level data
     */
    //! Sections settings array
    PGELIST<LevelSection > sections;

    //! Player spawn points array
    PGELIST<PlayerPoint > players;

    //! Array of all presented Blocks in this level
    PGELIST<LevelBlock > blocks;
    //! Last used block's array ID
    unsigned int blocks_array_id = 1;

    //! Array of all presented Background objects in this level
    PGELIST<LevelBGO > bgo;
    //! Last used Background object array ID
    unsigned int bgo_array_id = 1;

    //! Array of all presented NPCs in this level
    PGELIST<LevelNPC > npc;
    //! Last used NPC's array ID
    unsigned int npc_array_id = 1;

    //! Array of all presented Warp Entries in this level
    PGELIST<LevelDoor > doors;
    //! Last used warp's array ID
    unsigned int doors_array_id = 1;

    //! Array of all presented Physical Environment Zones in this level
    PGELIST<LevelPhysEnv > physez;
    //! Last used Physical Environment Zone's array ID
    unsigned int physenv_array_id = 1;

    //! Array of all presented layers in this level
    PGELIST<LevelLayer > layers;
    //! Last used Layer's array ID
    unsigned int layers_array_id = 1;

    //! Array of all presented events in this level
    PGELIST<LevelSMBX64Event > events;
    //! Last used Event's array ID
    unsigned int events_array_id = 1;

    PGELIST<LevelVariable> variables;
    PGELIST<LevelScript>   scripts;

    //! SMBX-38A specific custom configs
    PGELIST<LevelItemSetup38A> custom38A_configs;

    //! Meta-data: Position bookmarks, Auto-Script configuration, etc., Crash meta-data, etc.
    MetaData metaData;

    /*
     * Editor-only parameters which are not saving into file
     */
    //! ID of currently editing section
    int CurSection = 0;
    //! is music playing button pressed?
    bool playmusic = false;

    /*
     * Helpful functions
     */
    /*!
     * \brief Checks is event with specified title exist in this level
     * \param title Event name which need to check for existsing
     * \return true if requested event is exists
     */
    bool eventIsExist(PGESTRING title);
    /*!
     * \brief Checks is layer with specified title exist in this level
     * \param title Layer name which need to check for existsing
     * \return true if requested event is exists
     */
    bool layerIsExist(PGESTRING title);
};



#endif // LVL_FILEDATA_H
