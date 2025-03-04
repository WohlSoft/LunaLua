//*** Defines.h - It has defines in it, also basically the known RAM map. ***
#ifndef Defines_hhhh
#define Defines_hhhh

// Includes
#include <windows.h>
#include <stddef.h>
#include "Misc/VB6StrPtr.h"

extern const char* LUNALUA_VERSION;
extern const char* GAME_ENGINE;

#define LUNA_VERSION        8
#define COMPILE_PLAYGROUND  0 //See Misc/Playground.cpp
#define COMBOOL(b) (b ? -1 : 0)

enum FIELDTYPE {
    FT_INVALID = 0,
    FT_BYTE = 1,
    FT_WORD = 2,
    FT_DWORD = 3,
    FT_FLOAT = 4,
    FT_DFLOAT = 5,

    FT_MAX = 5
};

enum OPTYPE {
    OP_Assign = 0,
    OP_Add = 1,
    OP_Sub = 2,
    OP_Mult = 3,
    OP_Div = 4,
    OP_XOR = 5,
    OP_ABS = 6
};

enum COMPARETYPE {
    CMPT_EQUALS = 0,
    CMPT_GREATER = 1,
    CMPT_LESS = 2,
    CMPT_NOTEQ =3
};

enum DIRECTION {
    DIR_UP = 1,
    DIR_RIGHT = 2,
    DIR_DOWN = 3,
    DIR_LEFT = 4
};

enum PRIORITY {
    PRI_LOW = 0,
    PRI_MID,
    PRI_HIGH
};

enum WORLD_HUD_CONTROL {
    WHUD_ALL,
    WHUD_ONLY_OVERLAY,
    WHUD_NONE
};


enum LEVEL_HUD_CONTROL {
    LHUD_UNKNOWN1 // Only temporary
};


////////////////////////
/// -Shared Strcuts- ///
////////////////////////

struct Momentum
{
    double x;
    double y;
    double height;
    double width;
    double speedX;
    double speedY;
};

struct Bounds
{
    double left;
    double top;
    double bottom;
    double right;
    double unk1;
    double unk2;
};

struct KeyMap{
    short    upKeyState; //Up
    short    downKeyState; //Down
    short    leftKeyState; //Left
    short    rightKeyState; //Right
    short    jumpKeyState; //Jump
    short    altJumpKeyState; //Spin Jump
    short    runKeyState;  //Run
    short    altRunKeyState; //Alt Run
    short    dropItemKeyState; //Select/Drop Item
    short    pauseKeyState; //Pause
};

struct CharacterHitBoxData {
    short hitbox_h;
    short hitbox_h_d;
    short hitbox_w;
    short hitbox_graboff_x;
    short hitbox_graboff_y;
    short gfxoffset_x[100];
    short gfxoffset_y[100];
};

enum Characters : short {
    CHARACTER_UNKNOWN = 0,
    CHARACTER_MARIO = 1,    //DEMO
    CHARACTER_LUIGI = 2,    //IRIS
    CHARACTER_PEACH = 3,    //KOOD
    CHARACTER_TOAD = 4,    //RAOCOW
    CHARACTER_LINK = 5     //SHEATH
};

enum PowerupID : short {
    PLAYER_SMALL = 1,
    PLAYER_BIG = 2,
    PLAYER_FIREFLOWER = 3,
    PLAYER_LEAF = 4,
    PLAYER_TANOOKIE = 5,
    PLAYER_HAMMER = 6,
    PLAYER_ICE = 7
};

enum ExitType : short {
    EXITTYPE_ANY = -1,
    EXITTYPE_NONE = 0,
    EXITTYPE_CARD_ROULETTE = 1,
    EXITTYPE_BOSS = 2,
    EXITTYPE_OFFSCREEN = 3,
    EXITTYPE_SECRET = 4,
    EXITTYPE_CRYSTAL = 5,
    EXITTYPE_WARP = 6,
    EXITTYPE_STAR = 7,
    EXITTYPE_TAPE = 8
};

enum CollidersType : short {
    HARM_TYPE_JUMP = 1,            // other is 'player index'. Triggered for jumping on NPC
    HARM_TYPE_FROMBELOW = 2,       // other is 'block index'. Triggered for hit from below or pow
    HARM_TYPE_NPC = 3,             // other is 'npc index'. Triggered for thrown NPCS, bomb explosions, shells, etc
    HARM_TYPE_PROJECTILE_USED = 4, // other is 'npc index'.  Triggered on a projectile once it hits something, in case the projectile should be destroyed
    HARM_TYPE_LAVA = 6,            // other is 'block index'. Triggered for being hit by lava
    HARM_TYPE_HELD = 5,            // other is 'npc index'. Triggered by colliding with held NPCs or kicked gloombas
    HARM_TYPE_TAIL = 7,            // other is 'player index'. Triggered for being hit by tail
    HARM_TYPE_SPINJUMP = 8,        // other is 'player index'. Triggered for spinjump or statue
    HARM_TYPE_OFFSCREEN = 9,       // other is 0. Triggered when timing out offscreen
    HARM_TYPE_SWORD = 10,          // other is 'player index'. Triggered for sword or sword-beam

    // Extended types for Lua use
    HARM_TYPE_EXT_FIRE = -1,
    HARM_TYPE_EXT_ICE = -2,
    HARM_TYPE_EXT_HAMMER = -3,
};

// value passed to onNPCTransform
enum NPCTransformationCause {
    NPC_TFCAUSE_UNKNOWN = 0,
    NPC_TFCAUSE_HIT = 1,
    NPC_TFCAUSE_DESPAWN = 2,
    NPC_TFCAUSE_CONTAINER = 3,
    NPC_TFCAUSE_AI = 4,
    NPC_TFCAUSE_EATEN = 5,
    NPC_TFCAUSE_LINK = 6,
    NPC_TFCAUSE_SWITCH = 7,
};


#define GM_BASE             0x00B25000
#define GM_END              0x00B2E000

#define GM_FILE_START       0x00400000
#define FN_OFFSET2ADDR(offs) (void*)(GM_FILE_START + (DWORD)offs)

#define GM_PLAYER_KEY_UP    0
#define GM_PLAYER_KEY_DOWN  1
#define GM_PLAYER_KEY_LEFT  2
#define GM_PLAYER_KEY_RIGHT 3
#define GM_PLAYER_KEY_JUMP  4
#define GM_PLAYER_KEY_SJUMP 5
#define GM_PLAYER_KEY_X     6
#define GM_PLAYER_KEY_RUN   7
#define GM_PLAYER_KEY_SEL   8
#define GM_PLAYER_KEY_STR   9

#define DEFMEM(name, type, addr) static auto& name = *(type*)(addr); \
    static constexpr std::uintptr_t name ## _ADDR = addr; \
    static auto name ## _POINTER = (type*)(addr);
#define DEFMEM_PTR(name, type, addr) static auto name = (const type*)(addr)

// General
DEFMEM(GM_SCRN_HDC,         HDC,   0x00B25028);
DEFMEM(GM_DO_SCREENSHOT,    short, 0x00B2504C);

// Modes
DEFMEM(GM_CREDITS_MODE,     WORD,  0x00B2C89C);
DEFMEM(GM_EPISODE_MODE,     WORD,  0x00B2C5B4);      // 0xFFFF = leave current level
DEFMEM(GM_LEVEL_MODE,       WORD,  0x00B2C620);
DEFMEM(GM_TITLE_INTRO_MODE, WORD,  0x00B2C620)
/*
The modes work as followed:
GM_CREDITS_MODE == -1                        --> Credits
GM_LEVEL_MODE   == -1                        --> Intro (Main Menu)
GM_EPISODE_MODE == -1                        --> Overworld
GM_EPISODE_MODE == -1 && GM_LEVEL_MODE == -1 --> Level
*/


// Pre-Defined Strings
DEFMEM(GM_STR_NULL,         VB6StrPtr, 0x00423D00);
DEFMEM(GM_STR_MSGBOX,       VB6StrPtr, 0x00B250E4);
DEFMEM(GM_STR_CHECKPOINT,   VB6StrPtr, 0x00B250B0);     //This contains the levelname (GM_FULLPATH) of the hitted checkpoint

DEFMEM(GM_IS_EDITOR_TESTING_NON_FULLSCREEN, WORD, 0x00B250B4);
DEFMEM(GM_UNK_IS_CONNECTED, WORD,  0x00B253C4);
DEFMEM(GM_CAMERA_CONTROL,   WORD,  0x00B25130);
DEFMEM(GM_SUPERMARIO2_PLAYER_IDX, WORD, 0x00B2C896);
DEFMEM(GM_CLEAR_LEVEL_CHECKPOINT, WORD, 0x00B2C59C);
DEFMEM(GM_UNK_B2B9E4,       WORD,  0x00B2B9E4);
DEFMEM(GM_UNK_B2C5A0,       WORD,  0x00B2C5A0);
DEFMEM(GM_UNK_B2C6DA,       WORD,  0x00B2C6DA);
DEFMEM(GM_UNK_B2C8E4,       WORD,  0x00B2C8E4);
DEFMEM(GM_UNK_B2D742,       WORD,  0x00B2D742);
DEFMEM(GM_UNK_WINDOWED,     WORD,  0x00B250D8);

DEFMEM(GM_UNK_SOUND_VOLUME, WORD*, 0x00B2C590);

// NPC Settings
DEFMEM(GM_CONF_WIDTH,       WORD*, 0x00B25BA8);
DEFMEM(GM_CONF_HEIGHT,      WORD*, 0x00B25BC4);
DEFMEM(GM_CONF_GFXWIDTH,    WORD*, 0x00B25BE0);
DEFMEM(GM_CONF_GFXHEIGHT,   WORD*, 0x00B25BFC);

// Frame counters
DEFMEM(GM_TRANS_FRAMECT,  DWORD,  0x00B2C670);
DEFMEM(GM_ACTIVE_FRAMECT, double, 0x00B2C67C);

// MOB Related memory
DEFMEM(GM_NPCS_PTR,         void*, 0x00B259E8);     // +0xAD58 + 0x20  to NPCs
DEFMEM(GM_NPCS_COUNT,       WORD,  0x00B2595A);
DEFMEM(GM_PLAYERS_PTR,      void*, 0x00B25A20);
DEFMEM(GM_PLAYERS_TEMPLATE, void*, 0x00B2C91C);     // Editor Template
DEFMEM(GM_PLAYERS_COUNT,    WORD,  0x00B2595E);
DEFMEM(GM_EDIT_PLAYERS_PTR, void*, 0x00CF74D8);     // Editor Template player
DEFMEM(GM_PLAYER_POS,       Momentum*, 0x00B25148);

// Star counting
DEFMEM(GM_STAR_COUNT,       WORD, 0x00B251E0);
DEFMEM(GM_STARS_PTR,        void*, 0x00B25714);
DEFMEM(GM_STAR_COUNT_LEVEL, WORD, 0x00B2C8A8);

// HUD stuff
DEFMEM(GM_COINS,            WORD,  0x00B2C5A8);
DEFMEM(GM_PLAYER_LIVES,     FLOAT, 0x00B2C5AC);

// Menu Stuff
DEFMEM(GM_CUR_MENUCHOICE,   WORD,  0x00B2C880);      // Current menu choice
DEFMEM(GM_CUR_MENUTYPE,     WORD,  0x00B2C882);

// Menu already choosen stuff
DEFMEM(GM_CUR_MENUPLAYER1,  WORD,  0x00B2D6B8);
DEFMEM(GM_CUR_MENUPLAYER2,  WORD,  0x00B2D6BA);

DEFMEM(GM_CUR_MENULEVEL,    WORD,  0x00B2C628);

// Menu Episode List
DEFMEM(GM_EP_LIST_COUNT,    WORD,  0x00B250E8);
DEFMEM(GM_EP_LIST_PTR,      void*, 0x00B250FC);

// Interaction
DEFMEM(GM_MOUSE_X,        double,  0x00B2D6BC);
DEFMEM(GM_MOUSE_Y,        double,  0x00B2D6C4);
DEFMEM(GM_MOUSEMOVING,      WORD,  0x00B2D6D2);
DEFMEM(GM_MOUSERELEASED,    WORD,  0x00B2D6D0);
DEFMEM(GM_MOUSEPRESSING,    WORD,  0x00B2D6CC);
DEFMEM(GM_KEYRELEASED,      WORD,  0x00B2C884);


// States
DEFMEM(GM_FREEZWITCH_ACTIV, WORD,  0x00B2C8B4);
DEFMEM(GM_PAUSE_OPEN,       WORD,  0x00B250E2);

DEFMEM(GM_CHEAT_MONEYTREE_HAVEMONEY, DWORD, 0x00B2C8BA);

// Camera
DEFMEM(GM_CAMERA_X,         double*, 0x00B2B984);
DEFMEM(GM_CAMERA_Y,         double*, 0x00B2B9A0);
DEFMEM(GM_CAMINFO,          void*,   0x00B25124);

// Overworld base struct
DEFMEM(GM_OVERWORLD_PTR,    void*, 0x00B2C5C8);

// Overworld Level Array
DEFMEM(GM_LEVEL_COUNT,      WORD,  0x00B25960);
DEFMEM(GM_LEVEL_BASE,       void*, 0x00B25994);


// Level related memory
DEFMEM(GM_LVLFILENAME_PTR,  VB6StrPtr, 0x00B2C5A4);   // Lvl filename
DEFMEM(GM_LVLNAME_PTR,      VB6StrPtr, 0x00B2D764);
DEFMEM(GM_FULLPATH,         VB6StrPtr, 0x00B2C618);   // Full path to current .lvl file
DEFMEM(GM_FULLDIR,          VB6StrPtr, 0x00B2C61C);   // Full path to current world dir
DEFMEM(GM_CUR_LVL,          WORD,  0x00B2C6D8);

DEFMEM(GM_NEXT_LEVEL_FILENAME, VB6StrPtr, 0x00B25720);
DEFMEM(GM_NEXT_LEVEL_WARPIDX, WORD, 0x00B2C6DA);

// Section related arrays
DEFMEM(GM_LVL_BOUNDARIES,   Bounds*, 0x00B257D4);     // 6 doubles each section, L/U/R/D/?/?
DEFMEM(GM_ORIG_LVL_BOUNDS,  Bounds*, 0x00B2587C);     // Same as above, but always the initial values. Used by events that reset level boundaries.
DEFMEM(GM_SEC_ISWARP,       short*, 0x00B257F0);
DEFMEM(GM_SEC_OFFSCREEN,    short*, 0x00B2580C);
DEFMEM(GM_SEC_NOTURNBACK,   short*, 0x00B2C5EC);
DEFMEM(GM_SEC_ISUNDERWATER, short*, 0x00B2C608);
DEFMEM(GM_SEC_CURRENT_MUSIC_ID,short, 0x00B25888);

// Background objects
DEFMEM(GM_BGO_COUNT,        WORD,  0x00B25958);
DEFMEM(GM_BGOS_PTR,         void*, 0x00B259B0);

DEFMEM(GM_BGO_LOCKS_COUNT,  WORD,  0x00B250D6);

// Warps
DEFMEM(GM_WARP_COUNT,       WORD,  0x00B258E2);
DEFMEM(GM_WARPS_PTR,        void*, 0x00B258F4);

// Water/Quicksand areas
DEFMEM(GM_WATER_AREA_COUNT, WORD,  0x00B25700);
DEFMEM(GM_WATER_AREAS_PTR,  void*, 0x00B256F4);

// Layers
DEFMEM(GM_LAYER_UNK_PTR,    void*, 0x00B259E8);
DEFMEM(GM_LAYER_ARRAY_PTR,  void*, 0x00B2C6B0);

// Events
DEFMEM(GM_EVENTS_PTR,       void*, 0x00B2C6CC);
DEFMEM(GM_EVENT_TIMES_PTR,  void*, 0x00B2D104);     // array of 100 shorts
DEFMEM(GM_EVENT_COUNT,      WORD,  0x00B2D710);

// Blocks
DEFMEM(GM_BLOCK_COUNT,      WORD,  0x00B25956);
DEFMEM(GM_BLOCKS_PTR,       void*, 0x00B25A04);
DEFMEM(GM_BLOCKS_SORTED,    WORD,  0x00B2C894);
DEFMEM(GM_BLOCK_LOOKUP_MIN, WORD*, 0x00B25758);
DEFMEM(GM_BLOCK_LOOKUP_MAX, WORD*, 0x00B25774);

// Backgrounds
DEFMEM(GM_SEC_BG_ID,     short*,  0x00B258B8); //Define for sections background id
DEFMEM(GM_SEC_ORIG_BG_ID,   WORD*, 0x00B25860);   // Same as above but used for events (initial data)
DEFMEM(GM_BG_XPOS_PTR,      double*, 0x00B2B984);

// Animations
DEFMEM(GM_ANIM_COUNT,       WORD,  0x00B2595C);
DEFMEM(GM_ANIM_PTR,         void*, 0x00B259CC);

// Tile
DEFMEM(GM_TILE_COUNT,       WORD,   0x00B257A4);
DEFMEM(GM_TILE_PTR,         void*,  0x00B25910);

// Scenery
DEFMEM(GM_SCENERY_COUNT,    WORD,   0x00B257A6);
DEFMEM(GM_SCENERY_PTR,      void*,  0x00B2592C);

// Path
DEFMEM(GM_PATH_COUNT,       WORD,   0x00B258E0);
DEFMEM(GM_PATH_PTR,         void*,  0x00B258D4);

// Musicbox
DEFMEM(GM_MUSICBOX_COUNT,   WORD,   0x00B25980);
DEFMEM(GM_MUSICBOX_PTR,     void*,  0x00B25974);




// Sound
DEFMEM(GM_MUSIC_PATHS_PTR,   VB6StrPtr*, 0x00B257B8);
DEFMEM(GM_SEC_MUSIC_TBL,         short*, 0x00B25828); // 21 shorts containing music # for each section
DEFMEM(GM_SEC_DEFAULT_MUSIC_TBL, short*, 0x00B25844);
DEFMEM(GM_MUSIC_RESTORE_PL,        WORD, 0x00B2C630); // If pswitch is active, then the music of the section of the player index is restored.


// Input
DEFMEM(GM_VKEY_TABLE_PTR,   void*,  0x00B25068);
DEFMEM(GM_VJOY_TABLE_PTR,   void*,  0x00B25084);
DEFMEM(GM_INPUTTYPE,        short*, 0x00B250A0);
DEFMEM(GM_INPUTSTR_BUF_PTR, VB6StrPtr, 0x00B2C898);

// Saves
DEFMEM(GM_CUR_SAVE_SLOT,    WORD,  0x00B2C62A);      // 1 2 or 3

// Cheats
DEFMEM(GM_PLAYER_INVULN,    WORD,   0x00B2C8C0);    // 0xFFFF = invuln
DEFMEM(GM_PLAYER_INFJUMP,   WORD,   0x00B2C8AC);    // 0xFFFF = infinite jumps
DEFMEM(GM_PLAYER_SHADOWSTAR,WORD,   0x00B2C8AA);    // 0xFFFF = shadowstar
DEFMEM(GM_CHEATED,          WORD,   0x00B2C8C4);    // 0xFFFF = cheated

// Frame counter
DEFMEM(GM_SHOW_FPS,         WORD,   0x00B25108);    // 0xFFFF = show FPS counter
DEFMEM(GM_FPS_VALUE,        double, 0x00B2510C);    // Must be zeroed on disabling FPS counter showing

// Miscs
DEFMEM(GM_GRAVITY,          WORD,  0x00B2C6F4);      // 12 = default, 0 = non-falling
DEFMEM(GM_JUMPHIGHT,        WORD,  0x00B2C6DC);      // 20 = default, higher number = higher jumps
DEFMEM(GM_JUMPHIGHT_BOUNCE, WORD,  0x00B2C6E2);      // Same as normal jumphight, but from NPC
DEFMEM(GM_EARTHQUAKE,       WORD,  0x00B250AC);      // 0 = default, higher number = bigger shaking, slowly resets to 0

DEFMEM(GM_MARIO_VS_LUIGI_T, WORD,  0x00B2D760);      // 0 = default, if higher than 0 then display text "Mario VS Luigi"
DEFMEM(GM_WINS_T,           WORD,  0x00B2D762);      // 0 = default, if higher than 0 then display text "WINS!"

DEFMEM(GM_WINNING,          WORD,  0x00B2C59E);      // 0 = not winning, if higher than 0 then winning by this win-type

DEFMEM(GM_LEVEL_EXIT_TYPE,  WORD,  0x00B2C5D4);      // 0 = didn't win, if higher then this represents exit type as saved in the world map

DEFMEM(GM_PSWITCH_COUNTER,  WORD,  0x00B2C62C);
DEFMEM(GM_PSWITCH_LENGTH,   WORD,  0x00B2C87C);

DEFMEM(GM_UNK_OV_DATABLOCK, short*,0x00B25164);     // Pointer to some kind of overworld data block involving locked character selection (not 100% sure)

DEFMEM(GM_NPC_WALKSPEED,    float, 0x00B2C86C);

//Hitbox
DEFMEM(GM_HITBOX_H_PTR,     short,0x00B2C6FC);      // player hitbox height for each character/power-up state (starts with small mario through small link, then cycles same way through each power up)
DEFMEM(GM_HITBOX_H_D_PTR,   short,0x00B2C742);      // player hitbox heights while ducking
DEFMEM(GM_HITBOX_W_PTR,     short,0x00B2C788);      // player hitbox widths
DEFMEM(GM_HITBOX_GRABOFF_X, short,0x00B2C7CE);      // player graboffset x
DEFMEM(GM_HITBOX_GRABOFF_Y, short,0x00B2C814);      // player graboffset y

DEFMEM(GM_GFXOFFSET_MARIO_X, short*, 0x00B25A3C);    // mario gfx offset x
DEFMEM(GM_GFXOFFSET_MARIO_Y, short*, 0x00B25A58);    // mario gfx offset y
DEFMEM(GM_GFXOFFSET_LUIGI_X, short*, 0x00B25A74);    // luigi gfx offset x
DEFMEM(GM_GFXOFFSET_LUIGI_Y, short*, 0x00B25A90);    // luigi gfx offset y
DEFMEM(GM_GFXOFFSET_PEACH_X, short*, 0x00B25AAC);    // peach gfx offset x
DEFMEM(GM_GFXOFFSET_PEACH_Y, short*, 0x00B25AC8);    // peach gfx offset y
DEFMEM(GM_GFXOFFSET_TOAD_X,  short*, 0x00B25AE4);    // toad gfx offset x
DEFMEM(GM_GFXOFFSET_TOAD_Y,  short*, 0x00B25B00);    // toad gfx offset y
DEFMEM(GM_GFXOFFSET_LINK_X,  short*, 0x00B25B1C);    // link gfx offset x
DEFMEM(GM_GFXOFFSET_LINK_Y,  short*, 0x00B25B38);    // link gfx offset y

//Startup Config:
DEFMEM(GM_ISLEVELEDITORMODE, WORD, 0x00B25134);
DEFMEM(GM_ISGAME,           WORD,  0x00B25046);
DEFMEM(GM_NOSOUND,          WORD,  0x00B2D734);
DEFMEM(GM_FRAMESKIP,        WORD,  0x00B2C684);


//Graphics Memory [Level]
DEFMEM(GM_GFX_BLOCKS_PTR,           HDC*, 0x00B2C95C);
DEFMEM(GM_GFX_BLOCKS_MASK_PTR,      HDC*, 0x00B2C978);
DEFMEM(GM_GFX_BLOCKS_NO_MASK,     short*, 0x00B2C010);
DEFMEM(GM_GFX_BACKGROUND2_PTR,      HDC*, 0x00B2CA00);
DEFMEM(GM_GFX_BACKGROUND2_H_PTR,  short*, 0x00B2CA44);
DEFMEM(GM_GFX_BACKGROUND2_W_PTR,  short*, 0x00B2CA60);
DEFMEM(GM_GFX_NPC_PTR,              HDC*, 0x00B2CA98);     // Array of NPC graphics HDCs, len 300, indexed by (npc.id - 1)
DEFMEM(GM_GFX_NPC_MASK_PTR,         HDC*, 0x00B2CAB4);     // Array of NPC mask graphics HDCs, len 300, indexed by (npc.id - 1)
DEFMEM(GM_GFX_NPC_H_PTR,          short*, 0X00B2CB20);
DEFMEM(GM_GFX_NPC_W_PTR,          short*, 0X00B2CB3C);
DEFMEM(GM_GFX_EFFECTS_PTR,          HDC*, 0X00B2CB74);
DEFMEM(GM_GFX_EFFECTS_MASK_PTR,     HDC*, 0X00B2CB90);
DEFMEM(GM_GFX_EFFECTS_H_PTR,      short*, 0X00B2CBFC);
DEFMEM(GM_GFX_EFFECTS_W_PTR,      short*, 0X00B2CC18);
DEFMEM(GM_GFX_BACKGROUND_PTR,       HDC*, 0X00B2CC50);
DEFMEM(GM_GFX_BACKGROUND_MASK_PTR,  HDC*, 0X00B2CC6C);
DEFMEM(GM_GFX_BACKGROUND_H_PTR,   short*, 0X00B2CCD8);
DEFMEM(GM_GFX_BACKGROUND_W_PTR,   short*, 0X00B2CCF4);
DEFMEM(GM_GFX_BACKGROUND_H_UNK_PTR, short*, 0X00B2BE4C); // ? TODO: Understand the role of this
DEFMEM(GM_GFX_BACKGROUND_W_UNK_PTR, short*, 0X00B2BE30); // ? TODO: Understand the role of this
DEFMEM(GM_GFX_MARIO_PTR,            HDC*, 0X00B2CD2C);
DEFMEM(GM_GFX_MARIO_MASK_PTR,       HDC*, 0X00B2CD48);
DEFMEM(GM_GFX_MARIO_H_PTR,        short*, 0X00B2CDB4);
DEFMEM(GM_GFX_MARIO_W_PTR,        short*, 0X00B2CDD0);
DEFMEM(GM_GFX_LUIGI_PTR,            HDC*, 0X00B2CE08);
DEFMEM(GM_GFX_LUIGI_MASK_PTR,       HDC*, 0X00B2CE24);
DEFMEM(GM_GFX_LUIGI_H_PTR,        short*, 0X00B2CE90);
DEFMEM(GM_GFX_LUIGI_W_PTR,        short*, 0X00B2CEAC);
DEFMEM(GM_GFX_PEACH_PTR,            HDC*, 0X00B2CEE4);
DEFMEM(GM_GFX_PEACH_MASK_PTR,       HDC*, 0X00B2CF00);
DEFMEM(GM_GFX_PEACH_H_PTR,        short*, 0X00B2CF6C);
DEFMEM(GM_GFX_PEACH_W_PTR,        short*, 0X00B2CF88);
DEFMEM(GM_GFX_TOAD_PTR,             HDC*, 0X00B2CFC0);
DEFMEM(GM_GFX_TOAD_MASK_PTR,        HDC*, 0X00B2CFDC);
DEFMEM(GM_GFX_TOAD_H_PTR,         short*, 0X00B2D048);
DEFMEM(GM_GFX_TOAD_W_PTR,         short*, 0X00B2D064);
DEFMEM(GM_GFX_LINK_PTR,             HDC*, 0X00B2D09C);
DEFMEM(GM_GFX_LINK_MASK_PTR,        HDC*, 0X00B2D0B8);
DEFMEM(GM_GFX_LINK_H_PTR,         short*, 0X00B2D124);
DEFMEM(GM_GFX_LINK_W_PTR,         short*, 0X00B2D140);
DEFMEM(GM_GFX_YOSHIB_PTR,           HDC*, 0X00B2D178);
DEFMEM(GM_GFX_YOSHIB_MASK_PTR,      HDC*, 0X00B2D194);
DEFMEM(GM_GFX_YOSHIT_PTR,           HDC*, 0X00B2D21C);
DEFMEM(GM_GFX_YOSHIT_MASK_PTR,      HDC*, 0X00B2D238);

//Graphics Memory [Overworld]
DEFMEM(GM_GFX_TILES_PTR,            HDC*, 0X00B2D2C0);
DEFMEM(GM_GFX_TILES_H_PTR,        short*, 0X00B2D304);
DEFMEM(GM_GFX_TILES_W_PTR,        short*, 0X00B2D320);
DEFMEM(GM_GFX_LEVEL_PTR,            HDC*, 0X00B2D358);
DEFMEM(GM_GFX_LEVEL_MASK_PTR,       HDC*, 0X00B2D374);
DEFMEM(GM_GFX_LEVEL_H_PTR,        short*, 0X00B2D3E0);
DEFMEM(GM_GFX_LEVEL_W_PTR,        short*, 0X00B2D3FC);
DEFMEM(GM_GFX_SCENE_PTR,            HDC*, 0X00B2D450);
DEFMEM(GM_GFX_SCENE_MASK_PTR,       HDC*, 0X00B2D46C);
DEFMEM(GM_GFX_SCENE_H_PTR,        short*, 0X00B2D4D8);
DEFMEM(GM_GFX_SCENE_W_PTR,        short*, 0X00B2D4F4);
DEFMEM(GM_GFX_PATH_PTR,             HDC*, 0X00B2D52C);
DEFMEM(GM_GFX_PATH_MASK_PTR,        HDC*, 0X00B2D548);
DEFMEM(GM_GFX_PATH_H_PTR,         short*, 0X00B2D5B4);
DEFMEM(GM_GFX_PATH_W_PTR,         short*, 0X00B2D5D0);
DEFMEM(GM_GFX_PLAYER_PTR,           HDC*, 0X00B2D608);
DEFMEM(GM_GFX_PLAYER_MASK_PTR,      HDC*, 0X00B2D624);
DEFMEM(GM_GFX_PLAYER_H_PTR,       short*, 0X00B2D690);
DEFMEM(GM_GFX_PLAYER_W_PTR,       short*, 0X00B2D6AC);

// Block Animation
DEFMEM(GM_BLOCK_ANIM_TIMER,       short*, 0x00B2BEBC);
DEFMEM(GM_BLOCK_ANIM_FRAME,       short*, 0x00B2BEA0);

DEFMEM(npc_gfxoffsetx,          short*, 0x00B25B70);
DEFMEM(npc_gfxoffsety,          short*, 0x00B25B8C);
DEFMEM(npc_width,               short*, 0x00B25BA8);
DEFMEM(npc_height,              short*, 0x00B25BC4);
DEFMEM(npc_gfxwidth,            short*, 0x00B25BE0);
DEFMEM(npc_gfxheight,           short*, 0x00B25BFC);
DEFMEM(npc_speed,               float*, 0x00B25C18);
DEFMEM(npc_isShell,             short*, 0x00B25C34);
DEFMEM(npc_npcblock,            short*, 0x00B25C50);
DEFMEM(npc_npcblocktop,         short*, 0x00B25C6C);
DEFMEM(isInteractableNPC_ptr,   short*, 0x00B25C88);
DEFMEM(isCoin_ptr,              short*, 0x00B25CA4);
DEFMEM(isVineNPC_ptr,           short*, 0x00B25CC0);
DEFMEM(isCollectableGoalNPC_ptr,short*, 0x00B25CDC);
DEFMEM(npc_isflying,            short*, 0x00B25CF8);
DEFMEM(npc_isWaterNPC,          short*, 0x00B25D14);
DEFMEM(npc_jumphurt,            short*, 0x00B25D30);
DEFMEM(npc_noblockcollision,    short*, 0x00B25D4C);
DEFMEM(npc_score,               short*, 0x00B25D68);
DEFMEM(npc_playerblocktop,      short*, 0x00B25D84);
DEFMEM(npc_grabtop,             short*, 0x00B25DA0);
DEFMEM(npc_cliffturn,           short*, 0x00B25DBC);
DEFMEM(npc_nohurt,              short*, 0x00B25DD8);
DEFMEM(npc_playerblock,         short*, 0x00B25DF4);
DEFMEM(npc_grabside,            short*, 0x00B25E2C);
DEFMEM(isShoeNPC_ptr,           short*, 0x00B25E48);
DEFMEM(isYoshiNPC_ptr,          short*, 0x00B25E64);
DEFMEM(npc_noyoshi,             short*, 0x00B25E9C);
DEFMEM(npc_foreground,          short*, 0x00B25EB8);
DEFMEM(npc_isBot,               short*, 0x00B25ED4);
DEFMEM(isVegetableNPC_ptr,      short*, 0x00B25F0C);
DEFMEM(npc_nofireball,          short*, 0x00B25F28);
DEFMEM(npc_noiceball,           short*, 0x00B25F44);
DEFMEM(npc_nogravity,           short*, 0x00B25F60);
DEFMEM(npc_frames,              short*, 0x00B25F7C);
DEFMEM(npc_framespeed,          short*, 0x00B25F98);
DEFMEM(npc_framestyle,          short*, 0x00B25FB4);

DEFMEM_PTR(npcdef_gfxoffsetx,       short, 0x00B25FC0);
DEFMEM_PTR(npcdef_gfxoffsety,       short, 0x00B2621A);
DEFMEM_PTR(npcdef_width,            short, 0x00B26474);
DEFMEM_PTR(npcdef_height,           short, 0x00B266CE);
DEFMEM_PTR(npcdef_gfxwidth,         short, 0x00B26928);
DEFMEM_PTR(npcdef_gfxheight,        short, 0x00B26B82);
DEFMEM_PTR(npcdef_isShell,          short, 0x00B26DDC);
DEFMEM_PTR(npcdef_npcblock,         short, 0x00B27036);
DEFMEM_PTR(npcdef_npcblocktop,      short, 0x00B27290);
DEFMEM_PTR(npcdef_isInteractableNPC,short, 0x00B274EA);
DEFMEM_PTR(npcdef_isCoin,           short, 0x00B27744);
DEFMEM_PTR(npcdef_isVine,           short, 0x00B2799E);
DEFMEM_PTR(npcdef_isCollectableGoal,short, 0x00B27BF8);
DEFMEM_PTR(npcdef_isFlyingNPC,      short, 0x00B27E52);
DEFMEM_PTR(npcdef_isWaterNPC,       short, 0x00B280AC);
DEFMEM_PTR(npcdef_jumphurt,         short, 0x00B28306);
DEFMEM_PTR(npcdef_noblockcollision, short, 0x00B28560);
DEFMEM_PTR(npcdef_score,            short, 0x00B287BA);
DEFMEM_PTR(npcdef_playerblocktop,   short, 0x00B28A14);
DEFMEM_PTR(npcdef_grabtop,          short, 0x00B28C6E);
DEFMEM_PTR(npcdef_cliffturn,        short, 0x00B28EC8);
DEFMEM_PTR(npcdef_nohurt,           short, 0x00B29122);
DEFMEM_PTR(npcdef_playerblock,      short, 0x00B2937C);
DEFMEM_PTR(npcdef_grabside,         short, 0x00B29830);
DEFMEM_PTR(npcdef_isShoeNPC,        short, 0x00B29A8A);
DEFMEM_PTR(npcdef_isYoshiNPC,       short, 0x00B29CE4);
DEFMEM_PTR(npcdef_noYoshi,          short, 0x00B2A198);
DEFMEM_PTR(npcdef_foreground,       short, 0x00B2A3F2);
DEFMEM_PTR(npcdef_isBot,            short, 0x00B2A64C);
DEFMEM_PTR(npcdef_isVegetableNPC,   short, 0x00B2AB00);
DEFMEM_PTR(npcdef_speed,            float, 0x00B2AD5C);
DEFMEM_PTR(npcdef_nofireball,       short, 0x00B2B210);
DEFMEM_PTR(npcdef_noiceball,        short, 0x00B2B46A);
DEFMEM_PTR(npcdef_nogravity,        short, 0x00B2B6C4);

DEFMEM(blockdef_isResizeableBlock, short*, 0x00B2B930);
DEFMEM(blockdef_width,             short*, 0x00B2B9F8);
DEFMEM(blockdef_height,            short*, 0x00B2BA14);
DEFMEM(blockdef_floorslope,        short*, 0x00B2B94C);
DEFMEM(blockdef_ceilingslope,      short*, 0x00B2B968);
DEFMEM(blockdef_semisolid,         short*, 0x00B2C048);
DEFMEM(blockdef_passthrough,       short*, 0x00b2c0d4);

DEFMEM(bgodef_width, WORD*, 0x00B2CCF4);
DEFMEM(bgodef_height, WORD*, 0x00B2BE4C);

DEFMEM(effectdef_width, short*, 0x00B2BA68);
DEFMEM(effectdef_height, short*, 0x00B2BA84);

DEFMEM(tiledef_height, short*, 0X00B2BF84);
DEFMEM(tiledef_width, short*, 0X00B2BF68);

// Frame timing related references
DEFMEM(GM_LAST_FRAME_TIME, double, 0x00B2D72C);
DEFMEM(GM_CURRENT_TIME,    double, 0x00B2D738);
DEFMEM(GM_MAX_FPS_MODE,    short,  0x00B2C8BE);


// VB6 GUI
DEFMEM(GM_FORM_GFX,        IDispatch*,      0x00B2D7C4);

// VB6 Window Names
DEFMEM(GM_GAMETITLE_1, VB6StrPtr, 0x8BD869);
DEFMEM(GM_GAMETITLE_2, VB6StrPtr, 0x8BE25A);
DEFMEM(GM_GAMETITLE_3, VB6StrPtr, 0x96AF26);


/////////////////////
///  -Assembly-   ///
/////////////////////

//VASM = value is assembly code
//npcToCoins ending Animation Settings
DEFMEM(VASM_END_ANIM,      BYTE, 0x00A3C86E);      // = 11
DEFMEM(VASM_END_COINSOUND, BYTE, 0x00A3C87F);      // = 14
DEFMEM(VASM_END_COINVAL,   BYTE, 0x00A3C891);      // = 1

// see LEVEL_HUD_CONTROL

////////////////////////
///    -Imports-     ///
////////////////////////

DEFMEM(IMP_vbaStrCmp,       void*, 0x004010F8); // Ptr to __stdcall
DEFMEM(IMP_vbaStrCopy,      void*, 0x004011b0); // Ptr to __fastcall
DEFMEM(IMP_vbaFreeStr,      void*, 0x00401248); // Ptr to __fastcall
DEFMEM(IMP_rtcRandomize,    void*, 0x00401090); // Ptr to __stdcall
DEFMEM(IMP_vbaFileOpen,     void*, 0x00401194); // Ptr to __stdcall
DEFMEM(IMP_vbaNew2,         void*, 0x004011A0); // Ptr to __stdcall
DEFMEM(IMP_vbaHresultCheckObj, void*, 0x00401070); // Ptr to __stdcall
DEFMEM(IMP_vbaInputFile, void*, 0x00401158); // Ptr to __cdecl

////////////////////////
///    -Functions-   ///
////////////////////////

// Print using game's text function

//      Arg1 = WCSTR* string
//      Arg2 = Int* fonttype
//      Arg3 = float* x
//      Arg4 = float* y
#define GF_PRINT            0x00951F50

#define GF_INIT_STATIC_VALS 0x008BE410

#define GF_INIT_DEF_VALS    0x008C2720

#define GF_SAVE_GAME        0x008E47D0
#define GF_LOAD_GAME        0x008E4E00

#define GF_LOAD_WORLD       0x008DF5B0

//      No args
#define GF_INIT_LEVEL_ENVIR 0x009944F0

// Start kill event for a player
//      Arg1 = int* to Index of player
#define GF_KILL_PLAYER      0x009B66D0

//      Arg1 = int* Index of player
#define GF_HARM_PLAYER      0x009B51E0

//      Arg1 = int* Sprite ID
//      Arg2 = int* Unk flags
//      Arg3 = int* Unk
#define GF_INIT_NPC         0x00A03630

//      Arg1 = int* Index of NPC
#define GF_NPC_FRAME        0x00A3C990

//      Arg1 = int* Index of NPC in NPC list
#define GF_UPDATE_NPC       0x00A3B680

//      Arg1 = int* Index of section containing music settings to play now
#define GF_PLAY_MUSIC       0x00A61B40

//      no args
#define GF_STOP_MUSIC       0x00A621A0


//      Arg1 = int* SoundIndex
#define GF_PLAY_SFX         0x00A73FD0

//      no args
#define GM_SETUP_SFX        0x00A74420

//      Arg1 = POS* structure of player, POS* structure of block
//      1=Collision from top, 2=From right, 3=From bottom, 4=From left, 5=?
#define GF_MOB_BLOCK_COL    0x00994250

//      Arg1 = int* spriteIndex
#define GF_SPRITESHEET_X    0x00987CE0

//      Arg1 = int* spriteIndex
#define GF_SPRITESHEET_Y    0x00987D90

//      Arg1 = int* (1)
//      Arg2 = Momentum* structure
#define GF_IS_ON_CAMERA     0x00993DE0

//      Arg1 = int* (1)
//      Arg2 = Momentum* structure
#define GF_IS_ON_WCAMERA    0x00993F90


//      Arg1 = wchar_t** name of event to start
//      Arg2 = short* [VB6 Bool] Force no smoke (ignore Event setting)
#define GF_TRIGGER_EVENT    0x00AA42D0

//      Arg1
#define GF_BITBLT_CALL      0x004242D0

//      Arg1 = int* Killed NPC index
//      Arg2 = NPCMOB* Killer NPC
#define GF_NPC_CLEANUP      0x00A315A0

//      Arg1 = short* The NPC Index which gets hit by an object
//      Arg2 = CollidersType* object type
//      Arg3 = short* The Object Index
#define GF_NPC_COLLIDES     0x00A281B0

//      Arg1 = short* The index of the player collecting it
//      Arg2 = short* The index of the NPC collected
#define GF_NPC_COLLECT      0x00A24CD0

//      Arg1 = Momentum* The location for the bomb explosion
//      Arg2 = short* Bomb explosion type
//      Arg3 = short* Player index
#define GF_DO_BOMB          0x00A3BA90

//      No Args, Does convert every NPC to coins (Like player would win)
#define GF_NPC_TO_COINS     0x00A3C580


//      Arg1 = unsigned int*  block index
//      Arg2 = short*         playSoundAndEffects
#define GF_BLOCK_REMOVE     0x009E0D50

//      Arg1 = unsigned int*  block index
//      Arg2 = short*         unknown flag 1
//      Arg3 = short*         unknown flag 2
#define GF_BLOCK_HIT        0x009DA620

//      Arg1 = short*         if to activate or deactivate the pswitch.
#define GF_DO_PSWITCH       0x009E33B0

//      Arg1 = short* Animation ID
//      Arg2 = Momentum* (for x and y coor)
//      Arg3 = float* Animation frame (NOTE: Not the direct frame of the gif-file)
//      Arg4 = short* npcID (for yoshi egg)
//      Arg5 = short* [VB 6 Bool] True, if only drawing the mask => Shadow effect
#define GF_RUN_ANIM         0x009E7380

//      Arg1 = short* Base score
//      Arg2 = Momentum* (for x and y coor)
//      Arg3 = short* Score factor
//      Actually Arg1 and Arg3 will be multiplied
#define GF_SCORE_RELEATED   0x008F80C0

//      This function displays the native SMBX messagebox AND menu.
//      To display you text you need to set GM_STR_MSGBOX before the call and reset it after the call.
//      Arg1 = short* Some sort of type (mostly the value 1)
#define GF_MSGBOX           0x008E54C0

//      Arg1 = short* startIndex
//      Arg2 = short* endIndex
#define GF_BLOCKSORT_Y      0x00A985A0

//      Arg1 = short* startIndex
//      Arg2 = short* endIndex
#define GF_BLOCKSORT_X      0x00A988E0

//      No Args
#define GF_BLOCKSORT_FIN1   0x00A98FE0

//      No Args
#define GF_BLOCKSORT_FIN2   0x00A99870

//      No Args
#define GF_BGOSORT_RELATED  0x00A9A000

//      Arg1 = VB6StrPtr* layerName
//      Arg2 = short* [VB 6 Bool] NoSmoke: False, to display the smoke effect
#define GF_SHOW_LAYER       0x00AA2760

//      Arg1 = VB6StrPtr* layerName
//      Arg2 = short* [VB 6 Bool] NoSmoke: False, to display the smoke effect
#define GF_HIDE_LAYER       0x00AA3730

#define GF_POW              0x009E4600

#define GF_LOAD_LOCAL_GFX   0x00ACD220

#define GF_LOAD_GRAPHICS_FROM_FOLDER 0x00AD9DC0

#define GF_LOAD_WORLD_GFX   0x00ADFF90

//      Arg1 = VB6StrPtr* Added text to the cheat buffer and execute
#define GF_UPDATESCAN_CHEAT 0x008E7490

#define GF_LOAD_WORLD_LIST  0x008E35E0

#define GF_LOAD_SAVE_STATES 0x008E41D0

#define GF_LOAD_NPC_CONFIG  0x00B20E50

//      Arg1 = Pointer to structure
#define GF_THUN_RT_MAIN     0x0040BDD2

//      Arg1 = Camera Index
//      Arg2 = Always 0x4002 in, but maybe used as an output?
#define GF_RENDER_LEVEL_HUD 0x0096BF20

#define GF_UPDATE_INPUT     0x00A74910
#define GF_RENDER_WORLD     0x008FEB10
#define GF_RENDER_LEVEL     0x00909290
#define GF_UPDATE_BLOCK_ANIM 0x009E14B0

#define GF_CLEANUP_LEVEL    0x008DC6E0
#define GF_LOAD_LEVEL       0x008D8F40
#define GF_INIT_CAMERA      0x009502E0
#define GF_RENDER_INIT_SCREEN 0x00987DE0
#define GF_AUDIO_MANAGEMENT 0x00A74460
//0x008E6F70

#define GF_EXIT_MAIN_GAME   0x008D6BB0

#define GF_APPLY_FULLSCREEN 0x00A98190

#define GF_UNK_DOORS_COUNT  0x008F7D70

#define GF_LOAD_DEFAULT_CONTROLS 0x008E6700

#define GF_DRAW_BACKGROUND 0x00954F50

DEFMEM(GF_RTC_DO_EVENTS, void*, 0x004010B8);

static const auto native_initStaticVals = (void(__stdcall *)())GF_INIT_STATIC_VALS;
static const auto native_initDefVals    = (void(__stdcall *)())GF_INIT_DEF_VALS;

static const auto native_print          = (void(__stdcall *)(VB6StrPtr* /*Text*/, short* /*fonttype*/, float* /*x*/, float* /*y*/))GF_PRINT;

static const auto native_saveGame       = (void(__stdcall *)())GF_SAVE_GAME;
static const auto native_loadGame       = (void(__stdcall *)())GF_LOAD_GAME;

static const auto native_loadWorld      = (void(__stdcall *)(VB6StrPtr* /*path*/))GF_LOAD_WORLD;

static const auto native_spritesheetX   = (short(__stdcall *)(int* /*spriteIndex*/))GF_SPRITESHEET_X;
static const auto native_spritesheetY   = (short(__stdcall *)(int* /*spriteIndex*/))GF_SPRITESHEET_Y;

static const auto native_isOnCamera     = (short(__stdcall *)(unsigned int* /*camIndex*/, Momentum* /*momentumObj*/))GF_IS_ON_CAMERA;
static const auto native_isOnWCamera    = (short(__stdcall *)(unsigned int* /*camIndex*/, Momentum* /*momentumObj*/))GF_IS_ON_WCAMERA;

static const auto native_initLevelEnv   = (void(__stdcall *)())GF_INIT_LEVEL_ENVIR;

static const auto native_killPlayer     = (void(__stdcall *)(short* /*playerIndex*/))GF_KILL_PLAYER;
static const auto native_harmPlayer     = (void(__stdcall *)(short* /*playerIndex*/))GF_HARM_PLAYER;
static const auto native_updateNPC      = (void(__stdcall *)(short* /*npcID*/))GF_UPDATE_NPC;

static const auto native_playMusic      = (void(__stdcall *)(short* /*section*/))GF_PLAY_MUSIC;
static const auto native_stopMusic      = (void(__stdcall *)())GF_STOP_MUSIC;
static const auto native_playSFX        = (void(__stdcall *)(short* /*soundIndex*/))GF_PLAY_SFX;
static const auto native_setupSFX       = (void(__stdcall *)())GM_SETUP_SFX;

static const auto native_cleanupKillNPC = (void(__stdcall *)(short* /**/, short* /**/))GF_NPC_CLEANUP;

static const auto native_collideNPC     = (void(__stdcall *)(short* /*npcIndexToCollide*/, CollidersType* /*typeOfObject*/, short* /*objectIndex*/))GF_NPC_COLLIDES;

static const auto native_collectNPC     = (void(__stdcall *)(short* /*playerIdx*/, short* /*npcIdx*/))GF_NPC_COLLECT;

static const auto native_doBomb         = (void(__stdcall *)(Momentum* /*position*/, short* /*bombType*/, short* /*player index*/))GF_DO_BOMB;
static const auto native_npcToCoins     = (void(__stdcall *)())GF_NPC_TO_COINS;
static const auto native_doPow          = (void(__stdcall *)())GF_POW;

static const auto native_hitBlock       = (void(__stdcall *)(unsigned int* /*blockIndex*/, short* /*unknownFlag1*/, unsigned short* /*unknownFlag2*/))GF_BLOCK_HIT;
static const auto native_removeBlock    = (void(__stdcall *)(unsigned int* /*blockIndex*/, short* /*playEffects*/))GF_BLOCK_REMOVE;
static const auto native_doPSwitch      = (void(__stdcall *)(short* /*doPSwith*/))GF_DO_PSWITCH;

static const auto native_runEffect      = (void(__stdcall *)(short* /*EffectID*/, Momentum* /*coor*/, float* /*EffectFrame*/, short* /*npcID*/, short* /*showOnlyMask*/))GF_RUN_ANIM;
static const auto native_addScoreEffect = (void(__stdcall *)(short* /*baseValue*/, Momentum* /*coor*/, short* /*factor*/))GF_SCORE_RELEATED;

static const auto native_msgbox         = (void(__stdcall *)(short* /*unkVal*/))GF_MSGBOX;

static const auto native_sortY          = (void(__stdcall *)(short* /*startIndex*/, short* /*endIndex*/))GF_BLOCKSORT_Y;
static const auto native_sortX          = (void(__stdcall *)(short* /*startIndex*/, short* /*endIndex*/))GF_BLOCKSORT_X;
static const auto native_sort_finalize1 = (void(__stdcall *)())GF_BLOCKSORT_FIN1;
static const auto native_sort_finalize2 = (void(__stdcall *)())GF_BLOCKSORT_FIN2;
static const auto native_sort_bgo       = (void(__stdcall *)())GF_BGOSORT_RELATED;

static const auto native_unkDoorsCount  = (void(__stdcall *)())GF_UNK_DOORS_COUNT;

static const auto native_triggerEvent   = (void(__stdcall *)(VB6StrPtr* /*eventName*/, short* /*forceNoSmoke*/))GF_TRIGGER_EVENT;

static const auto native_showLayer      = (void(__stdcall *)(VB6StrPtr* /*layerName*/, short* /*noSmoke*/))GF_SHOW_LAYER;
static const auto native_hideLayer      = (void(__stdcall *)(VB6StrPtr* /*layerName*/, short* /*noSmoke*/))GF_HIDE_LAYER;

static const auto native_loadLocalGfx   = (void(__stdcall *)())GF_LOAD_LOCAL_GFX;
static const auto native_loadWorldGfx   = (void(__stdcall *)())GF_LOAD_WORLD_GFX;

static const auto native_updateCheatbuf = (void(__stdcall *)(VB6StrPtr* /*addedText*/))GF_UPDATESCAN_CHEAT;

static const auto native_loadWorldList  = (void(__stdcall *)())GF_LOAD_WORLD_LIST;
static const auto native_loadSaveStates = (void(__stdcall *)())GF_LOAD_SAVE_STATES;

static const auto native_ThunRTMain     = (void(__stdcall *)(void *))GF_THUN_RT_MAIN;

static const auto native_rtcDoEvents    = (void(__stdcall *)())GF_RTC_DO_EVENTS;

static const auto native_renderLevelHud = (void(__stdcall *)(int* /*camIndex*/, int* /* unknown0x4002 */))GF_RENDER_LEVEL_HUD;
static const auto native_updateInput    = (void(__stdcall *)(void))GF_UPDATE_INPUT;
static const auto native_renderWorld    = (void(__stdcall *)(void))GF_RENDER_WORLD;
static const auto native_renderLevel    = (void(__stdcall *)(void))GF_RENDER_LEVEL;
static const auto native_updateBlockAnim = (void(__stdcall *)(void))GF_UPDATE_BLOCK_ANIM;

static const auto native_cleanupLevel   = (void(__stdcall *)(void))GF_CLEANUP_LEVEL;
static const auto native_loadLevel      = (void(__stdcall *)(VB6StrPtr* /*path*/))GF_LOAD_LEVEL;
static const auto native_initCamera     = (void(__stdcall *)(void))GF_INIT_CAMERA;
static const auto native_renderInitScreen = (void(__stdcall *)(void))GF_RENDER_INIT_SCREEN;
static const auto native_audioManagement = (void(__stdcall *)(void))GF_AUDIO_MANAGEMENT;

static const auto native_loadNPCConfig  = (void(__stdcall *)(VB6StrPtr* /*customPath*/))GF_LOAD_NPC_CONFIG;

static const auto native_loadGraphicsFromFolder = (void(__stdcall *)(VB6StrPtr* /*customFolder*/))GF_LOAD_GRAPHICS_FROM_FOLDER;

static const auto native_exitMainGame = (void(__stdcall *)(void))GF_EXIT_MAIN_GAME;

static const auto native_applyFullscreen = (void(__stdcall *)(void* arg1, void* arg2, void* arg3, void* arg4))GF_APPLY_FULLSCREEN;

static const auto native_loadDefaultControls = (void(__stdcall *)(void))GF_LOAD_DEFAULT_CONTROLS;

static const auto native_initNPC = (int(__stdcall *)(short* npcId, float* dir, void* arg3))GF_INIT_NPC;

static const auto native_drawBackground = (void(__stdcall *)(short* section, short* camera))GF_DRAW_BACKGROUND;

static const auto native_setNPCFrame = (void(__stdcall*)(short* npcidx))GF_NPC_FRAME;
/*
Function name
                                           Segment Start    Length   Locals   Arguments
sub_40B9B0                                   .text 0040B9B0 0000001D 00000000 FFFFFFFC R F . . . . .
j___vbaChkstk                                .text 0040BA60 00000006                   R . . . . . .
j___vbaExceptHandler                         .text 0040BA66 00000006                   R . . . . . .
j__adj_fdiv_m32                              .text 0040BA78 00000006 00000000 00000000 R . . . . T .
j__adj_fdiv_m64                              .text 0040BA84 00000006 00000000 00000000 R . . . . . .
j__adj_fdiv_r                                .text 0040BA8A 00000006                   R . . . . . .
j__adj_fdivr_m64                             .text 0040BAA2 00000006 00000000 00000000 R . . . . . .
j_DllFunctionCall                            .text 0040BAF0 00000006 00000000 00000000 R . . . . . .
j_ThunRTMain                                 .text 0040BDD2 00000006                   R . . . . . .
DeclareKernel32_Sleep                        .text 004240B4 00000019                   R . . . . . .
BitbltCall                                   .text 004242D0 00000019                   R . . . . T .
DeclareGdi32_StretchBlt                      .text 00424314 00000019                   R . . . . T .
DeclareGdi32_CreateCompatibleBitmap          .text 00424364 00000019                   R . . . . T .
DeclareGdi32_CreateCompatibleDC              .text 004243B0 00000019                   R . . . . T .
DeclareGdi32_SelectObject                    .text 00424444 00000019                   R . . . . T .
DeclareGdi32_DeleteObject                    .text 00424498 00000019                   R . . . . T .
DeclareGdi32_DeleteDC                        .text 004244DC 00000019                   R . . . . T .
DeclareUser32_GetKeyState                    .text 00424520 00000019                   R . . . . T .
DeclareWinmm_mciSendStringA                  .text 00424578 00000019                   R . . . . T .
DeclareUser32_GetActiveWindow                .text 00424748 00000019                   R . . . . . .
DeclareKernel32_GetTickCount                 .text 00424790 00000019                   R . . . . . .
DeclareUser32_ShowCursor                     .text 004276BC 00000019                   R . . . . . .
nullsub_1                                    .text 00427FCC 00000001                   R F . . . . .
DeclareSocket32_setsockopt                   .text 0042854C 00000019                   R . . . . . .
DeclareUser32_GetCursorPos                   .text 00428A18 00000019                   R . . . . . .
DeclareUser32_ChangeDisplaySettingsExA       .text 00428E18 00000019                   R . . . . T .
DeclareUser32_EnumDisplaySettingsA           .text 00428E68 00000019                   R . . . . . .
_O_Pub_Obj_Inf1_Event0x3_frmMain_DblClick    .text 008BD770 0000018C 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf1_Event0x4_frmMain_KeyDown     .text 008BD900 000000D8 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1_Event0x5_frmMain_KeyPress    .text 008BD9E0 000000C5 0000002C 00000008 R . . . B . .
_O_Pub_Obj_Inf1_Event0x6_frmMain_KeyUp       .text 008BDAB0 0000008B 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1_Event0x7_frmMain_Load        .text 008BDB40 00000334 000000B8 00000004 R . . . B . .
_O_Pub_Obj_Inf1_Event0x8_frmMain_MouseDown   .text 008BDE80 00000088 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1_Event0x9_frmMain_MouseMove   .text 008BDF10 00000166 00000024 00000014 R . . . B . .
_O_Pub_Obj_Inf1_Event0xA_frmMain_MouseUp     .text 008BE080 00000072 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf1_Event0xB_frmMain_QueryUnload .text 008BE100 00000098 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf1_Event0xC_frmMain_Resize      .text 008BE1A0 000001B8 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf1_Event0xD_frmMain_Unload      .text 008BE360 0000007F 00000024 00000008 . . . . B . .
unkInitVals                                  .text 008BE410 000005AA                   R . . . . T .
Sub_Main                                     .text 008BE9C0 00003D5F 00000394 00000000 . . . . B . .
loadDefaultsVars                             .text 008C2720 00007AEF 00000014 00000000 R . . . . T .
levelLoop                                    .text 008CA210 0000043F 00000058 00000000 R . . . B T .
introLoop                                    .text 008CA650 0000C52D 00000A50 00000000 R . . . B T .
editorLoop                                   .text 008D6B80 0000002A                   R . . . . . .
exitMainGame                                 .text 008D6BB0 0000010B 00000020 00000000 . . . . B . .
saveLevel                                    .text 008D6CF0 0000224E 00000310 00000004 R . . . B T .
loadLevel                                    .text 008D8F40 00003792 0000023C 00000004 R . . . B T .
cleanupLevel                                 .text 008DC6E0 0000174A 00000940 00000000 R . . . B T .
goToWorldmap                                 .text 008DDE30 00000242 00000090 00000000 R . . . . . .
doWinAnimations                              .text 008DE080 00000AD5 000000A0 00000000 R . . . . T .
saveWorld                                    .text 008DEB60 00000A4F 00000174 00000004 R . . . B . .
loadWorld                                    .text 008DF5B0 000010F5 00000188 00000004 R . . . B T .
worldLoop                                    .text 008E06B0 0000173F 00000158 00000000 R . . . B . .
sub_8E1DF0                                   .text 008E1DF0 00000554 00000064 0000000C R . . . . . .
sub_8E2350                                   .text 008E2350 000006E9 00000054 00000008 R . . . B . .
unkUpdateFPSCounter                          .text 008E2A40 000003F6 00000044 00000000 R . . . B . .
sub_8E2E40                                   .text 008E2E40 00000796 00000194 00000000 R . . . B T .
loadWorlds                                   .text 008E35E0 00000730 000000D4 00000000 R . . . B T .
loadBattleLevels                             .text 008E3D10 000004B9 000000CC 00000000 R . . . B T .
unkCountPercentageOfGameSave                 .text 008E41D0 000005FB 000000AC 00000000 R . . . B T .
saveGame                                     .text 008E47D0 0000062E 0000007C 00000000 R . . . B . .
loadGame                                     .text 008E4E00 000006B2 00000060 00000000 R . . . B T .
doMenuOrMessageBox                           .text 008E54C0 0000123F 000000E4 00000004 R . . . B T .
loadDefaultControls                          .text 008E6700 00000208 00000008 00000000 R . . . . . .
loadGameConfiguration                        .text 008E6910 000003AF 0000003C 00000000 R . . . B T .
saveGameConfiguration                        .text 008E6CC0 000002A4 00000024 00000000 R . . . B . .
unkMusicManagement_8E6F70                    .text 008E6F70 0000051C 00000054 00000000 R . . . B . .
updateScanCheat                              .text 008E7490 0000F88A 00000694 00000004 R . . . B T .
sub_8F6D20                                   .text 008F6D20 000004D2 0000007E 00000000 R . . . . . .
constructCreditsText                         .text 008F7200 00000B69 00000048 00000000 R . . . B . .
unkDoorsCount                                .text 008F7D70 00000288 00000094 00000000 R . . . B . .
addToCredits                                 .text 008F8000 0000004B 00000004 00000004 R . . . . T .
createMomentumStruct                         .text 008F8050 0000006F 00000038 00000014 R . . . . T .
funcScoreRelated                             .text 008F80C0 00000199 00000018 0000000C R . . . . T .
initPlayerOffsetVars                         .text 008F8260 0000603F 0000001C 00000000 R . . . . T .
initIsResizeableBlocksVar                    .text 008FE2A0 00000195                   R . . . . T .
resetGame                                    .text 008FE440 000004E2 000001F4 00000000 R . . . B . .
sub_8FE930                                   .text 008FE930 000001DC 00000090 00000004 R . . . B T .
renderWorld                                  .text 008FEB10 0000A772 000004A4 00000000 R . . . B . .
renderLevel                                  .text 00909290 00044C67 00003F50 FFFFFFF8 R . . . B T .
sub_94DF20                                   .text 0094DF20 00000FB7 00000010 00000004 R . . . . . .
sub_94EEE0                                   .text 0094EEE0 00000552 0000002C 00000000 R . . . . . .
sub_94F440                                   .text 0094F440 00000234 00000014 00000000 R . . . . . .
initGameHDC                                  .text 0094F680 00000248 00000020 00000000 R . . . B . .
setLevelWindowDefaults                       .text 0094F8D0 00000A0F 00000028 00000000 R . . . B . .
sub_9502E0                                   .text 009502E0 00000485 00000004 00000000 R . . . . . .
sub_950770                                   .text 00950770 000017DE 00000024 00000000 R . . . . . .
PrintText                                    .text 00951F50 000022EE 0000019C 00000010 R . . . B T .
sub_954240                                   .text 00954240 0000007A 0000001C 00000000 R . . . . . .
sub_9542C0                                   .text 009542C0 000007F1 00000050 00000004 R . . . B . .
sub_954AC0                                   .text 00954AC0 0000048E 0000000C 00000000 R . . . . . .
unkBackgroundDraw                            .text 00954F50 00014CA8 00000CE0 00000008 R . . . B T .
sub_969C00                                   .text 00969C00 000006ED 00000010 00000010 R . . . . . .
sub_96A2F0                                   .text 0096A2F0 00000A8E 00000010 00000010 R . . . . . .
initGameWindow                               .text 0096AD80 000008C0 00000174 00000000 R . . . . . .
sub_96B640                                   .text 0096B640 000004AE 0000001C 00000000 R . . . . . .
sub_96BAF0                                   .text 0096BAF0 00000428 00000064 00000000 R . . . B . .
unkRenderHud_96BF20                          .text 0096BF20 0001BDB7 000003A4 00000008 R . . . B . .
getXPosOfPlayerSpritesheet                   .text 00987CE0 000000A9 00000000 00000004 R . . . . T .
getYPosOfPlayerSpritesheet                   .text 00987D90 00000046 00000000 00000004 R . . . . . .
renderInitScreen                             .text 00987DE0 0000102F 00000090 00000000 R . . . B T .
renderPlayer                                 .text 00988E10 00008AA2 000009B8 00000008 R . . . B T .
renderScreenshot                             .text 009918C0 0000080F 00000088 00000000 R . . . B T .
renderNPC                                    .text 009920D0 00000E78 000001A0 00000008 R . . . . T .
unkCollisionCheck                            .text 00992F50 00000090 0000000C 00000008 R . . . . T .
sub_992FE0                                   .text 00992FE0 000001EF 00000014 00000008 R . . . . . .
sub_9931D0                                   .text 009931D0 00000156 0000001C 00000008 R . . . . . .
sub_993330                                   .text 00993330 00000139 00000010 00000008 R . . . . . .
sub_993470                                   .text 00993470 00000146 00000014 0000000C R . . . . . .
sub_9935C0                                   .text 009935C0 0000013F 00000010 00000008 R . . . . . .
sub_993700                                   .text 00993700 00000270 00000010 0000000C R . . . . . .
sub_993970                                   .text 00993970 00000270 00000010 0000000C R . . . . . .
sub_993BE0                                   .text 00993BE0 000000E9 00000014 00000008 R . . . . . .
sub_993CD0                                   .text 00993CD0 0000010E 00000020 0000000C R . . . . . .
IsOnCamera                                   .text 00993DE0 000001A3 00000018 00000008 R . . . . T .
IsOnOverworldCamera                          .text 00993F90 000001A0 00000018 00000008 R . . . . . .
sub_994130                                   .text 00994130 0000007B 00000014 00000008 R . . . . T .
sub_9941B0                                   .text 009941B0 00000094 00000018 0000000C R . . . . . .
MobBlockColl                                 .text 00994250 00000135 00000010 00000008 R . . . . T .
sub_994390                                   .text 00994390 000000E9 00000014 00000008 R . . . . . .
sub_994480                                   .text 00994480 00000063 0000000C 00000034 R . . . . T .
unkInitLevelEnvironment                      .text 009944F0 0000152E 00000160 00000000 R . . . B . .
runGameLoop                                  .text 00995A20 0001F7B9 000010D8 00000000 R . . . B T .
HarmPlayer                                   .text 009B51E0 000014E5 00000180 00000004 R . . . B T .
KillPlayer                                   .text 009B66D0 000007E7 000000D0 00000004 R . . . B . .
sub_9B6EC0                                   .text 009B6EC0 000006C9 00000064 00000004 R . . . B . .
sub_9B7590                                   .text 009B7590 000000E1 00000010 00000000 R . . . . . .
isNotAllPlayerDead                           .text 009B7680 00000083 00000010 00000000 R . . . . . .
isOnePlayerStillAlive                        .text 009B7710 00000064 0000000C 00000000 R . . . . . .
doPlayerDeadCode                             .text 009B7780 00000465 0000003C 00000000 R . . . B . .
unkDuckingUpdate                             .text 009B7BF0 0000021C 00000010 00000004 R . . . . T .
sub_9B7E10                                   .text 009B7E10 00000805 00000060 00000004 R . . . B . .
unkUpdatePlayerSpecialStates                 .text 009B8620 00002C8D 00000194 00000004 R . . . B T .
unkPickupAnimationRelated                    .text 009BB2B0 000001E0 00000008 00000008 R . . . . T .
sub_9BB490                                   .text 009BB490 000020C5 00000378 00000010 R . . . B . .
sub_9BD560                                   .text 009BD560 000000AB 0000000C 00000004 R . . . . . .
sub_9BD610                                   .text 009BD610 000010BF 00000104 00000004 R . . . B . .
sub_9BE6D0                                   .text 009BE6D0 00001961 0000011C 00000004 R . . . B T .
sub_9C0040                                   .text 009C0040 0000065F 000000B4 00000004 R . . . . . .
sub_9C06A0                                   .text 009C06A0 00000155 00000004 00000000 R . . . . . .
sub_9C0800                                   .text 009C0800 0000044E 00000054 00000008 R . . . . . .
unkPlayerUpdate9C0C50                        .text 009C0C50 0000113E 00000148 00000004 R . . . B T .
sub_9C1D90                                   .text 009C1D90 00002924 00000198 00000004 R . . . B . .
sub_9C46C0                                   .text 009C46C0 000003AA 00000024 00000000 R . . . . . .
sub_9C4A70                                   .text 009C4A70 00001043 00000248 00000000 R . . . B . .
sub_9C5AC0                                   .text 009C5AC0 00000966 00000134 00000004 R . . . B . .
sub_9C6430                                   .text 009C6430 000001C1 00000008 00000004 R . . . . . .
sub_9C6600                                   .text 009C6600 0000313D 000001B8 00000004 R . . . B . .
sub_9C9740                                   .text 009C9740 0000177D 00000154 00000004 R . . . B . .
unkPlayerCollisionCheck                      .text 009CAEC0 000013EA 00000168 00000004 R . . . B T .
sub_9CC2B0                                   .text 009CC2B0 000051C7 00000248 00000008 R . . . B . .
sub_9D1480                                   .text 009D1480 000009FB 000000AC 00000004 R . . . B . .
sub_9D1E80                                   .text 009D1E80 00008791 000008BC 00000004 R . . . B . .
unkDoBlockHit                                .text 009DA620 00006319 00000330 0000000C R . . . B T .
startBlockHitAnimationToUp                   .text 009E0940 000000B6 0000000C 00000004 R . . . . . .
setBlockHitEffect                            .text 009E0A00 000000B6 0000000C 00000004 R . . . . T .
startBlockHitAnimationToDown                 .text 009E0AC0 000000B6 0000000C 00000004 R . . . . . .
unkBlockRemove                               .text 009E0B80 000001CA 00000024 00000004 R . . . B T .
removeBlock                                  .text 009E0D50 00000760 000000B0 00000008 R . . . B T .
sub_9E14B0                                   .text 009E14B0 00000BDD 00000054 00000000 R . . . B . .
updateBlock                                  .text 009E2090 0000131F 00000108 00000000 R . . . . T .
togglePSwitch                                .text 009E33B0 0000124B 00000118 00000004 R . . . B T .
doPOW                                        .text 009E4600 000002F8 00000064 00000000 R . . . B T .
updateEffect                                 .text 009E4900 00002A72 00000248 00000000 R . . . B . .
runEffect                                    .text 009E7380 00004DC3 0000027C 00000014 R . . . B T .
cleanupEffect                                .text 009EC150 000000A2 00000008 00000004 R . . . . T .
updateLvlEditorGui                           .text 009EC200 00010B0A 00000520 00000000 R . . . B . .
sub_9FCD10                                   .text 009FCD10 00000085                   R . . . . . .
sub_9FCDA0                                   .text 009FCDA0 00003EB0 000001D8 00000000 R . . . B . .
sub_A00C50                                   .text 00A00C50 00001439 00000148 00000000 R . . . B . .
sub_A02090                                   .text 00A02090 0000008C                   R . . . . . .
removeWarp                                   .text 00A02120 000000FC 000000A4 00000004 R . . . B T .
runEditorLevel                               .text 00A02220 0000140C 000002A4 00000000 R . . . B T .
InitNPC_orAnimFrame                          .text 00A03630 00000B9C 00000074 0000000C R . . . B T .
sub_A041D0                                   .text 00A041D0 00001A66 00000688 0000000C R . . . B T .
sub_A05C40                                   .text 00A05C40 000000DC 00000170 00000004 R . . . B T .
sub_A05D20                                   .text 00A05D20 0000036C 0000006C 00000004 R . . . . . .
sub_A06090                                   .text 00A06090 0001E278 00000E00 00000000 R . . . B T .
doItemDrop                                   .text 00A24310 000009B8 0000006C 00000004 R . . . B T .
unkOnInteractA24CD0                          .text 00A24CD0 000034D9 00000264 00000008 R . . . B T .
unkOnCollision                               .text 00A281B0 000093EF 00000524 0000000C R . . . B T .
NpcKill                                      .text 00A315A0 0000A0D4 00000944 00000008 R . . . B T .
UpdateNPCMovement                            .text 00A3B680 000001F7 00000010 00000004 R . . . . T .
sub_A3B880                                   .text 00A3B880 00000204 00000014 00000004 R . . . B . .
doBombExplosion                              .text 00A3BA90 00000AE9 00000114 0000000C R . . . B T .
NpcToCoins                                   .text 00A3C580 0000040B 00000030 00000000 R . . . . T .
unkIsDestroyableNPC                          .text 00A3C990 00007D0C 00000400 00000004 R . . . B . .
triggerMovementOfSkullRaft                   .text 00A446A0 00000227 00000040 00000004 R . . . . T .
updateNPC                                    .text 00A448D0 0000E2D5 00000844 00000004 R . . . B T .
updateNPCRelated                             .text 00A52BB0 0000DF39 000008A8 00000004 R . . . B T .
unkOnUpdateMovementNPC                       .text 00A60AF0 00000F05 00000020 00000008 R . . . . T .
sub_A61A00                                   .text 00A61A00 00000138 0000004C 00000004 R . . . B T .
PlayMusic                                    .text 00A61B40 0000065C 00000100 00000004 R . . . B . .
stopMusic                                    .text 00A621A0 0000032A 00000094 00000000 R . . . B T .
initAssignSoundFiles                         .text 00A624D0 00011AFD 000000F0 00000000 R . . . B . .
playSound                                    .text 00A73FD0 00000445 00000024 00000004 R . . . B T .
setupSound                                   .text 00A74420 0000003A 0000000C 00000000 R . . . . . .
unkCheckMusicState                           .text 00A74460 000004AC 0000006C 00000000 R . . . B T .
updateInput                                  .text 00A74910 00000D62 000000EC 00000000 R . . . B . .
unkJoystick                                  .text 00A75680 00000195 000001B4 00000002 R . . . . T .
updateJoystick                               .text 00A75820 00000143 0000002C 00000000 R . . . . . .
_O_Pub_Obj_InfD_Event0x1                     .text 00A75970 000000CE 00000030 00000004 R . . . B . .
_O_Pub_Obj_InfD_Event0x2                     .text 00A75A40 0000006F 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfD_Event0x3                     .text 00A75AB0 00000126 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfD_Event0x4                     .text 00A75BE0 00000126 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfD_Event0x5                     .text 00A75D10 0000006F 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfD_Event0x6                     .text 00A75D80 00000078 00000024 00000008 R . . . B . .
_O_Pub_Obj_InfD_Event0x7                     .text 00A75E00 000002FC 00000030 00000004 R . . . B . .
_O_Pub_Obj_InfD_Event0x8                     .text 00A76100 000000A7 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfD_Event0x9                     .text 00A761B0 000008CF 000000C0 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x1                     .text 00A76A80 000000CE 00000030 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x2                     .text 00A76B50 0000006F 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x3                     .text 00A76BC0 00000126 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x4                     .text 00A76CF0 0000019A 0000005C 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x5                     .text 00A76E90 000001C9 00000040 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x6                     .text 00A77060 00000363 0000006C 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x7                     .text 00A773D0 000000A7 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfE_Event0x8                     .text 00A77480 00000078 00000024 00000008 R . . . B . .
_O_Pub_Obj_InfE_Event0x9                     .text 00A77500 00000A29 000000C8 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x1                     .text 00A77F30 00000114 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x2                     .text 00A78050 00000114 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x3                     .text 00A78170 0000463F 000004AC 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x4                     .text 00A7C7B0 00000044 00000024 00000004 . . . . B . .
_O_Pub_Obj_InfF_Event0x5                     .text 00A7C820 00000044 00000024 00000004 . . . . B . .
_O_Pub_Obj_InfF_Event0x6                     .text 00A7C890 000002A8 000000B8 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x7                     .text 00A7CB40 000002A8 000000B8 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x8                     .text 00A7CDF0 000000E3 00000034 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x9                     .text 00A7CEE0 000001EF 00000070 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0xA                     .text 00A7D0D0 00000044 00000024 00000004 . . . . B . .
_O_Pub_Obj_InfF_Event0xB                     .text 00A7D140 00000425 00000050 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0xC                     .text 00A7D570 00000107 00000034 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0xD                     .text 00A7D680 00000107 00000034 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0xE                     .text 00A7D790 000001EF 00000070 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0xF                     .text 00A7D980 00000252 00000078 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x10                    .text 00A7DBE0 0000008D 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x11                    .text 00A7DC70 0000006F 00000024 00000000 R . . . B . .
_O_Pub_Obj_InfF_Event0x12                    .text 00A7DCE0 00000119 00000020 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x13                    .text 00A7DE00 00000073 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x14                    .text 00A7DE80 00000073 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x15                    .text 00A7DF00 00000119 00000020 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x16                    .text 00A7E020 00000073 00000024 00000004 R . . . B T .
_O_Pub_Obj_InfF_Event0x17                    .text 00A7E0A0 00000073 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x18                    .text 00A7E120 00000073 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x19                    .text 00A7E1A0 00000073 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x1A                    .text 00A7E220 00000085 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x1B                    .text 00A7E2B0 0000007C 00000024 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x1C                    .text 00A7E330 000000E3 00000034 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x1D                    .text 00A7E420 00000493 000000C0 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x1E                    .text 00A7E8C0 000000E3 00000034 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x1F                    .text 00A7E9B0 00000560 000000CC 00000004 R . . . B . .
_O_Pub_Obj_InfF_Event0x20                    .text 00A7EF10 000025A8 00000220 00000008 R . . . B . .
_O_Pub_Obj_Inf10_Method0x1                   .text 00A814C0 00000045 00000024 00000004 . . . . B . .
_O_Pub_Obj_Inf10_Event0x2                    .text 00A81530 000008ED 00000260 00000004 R . . . B . .
_O_Pub_Obj_Inf10_Event0x3                    .text 00A81E20 0000006A 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf10_Event0x4                    .text 00A81E90 000001B2 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf10_Event0x5                    .text 00A82050 0000006D 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf10_Event0x6                    .text 00A820C0 00000077 00000024 0000000C R . . . B . .
_O_Pub_Obj_Inf10_Event0x7                    .text 00A82140 000000C5 0000002C 0000000C R . . . B . .
_O_Pub_Obj_Inf10_Event0x8                    .text 00A82210 0000006A 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf10_Event0x9                    .text 00A82280 00000256 00000024 00000018 R . . . B T .
_O_Pub_Obj_Inf10_Event0xA                    .text 00A824E0 000004F6 000000A0 00000018 R . . . B T .
_O_Pub_Obj_Inf10_Event0xB                    .text 00A829E0 00000077 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0x1                    .text 00A82A60 00000453 000000A4 00000004 R . . . B . .
_O_Pub_Obj_Inf11_Event0x2                    .text 00A82EC0 00000433 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf11_Event0x3                    .text 00A83300 00000433 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf11_Event0x4                    .text 00A83740 00000433 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf11_Event0x5                    .text 00A83B80 00000433 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf11_Event0x6                    .text 00A83FC0 000002B9 0000005C 00000004 R . . . B . .
_O_Pub_Obj_Inf11_Event0x7                    .text 00A84280 0000011D 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0x8                    .text 00A843A0 0000049C 00000094 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0x9                    .text 00A84840 000001BD 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0xA                    .text 00A84A00 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0xB                    .text 00A84DC0 000002FB 00000074 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0xC                    .text 00A850C0 000004CA 00000094 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0xD                    .text 00A85590 00001796 00000198 00000008 R . . . B . .
_O_Pub_Obj_Inf11_Event0xE                    .text 00A86D30 00000211 00000028 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0x2                    .text 00A86F50 0000083F 0000009C 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x3                    .text 00A87790 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0x4                    .text 00A87900 0000023B 00000058 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0x5                    .text 00A87B40 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0x6                    .text 00A87CB0 000001EF 00000070 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0x7                    .text 00A87EA0 0000014C 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0x8                    .text 00A87FF0 0000014C 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0x9                    .text 00A88140 0000110C 0000008C 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0xA                    .text 00A89250 0000011D 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0xB                    .text 00A89370 000001A4 00000054 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0xC                    .text 00A89520 000001A4 00000054 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0xD                    .text 00A896D0 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf12_Event0xE                    .text 00A89840 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0xF                    .text 00A89C00 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x10                   .text 00A89FC0 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x11                   .text 00A8A380 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x12                   .text 00A8A740 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x13                   .text 00A8AB00 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x14                   .text 00A8AEC0 000001D3 00000054 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x15                   .text 00A8B0A0 000000A4 00000018 00000008 R . . . B . .
_O_Pub_Obj_Inf12_Event0x1                    .text 00A8B150 000026FC 00000230 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x2                    .text 00A8D850 000004E9 00000080 00000004 R . . . B T .
_O_Pub_Obj_Inf13_Event0x3                    .text 00A8DD40 000005B4 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x4                    .text 00A8E300 000002AC 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x5                    .text 00A8E5B0 000001EF 00000070 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x6                    .text 00A8E7A0 000001EF 00000070 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x7                    .text 00A8E990 000001EF 00000070 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x8                    .text 00A8EB80 000002D4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x9                    .text 00A8EE60 000005B4 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0xA                    .text 00A8F420 0000052E 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0xB                    .text 00A8F950 0000011D 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf13_Event0xC                    .text 00A8FA70 000002D4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0xD                    .text 00A8FD50 000005B4 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0xE                    .text 00A90310 0000345E 000002BC 00000008 R . . . B . .
_O_Pub_Obj_Inf13_Event0xF                    .text 00A93770 000000B8 0000001C 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x10                   .text 00A93830 00000136 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x11                   .text 00A93970 000006D1 000000BC 00000008 R . . . B . .
_O_Pub_Obj_Inf13_Event0x12                   .text 00A94050 00000172 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf13_Event0x1                    .text 00A941D0 0000021B 00000040 00000004 R . . . B . .
_O_Pub_Obj_Inf14_Event0x1                    .text 00A943F0 000002F1 00000074 00000008 R . . . B . .
_O_Pub_Obj_Inf14_Event0x2                    .text 00A946F0 000003F4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf14_Event0x3                    .text 00A94AF0 0000011D 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf14_Event0x4                    .text 00A94C10 000003BE 0000007C 00000008 R . . . B . .
_O_Pub_Obj_Inf15_Event0x1                    .text 00A94FD0 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0x2                    .text 00A95140 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0x3                    .text 00A952B0 000002D4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0x4                    .text 00A95590 000002D4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0x5                    .text 00A95870 000002D4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0x6                    .text 00A95B50 0000011D 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf15_Event0x7                    .text 00A95C70 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0x8                    .text 00A95DE0 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0x9                    .text 00A95F50 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf15_Event0xA                    .text 00A960C0 000005E9 00000098 00000008 R . . . B . .
_O_Pub_Obj_Inf15_Event0xB                    .text 00A966B0 00000383 00000074 00000004 R . . . B . .
_O_Pub_Obj_Inf16_Event0x1                    .text 00A96A40 0000031F 0000005C 00000004 R . . . B . .
_O_Pub_Obj_Inf16_Event0x2                    .text 00A96D60 000003DB 0000008C 00000008 R . . . B . .
_O_Pub_Obj_Inf16_Event0x3                    .text 00A97140 000002F1 00000074 00000008 R . . . B . .
_O_Pub_Obj_Inf17_Event0x1                    .text 00A97440 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf18_Method0x1                   .text 00A975B0 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf18_Event0x2                    .text 00A97720 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf18_Event0x3                    .text 00A97890 00000383 00000074 00000004 R . . . B . .
_O_Pub_Obj_Inf18_Event0x4                    .text 00A97C20 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf19_Event0x1                    .text 00A97D90 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf1A_Event0x1                    .text 00A97F00 00000161 00000050 00000004 R . . . B . .
saveOldDisplayOptions                        .text 00A98070 000000E0 000000B8 00000000 R . . . . . .
unkRestoreResolution                         .text 00A98150 0000003D 00000008 00000000 R . . . . . .
applyFullscreenResolution                    .text 00A98190 00000303 000000CC 00000010 R . . . B T .
saveDisplayOptions                           .text 00A984A0 000000FF 00000018 00000014 R . . . B T .
sortBlocksY                                  .text 00A985A0 00000333 000000A4 00000008 R . . . B T .
sortBlocksX                                  .text 00A988E0 00000333 000000A4 00000008 R . . . B T .
bgoSortingRelated                            .text 00A98C20 000003B8 000000A4 00000008 R . . . B T .
unkSortFinalize                              .text 00A98FE0 000002DA 00000054 00000000 R . . . . T .
sub_A992C0                                   .text 00A992C0 0000035B 0000001C 00000004 R . . . B T .
sub_A99620                                   .text 00A99620 00000244 0000018C 00000000 R . . . B . .
unkSortFinalize2                             .text 00A99870 000000C0 0000000C 00000000 R . . . . . .
sub_A99930                                   .text 00A99930 000002DC 00000024 00000008 R . . . B . .
sub_A99C10                                   .text 00A99C10 000003E2 00000194 00000008 R . . . B T .
bgoSortingRelatedToLoading                   .text 00A9A000 000001DD 0000002C 00000000 R . . . B . .
_O_Pub_Obj_Inf1D_Event0x1                    .text 00A9A1E0 00000243 0000002C 00000008 R . . . B . .
_O_Pub_Obj_Inf1D_Event0x2                    .text 00A9A430 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1D_Event0x3                    .text 00A9A4C0 000001BF 00000078 00000004 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x1                    .text 00A9A680 000000D1 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x2                    .text 00A9A760 000000D1 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x3                    .text 00A9A840 000000D1 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x4                    .text 00A9A920 000000D1 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x5                    .text 00A9AA00 000000D1 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x6                    .text 00A9AAE0 00000087 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x7                    .text 00A9AB70 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x8                    .text 00A9AC00 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x9                    .text 00A9ACC0 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0xA                    .text 00A9AD80 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0xB                    .text 00A9AE40 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0xC                    .text 00A9AF00 00000946 0000002C 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0xD                    .text 00A9B850 0000049B 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0xE                    .text 00A9BCF0 0000049B 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0xF                    .text 00A9C190 000007EA 0000002C 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x10                   .text 00A9C980 000007EA 0000002C 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x11                   .text 00A9D170 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x12                   .text 00A9D200 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x13                   .text 00A9D2C0 00000032 00000004 00000008 R . . . . . .
_O_Pub_Obj_Inf1E_Event0x14                   .text 00A9D300 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x15                   .text 00A9D390 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x16                   .text 00A9D420 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x17                   .text 00A9D4E0 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x18                   .text 00A9D570 00000019 00000000 00000008 R . . . . . .
_O_Pub_Obj_Inf1E_Event0x19                   .text 00A9D590 0000008B 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x1A                   .text 00A9D620 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x1B                   .text 00A9D6E0 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x1C                   .text 00A9D7A0 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x1D                   .text 00A9D830 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x1E                   .text 00A9D8F0 000000B5 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x1F                   .text 00A9D9B0 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x20                   .text 00A9DA40 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x21                   .text 00A9DAD0 000000B6 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf1E_Event0x22                   .text 00A9DB90 000008AE 00000020 00000008 R . . . B . .
_O_Pub_Obj_Inf1F_Method0x1                   .text 00A9E440 00000385 0000005C 00000000 R . . . B . .
_O_Pub_Obj_Inf1F_Event0x2                    .text 00A9E7D0 0000013A 0000001C 00000004 R . . . B . .
_O_Pub_Obj_Inf1F_Event0x3                    .text 00A9E910 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf20_Event0x2                    .text 00A9E9A0 000008BA 000000F4 00000004 R . . . B . .
_O_Pub_Obj_Inf20_Event0x3                    .text 00A9F260 00000B99 00000118 00000004 R . . . B . .
_O_Pub_Obj_Inf20_Event0x4                    .text 00A9FE00 00000101 0000001C 00000004 R . . . B . .
_O_Pub_Obj_Inf20_Event0x5                    .text 00A9FF10 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf20_Event0x6                    .text 00A9FFA0 00000F7B 00000118 00000004 R . . . B . .
_O_Pub_Obj_Inf20_Event0x7                    .text 00AA0F20 0000032B 00000034 00000004 R . . . B . .
_O_Pub_Obj_Inf20_Event0x8                    .text 00AA1250 00000F0F 000000F4 00000008 R . . . B . .
_O_Pub_Obj_Inf20_Event0x1                    .text 00AA2160 000005FC 00000034 00000004 R . . . B . .
showLayer                                    .text 00AA2760 00000FCF 00000144 00000008 R . . . B T .
hideLayer                                    .text 00AA3730 00000B9E 0000012C 00000008 R . . . B T .
TriggerEvent                                 .text 00AA42D0 00001E03 00000194 00000008 R . . . B T .
UpdateEvents                                 .text 00AA60E0 00000624 0000001C 00000000 R . . . . . .
UpdateLayers                                 .text 00AA6710 00001811 0000002C 00000000 R . . . . . .
_O_Pub_Obj_Inf22_Event0x2                    .text 00AA7F30 000004DC 000000E4 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x3                    .text 00AA8410 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x4                    .text 00AA87A0 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x5                    .text 00AA8B30 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x6                    .text 00AA8EC0 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x7                    .text 00AA9250 00000445 000000E4 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x8                    .text 00AA96A0 0000044D 000000E4 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x9                    .text 00AA9AF0 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0xA                    .text 00AA9E80 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0xB                    .text 00AAA210 00000343 000000E0 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0xC                    .text 00AAA560 0000040F 000000E4 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0xD                    .text 00AAA970 0000046D 000000E4 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0xE                    .text 00AAADE0 00000986 00000100 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0xF                    .text 00AAB770 0000065A 000000F4 00000008 R . . . B . .
_O_Pub_Obj_Inf22_Event0x10                   .text 00AABDD0 0000143A 000006C0 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x11                   .text 00AAD210 00000B20 00000120 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x12                   .text 00AADD30 000004BC 000000A8 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x13                   .text 00AAE1F0 0000046F 00000090 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x14                   .text 00AAE660 0000065A 000000F4 00000008 R . . . B . .
_O_Pub_Obj_Inf22_Event0x15                   .text 00AAECC0 000008F7 00000118 00000008 R . . . B . .
_O_Pub_Obj_Inf22_Event0x16                   .text 00AAF5C0 000004BC 000000A8 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x17                   .text 00AAFA80 0000046F 00000090 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x18                   .text 00AAFEF0 000004C5 000000A8 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x19                   .text 00AB03C0 00000475 00000090 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x1A                   .text 00AB0840 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x1B                   .text 00AB0BD0 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x1C                   .text 00AB0F60 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x1D                   .text 00AB12F0 00000390 00000080 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x1E                   .text 00AB1680 000006A2 000000F8 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x1F                   .text 00AB1D30 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf22_Event0x20                   .text 00AB1DC0 00000344 0000007C 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x21                   .text 00AB2110 0000013D 0000001C 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x22                   .text 00AB2250 00000681 00000134 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x23                   .text 00AB28E0 0000041F 000000FC 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x24                   .text 00AB2D00 0000041F 000000FC 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x25                   .text 00AB3120 0000036A 0000009C 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x26                   .text 00AB3490 000015D7 00000138 00000008 R . . . B . .
_O_Pub_Obj_Inf22_Event0x1                    .text 00AB4A70 0000483B 000001DC 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x27                   .text 00AB92B0 0000041F 000000FC 00000004 R . . . B . .
_O_Pub_Obj_Inf22_Event0x28                   .text 00AB96D0 0000041F 000000FC 00000004 R . . . B . .
_O_Pub_Obj_Inf23_Method0x1                   .text 00AB9AF0 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf24_Event0x1                    .text 00AB9B80 00000806 00000064 00000004 R . . . B . .
loadDefaultGraphics                          .text 00ABA390 000110A3 00000A88 00000000 R . . . B T .
clearGraphics                                .text 00ACB440 00001DD8 00000198 00000000 R . . . B T .
sub_ACD220                                   .text 00ACD220 000061F5 0000026C 00000000 R . . . B T .
unkLoadGraphics                              .text 00AD3420 00006996 000001C0 00000000 R . . . B . .
unkLoadLocalGraphics                         .text 00AD9DC0 000061CA 0000026C 00000004 R . . . B . .
loadCustomWorldGraphics                      .text 00ADFF90 00002691 00000134 00000000 R . . . B . .
loadDefaultWorldGraphics                     .text 00AE2630 00002C1E 00000144 00000000 R . . . B . .
sub_AE5250                                   .text 00AE5250 00000585 00000078 00000004 R . . . B . .
sub_AE57E0                                   .text 00AE57E0 00000DF8 00000150 00000004 R . . . B T .
sub_AE65E0                                   .text 00AE65E0 0000058D 00000078 00000004 R . . . B . .
sub_AE6B70                                   .text 00AE6B70 0000058D 00000078 00000004 R . . . B T .
sub_AE7100                                   .text 00AE7100 00000572 00000078 00000004 R . . . B . .
sub_AE7680                                   .text 00AE7680 0000069B 00000078 00000004 R . . . B . .
unkManagePath                                .text 00AE7D20 000003E9 00000034 00000004 R . . . B T .
unkUpdateCoinSpin                            .text 00AE8110 00000A16 00000038 00000000 R . . . B T .
_O_Pub_Obj_Inf26_Event0x2_sort               .text 00AE8B30 00000131 00000034 00000004 R . . . B . .
_O_Pub_Obj_Inf26_Event0x3                    .text 00AE8C70 0000045E 00000098 00000004 R . . . B . .
_O_Pub_Obj_Inf26_Event0x4                    .text 00AE90D0 000000F8 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf26_Event0x5                    .text 00AE91D0 00000069 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf26_Event0x1                    .text 00AE9240 000014FA 00000254 00000004 R . . . B . .
_O_Pub_Obj_Inf26_Event0x6                    .text 00AEA740 0000042F 000000A4 00000004 R . . . B . .
_O_Pub_Obj_Inf27_Event0x2                    .text 00AEAB70 00000085 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf27_Event0x3                    .text 00AEAC00 000000E1 0000001C 00000008 R . . . B . .
_O_Pub_Obj_Inf27_Event0x4                    .text 00AEACF0 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf27_Event0x5                    .text 00AEAE60 00000317 00000068 00000004 R . . . B . .
_O_Pub_Obj_Inf27_Method0x1                   .text 00AEB180 0000085C 00000030 00000004 R . . . B . .
_O_Pub_Obj_Inf27_Event0x6                    .text 00AEB9E0 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf27_Event0x7                    .text 00AEBB50 000004B8 00000090 00000004 R . . . B . .
_O_Pub_Obj_Inf28_Event0x1                    .text 00AEC010 000001A3 00000038 00000004 R . . . B . .
_O_Pub_Obj_Inf28_Event0x2                    .text 00AEC1C0 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf28_Event0x3                    .text 00AEC250 000002D4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf28_Event0x4                    .text 00AEC530 0000015B 0000002C 00000004 R . . . B . .
_O_Pub_Obj_Inf29_Method0x1                   .text 00AEC690 000002D4 00000064 00000004 R . . . B . .
_O_Pub_Obj_Inf29_Event0x2                    .text 00AEC970 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf29_Event0x3                    .text 00AECAE0 0000023B 00000058 00000004 R . . . B . .
_O_Pub_Obj_Inf29_Event0x4                    .text 00AECD20 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf2A_Method0x1                   .text 00AECE90 000001C1 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x19                   .text 00AED060 000002BB 00000030 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x1A                   .text 00AED320 00000798 00000088 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x1B                   .text 00AEDAC0 000009C7 000001D8 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x1C                   .text 00AEE490 00000411 00000054 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x1D                   .text 00AEE8B0 00000723 00000068 00000009 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x1E                   .text 00AEEFE0 00000A80 00000110 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x1                    .text 00AEFA60 0001C64F 00000E24 00000010 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x2                    .text 00B0C0B0 000003F6 00000088 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0x3                    .text 00B0C4B0 000026BA 000001A0 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x4                    .text 00B0EB70 00000722 00000074 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x5                    .text 00B0F2A0 00000208 00000054 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x6                    .text 00B0F4B0 00000646 000000D4 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0x7                    .text 00B0FB00 000003E8 00000084 00000010 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x8                    .text 00B0FEF0 00000358 00000060 00000010 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x9                    .text 00B10250 00000324 00000064 00000010 R . . . B . .
_O_Pub_Obj_Inf2B_Event0xA                    .text 00B10580 00000306 00000070 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0xB                    .text 00B10890 00000B4B 000000C4 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0xC                    .text 00B113E0 00000355 00000080 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0xD                    .text 00B11740 00000A63 000000F8 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0xE                    .text 00B121B0 0000026E 00000064 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0xF                    .text 00B12420 0000026B 0000005C 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0x10                   .text 00B12690 00001160 00000138 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0x11                   .text 00B137F0 00000183 0000003C 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x12                   .text 00B13980 00000C31 00000340 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0x13                   .text 00B145C0 000003B8 00000090 0000000C R . . . B . .
_O_Pub_Obj_Inf2B_Event0x14                   .text 00B14980 00000CFC 00000058 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x15                   .text 00B15680 0000049C 00000058 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x16                   .text 00B15B20 0000195E 00000108 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x1F                   .text 00B17480 0000040D 00000084 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x20                   .text 00B17890 000000D1 0000001C 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x21                   .text 00B17970 0000012B 0000002C 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x22                   .text 00B17AA0 000007C9 000000D4 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x23                   .text 00B18270 00000289 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x24                   .text 00B18500 00000476 00000054 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x25                   .text 00B18980 000016D7 00000068 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x17                   .text 00B1A060 00000D2A 000000E0 00000008 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x18                   .text 00B1AD90 00000691 0000002C 00000004 R . . . B . .
_O_Pub_Obj_Inf2B_Event0x26                   .text 00B1B430 000002BB 00000030 00000004 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x1                    .text 00B1B6F0 00000268 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x2                    .text 00B1B960 00001DD1 00000294 00000004 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x3                    .text 00B1D740 000000DA 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x4                    .text 00B1D820 00000174 00000050 00000008 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x5                    .text 00B1D9A0 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x6                    .text 00B1DA30 000001E4 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x7                    .text 00B1DC20 00000161 00000050 00000004 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x8                    .text 00B1DD90 000000DA 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf2C_Event0x9                    .text 00B1DE70 000006A0 000000B0 00000004 R . . . B . .
_O_Pub_Obj_Inf2D_Method0x1                   .text 00B1E510 0000008F 00000024 00000008 R . . . B . .
_O_Pub_Obj_Inf2D_Event0x2                    .text 00B1E5A0 000011E1 00000104 00000008 R . . . B . .
_O_Pub_Obj_Inf2E_Event0x1                    .text 00B1F790 0000034C 0000004C 00000004 R . . . B . .
_O_Pub_Obj_Inf2E_Event0x2                    .text 00B1FAE0 0000030C 00000030 00000004 R . . . B . .
_O_Pub_Obj_Inf2E_Event0x3_tmrLoad_Timer      .text 00B1FDF0 00000207 0000001C 00000004 R . . . B T .
_O_Pub_Obj_Inf2F_Event0x1                    .text 00B20000 00000074 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf2F_Event0x2                    .text 00B20080 00000044 00000024 00000004 . . . . B . .
_O_Pub_Obj_Inf2F_Event0x3                    .text 00B200F0 0000006E 00000024 00000004 R . . . B . .
_O_Pub_Obj_Inf2F_Event0x4                    .text 00B20160 000000C4 00000028 00000004 R . . . B . .
_O_Pub_Obj_Inf2F_Event0x5                    .text 00B20230 00000074 00000024 00000004 R . . . B . .
saveNPCDefaults                              .text 00B202B0 000005B1 0000000C 00000000 R . . . . . .
loadDefaultNPCConfiguration                  .text 00B20870 000005DD 0000000C 00000000 R . . . . T .
loadNPCConfigurations                        .text 00B20E50 000003A4 0000004C 00000004 R . . . B . .
loadNPCConfiguration                         .text 00B21200 000016F6 0000006C 00000008 R . . . B T .
_O_Pub_Obj_Inf31_Event0x1                    .text 00B22900 000000B2 0000001C 00000004 R . . . B . .
_O_Pub_Obj_Inf31_Event0x3                    .text 00B229C0 00000154 00000030 00000004 R . . . B . .
_O_Pub_Obj_Inf31_Event0x4                    .text 00B22B20 00000288 00000038 00000004 R . . . B . .
_O_Pub_Obj_Inf31_Event0x5                    .text 00B22DB0 000000DA 00000020 00000004 R . . . B . .
_O_Pub_Obj_Inf31_Event0x2                    .text 00B22E90 000010B0 00000088 0000000C R . . . B . .
_O_Pub_Obj_Inf31_Event0x6                    .text 00B23F40 000000A7 0000000C 00000010 R . . . . . .

*/

//DEBUG:
#define dbgbox(msg) LunaMsgBox::ShowW(NULL, msg, L"Dbg", NULL);
#define dbgboxA(msg) LunaMsgBox::ShowA(NULL, msg, "Dbg", NULL);

#endif


