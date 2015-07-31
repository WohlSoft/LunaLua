//*** Defines.h - It has defines in it, also basically the known RAM map. ***
#ifndef Defines_hhhh
#define Defines_hhhh

#include "resource.h"
#include <windows.h>
#include "Misc/VB6StrPtr.h"

// The version number is now defined in resource.h, as it now compiled as meta-data too.
// If you want to define your "custom" version number anyway, then set NORMAL_VER_NUM to 0 and modify the LUNALUA_VERSION string.
#define NORMAL_VER_NUM 1

#if NORMAL_VER_NUM
#define LUNALUA_VER_PREFIX   "LUNALUA V"
#define LUNALUA_VER_SUFFIX   " BETA"
#define LUNALUA_VERSION      LUNALUA_VER_PREFIX LUNALUA_VER_STR LUNALUA_VER_SUFFIX
#else
#define LUNALUA_VERSION      "LUNALUA V0.7.0.3 BETA"
#endif



#define LUNA_VERSION        8
#define COMPILE_PLAYGROUND  0 //See Misc/Playground.cpp
#define COMBOOL(b) (b ? -1 : 0)

enum FIELDTYPE {
    FT_INVALID = 0,
    FT_BYTE = 1,
    FT_WORD = 2,
    FT_DWORD = 3,
    FT_FLOAT = 4,
    FT_DFLOAT = 5
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

enum Characters : short {
    CHARACTER_MARIO = 1,    //DEMO
    CHARACTER_LUIGI = 2,    //IRIS
    CHARACTER_PEACH = 3,    //KOOD
    CHARACTER_TOAD = 4,    //RAOCOW
    CHARACTER_LINK = 5     //SHEATH
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

#define DEFMEM(name, type, addr) static auto& name = *(type*)(addr)

// General
DEFMEM(GM_SCRN_HDC,         DWORD, 0x00B25028);
DEFMEM(GM_MODE_INTRO,       WORD,  0x00B2C89C);

// Pre-Defined Strings      
DEFMEM(GM_STR_NULL,         VB6StrPtr, 0x00423D00);
DEFMEM(GM_STR_MSGBOX,       VB6StrPtr, 0x00B250E4);
DEFMEM(GM_STR_CHECKPOINT,   VB6StrPtr, 0x00B250B0);     //This contains the levelname (GM_FULLPATH) of the hitted checkpoint

// NPC Settings
DEFMEM(GM_CONF_WIDTH,       WORD*, 0x00B25BA8);
DEFMEM(GM_CONF_HEIGHT,      WORD*, 0x00B25BC4);
DEFMEM(GM_CONF_GFXWIDTH,    WORD*, 0x00B25BE0);
DEFMEM(GM_CONF_GFXHEIGHT,   WORD*, 0x00B25BFC);

// Frame counters
DEFMEM(GM_TRANS_FRAMECT,  DWORD, 0x00B2C670);
//DEFMEM(GM_ACTIVE_FRAMECT, DWORD, 0x00B2C67C);  Float?

// MOB Related memory
DEFMEM(GM_NPCS_PTR,         void*, 0x00B259E8);     // +0xAD58 + 0x20  to NPCs
DEFMEM(GM_NPCS_COUNT,       WORD,  0x00B2595A); 
DEFMEM(GM_PLAYERS_PTR,      void*, 0x00B25A20);
DEFMEM(GM_PLAYERS_COUNT,    WORD,  0x00B2595E);
DEFMEM(GM_EDIT_PLAYERS_PTR, void*, 0x00CF74D8);     // Editor Template player

// HUD stuff
DEFMEM(GM_STAR_COUNT,       DWORD, 0x00B251E0);
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
DEFMEM(GM_MOUSEMOVING,      WORD,  0x00B2D6D2);
DEFMEM(GM_MOUSERELEASED,    WORD,  0x00B2D6D0);
DEFMEM(GM_MOUSEPRESSING,    WORD,  0x00B2D6CC);
DEFMEM(GM_KEYRELEASED,      WORD,  0x00B2C884);


// States
DEFMEM(GM_FREEZWITCH_ACTIV, WORD,  0x00B2C8B4);
DEFMEM(GM_PAUSE_OPEN,       WORD,  0x00B250E2);

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
DEFMEM(GM_LVL_BOUNDARIES,   Bounds*, 0x00B257D4);     // 6 doubles each section, L/U/R/D/?/?
DEFMEM(GM_ORIG_LVL_BOUNDS,  Bounds*, 0x00B2587C);     // Same as above, but always the initial values. Used by events that reset level boundaries.
DEFMEM(GM_LVLFILENAME_PTR,  VB6StrPtr, 0x00B2C5A4);   // Lvl filename
DEFMEM(GM_LVLNAME_PTR,      VB6StrPtr, 0x00B2D764);
DEFMEM(GM_FULLPATH,         VB6StrPtr, 0x00B2C618);   // Full path to current .lvl file
DEFMEM(GM_FULLDIR,          VB6StrPtr, 0x00B2C61C);   // Full path to current world dir
DEFMEM(GM_CUR_LVL,          WORD,  0x00B2C6D8);

// Background objects
DEFMEM(GM_BGO_COUNT,        WORD,  0x00B25958);
DEFMEM(GM_BGOS_PTR,         void*, 0x00B259B0);

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

// Backgrounds
DEFMEM(GM_SEC_BG_ARRAY,     short*,  0x00B258B8);
DEFMEM(GM_BG_XPOS_PTR,      double*, 0x00B2B984);

// Animations
DEFMEM(GM_ANIM_COUNT,       WORD,  0x00B2595C);
DEFMEM(GM_ANIM_PTR,         void*, 0x00B259CC);

// Sound
DEFMEM(GM_MUSIC_PATHS_PTR,  VB6StrPtr*, 0x00B257B8); 
DEFMEM(GM_SEC_MUSIC_TBL,    short*, 0x00B25828);     // 21 shorts containing music # for each section

// Input
DEFMEM(GM_VKEY_TABLE_PTR,   void*,  0x00B25068); 
DEFMEM(GM_VJOY_TABLE_PTR,   void*,  0x00B25084);
DEFMEM(GM_INPUTTYPE,        short*, 0x00B250A0);
DEFMEM(GM_INPUTSTR_BUF_PTR, VB6StrPtr, 0x00B2C898);

// Saves
DEFMEM(GM_CUR_SAVE_SLOT,    WORD,  0x00B2C62A);      // 1 2 or 3

// Cheats
DEFMEM(GM_PLAYER_INVULN,    WORD,  0x00B2C8C0);      // 0xFFFF = invuln
DEFMEM(GM_PLAYER_INFJUMP,   WORD,  0x00B2C8AC);      // 0xFFFF = infinite jumps
DEFMEM(GM_CHEATED,          WORD,  0x00B2C8C4);      // 0xFFFF = cheated

// Miscs
DEFMEM(GM_GRAVITY,          WORD,  0x00B2C6F4);      // 12 = default, 0 = non-falling
DEFMEM(GM_JUMPHIGHT,        WORD,  0x00B2C6DC);      // 20 = default, higher number = higher jumps
DEFMEM(GM_JUMPHIGHT_BOUNCE, WORD,  0x00B2C6E2);      // Same as normal jumphight, but from NPC
DEFMEM(GM_EARTHQUAKE,       WORD,  0x00B250AC);      // 0 = default, higher number = bigger shaking, slowly resets to 0

DEFMEM(GM_MARIO_VS_LUIGI_T, WORD,  0x00B2D760);      // 0 = default, if higher than 0 then display text "Mario VS Luigi"
DEFMEM(GM_WINS_T,           WORD,  0x00B2D762);      // 0 = default, if higher than 0 then display text "WINS!"

DEFMEM(GM_WINNING,          WORD,  0x00B2C59E);      // 0 = not winning, if higher than 0 then winning by this win-type
DEFMEM(GM_WORLD_MODE,       WORD,  0x00B2C5B4);      // 0xFFFF = leave current level
DEFMEM(GM_INTRO_MODE,       WORD,  0x00B2C620);

DEFMEM(GM_UNK_OV_DATABLOCK, short*,0x00B25164);     // Pointer to some kind of overworld data block involving locked character selection (not 100% sure)

//Hitbox
DEFMEM(GM_HITBOX_H_PTR,     short*,0x00B2C6FC);      // player hitbox height for each character/power-up state (starts with small mario through small link, then cycles same way through each power up)
DEFMEM(GM_HITBOX_H_D_PTR,   short*,0x00B2C742);      // hitbox heights while ducking
DEFMEM(GM_HITBOX_W_PTR,     short*,0x00B2C788);      // hitbox widths

//Startup Config:
DEFMEM(GM_ISLEVELEDITORMODE, WORD, 0x00B25134);
DEFMEM(GM_ISGAME,           WORD,  0x00B25046);
DEFMEM(GM_NOSOUND,          WORD,  0x00B2D734);
DEFMEM(GM_FRAMESKIP,        WORD,  0x00B2C684);

//Graphics Memory
DEFMEM(GM_GFX_NPC_PTR,      void*, 0x00B2CA98);     // Array of NPC graphics HDCs, len 300, indexed by (npc.id - 1)
DEFMEM(GM_GFX_NPC_MASK_PTR, void*, 0x00B2CAB4);     // Array of NPC mask graphics HDCs, len 300, indexed by (npc.id - 1)

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

DEFMEM(npcdef_gfxoffsetx,       short*, 0x00B25FC0);
DEFMEM(npcdef_gfxoffsety,       short*, 0x00B2621A);
DEFMEM(npcdef_width,            short*, 0x00B26474);
DEFMEM(npcdef_height,           short*, 0x00B266CE);
DEFMEM(npcdef_gfxwidth,         short*, 0x00B26928);
DEFMEM(npcdef_gfxheight,        short*, 0x00B26B82);
DEFMEM(npcdef_isShell,          short*, 0x00B26DDC);
DEFMEM(npcdef_npcblock,         short*, 0x00B27036);
DEFMEM(npcdef_npcblocktop,      short*, 0x00B27290);
DEFMEM(npcdef_isInteractableNPC,short*, 0x00B274EA);
DEFMEM(npcdef_isCoin,           short*, 0x00B27744);
DEFMEM(npcdef_isVine,           short*, 0x00B2799E);
DEFMEM(npcdef_isCollectableGoal,short*, 0x00B27BF8);
DEFMEM(npcdef_isFlyingNPC,      short*, 0x00B27E52);
DEFMEM(npcdef_isWaterNPC,       short*, 0x00B280AC);
DEFMEM(npcdef_jumphurt,         short*, 0x00B28306);
DEFMEM(npcdef_noblockcollision, short*, 0x00B28560);
DEFMEM(npcdef_score,            short*, 0x00B287BA);
DEFMEM(npcdef_playerblocktop,   short*, 0x00B28A14);
DEFMEM(npcdef_grabtop,          short*, 0x00B28C6E);
DEFMEM(npcdef_cliffturn,        short*, 0x00B28EC8);
DEFMEM(npcdef_nohurt,           short*, 0x00B29122);
DEFMEM(npcdef_playerblock,      short*, 0x00B2937C);
DEFMEM(npcdef_grabside,         short*, 0x00B29830);
DEFMEM(npcdef_isShoeNPC,        short*, 0x00B29A8A);
DEFMEM(npcdef_isYoshiNPC,       short*, 0x00B29CE4);
DEFMEM(npcdef_noYoshi,          short*, 0x00B2A198);
DEFMEM(npcdef_foreground,       short*, 0x00B2A3F2);
DEFMEM(npcdef_isBot,            short*, 0x00B2A64C);
DEFMEM(npcdef_isVegetableNPC,   short*, 0x00B2AB00);
DEFMEM(npcdef_speed,            float*, 0x00B2AD5C);
DEFMEM(npcdef_nofireball,       short*, 0x00B2B210);
DEFMEM(npcdef_noiceball,        short*, 0x00B2B46A);
DEFMEM(npcdef_nogravity,        short*, 0x00B2B6C4);

// Frame timing related references
DEFMEM(GM_LAST_FRAME_TIME, double, 0x00B2D72C);
DEFMEM(GM_CURRENT_TIME,    double, 0x00B2D738);
DEFMEM(GM_MAX_FPS_MODE,    short,  0x00B2C8BE);

/////////////////////
///  -Assembly-   ///
/////////////////////

//VASM = value is assembly code
//npcToCoins ending Animation Settings
DEFMEM(VASM_END_ANIM,      BYTE, 0x00A3C86E);      // = 11
DEFMEM(VASM_END_COINSOUND, BYTE, 0x00A3C87F);      // = 14
DEFMEM(VASM_END_COINVAL,   BYTE, 0x00A3C891);      // = 1

////////////////////////
///    -Imports-     ///
////////////////////////

DEFMEM(IMP_vbaStrCmp,       void*, 0x004010F8); // Ptr to __stdcall
DEFMEM(IMP_vbaStrCopy,      void*, 0x004011b0); // Ptr to __fastcall
DEFMEM(IMP_vbaFreeStr,      void*, 0x00401248); // Ptr to __fastcall
DEFMEM(IMP_rtcRandomize,    void*, 0x00401090); // Ptr to __stdcall

////////////////////////
///    -Functions-   ///
////////////////////////

// Print using game's text function

//      Arg1 = WCSTR* string
//      Arg2 = Int* fonttype
//      Arg3 = float* x
//      Arg4 = float* y
#define GF_PRINT            0x00951F50

// Start kill event for a player
//      Arg1 = int* to Index of player
#define GF_KILL_PLAYER      0x009B66D0

//      Arg1 = int* Index of player
#define GF_HARM_PLAYER      0x009B51E0

//      Arg1 = int* Sprite ID
//      Arg2 = int* Unk flags
//      Arg3 = int* Unk
#define GF_INIT_NPC         0x00A03630

//      Arg1 = int* Index of NPC in NPC list
#define GF_UPDATE_NPC       0x00A3B680

//      Arg1 = int* Index of section containing music settings to play now
#define GF_PLAY_MUSIC       0x00A61B40

//      Arg1 = int* SoundIndex
#define GF_PLAY_SFX         0x00A73FD0

//      Arg1 = POS* structure of player, POS* structure of block
//      1=Collision from top, 2=From right, 3=From bottom, 4=From left, 5=?
#define GF_MOB_BLOCK_COL    0x00994250

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

//      No Args, Does convert every NPC to coins (Like player would win)
#define GF_NPC_TO_COINS     0x00A3C580

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

//      Arg1 = VB6StrPtr* layerName
//      Arg2 = short* [VB 6 Bool] NoSmoke: False, to display the smoke effect
#define GF_SHOW_LAYER       0x00AA2760

//      Arg1 = VB6StrPtr* layerName
//      Arg2 = short* [VB 6 Bool] NoSmoke: False, to display the smoke effect
#define GF_HIDE_LAYER       0x00AA3730

#define GF_POW              0x009E4600

#define GF_LOAD_LOCAL_GFX   0x00ACD220

//      Arg1 = VB6StrPtr* Added text to the cheat buffer and execute
#define GF_UPDATESCAN_CHEAT 0x008E7490

#define GF_LOAD_WORLD_LIST  0x008E35E0

#define GF_LOAD_SAVE_STATES 0x008E41D0

//      Arg1 = Pointer to structure
#define GF_THUN_RT_MAIN     0x0040BDD2

DEFMEM(GF_RTC_DO_EVENTS, void*, 0x004010B8);

static const auto native_print          = (void(__stdcall *)(VB6StrPtr* /*Text*/, short* /*fonttype*/, float* /*x*/, float* /*y*/))GF_PRINT;

static const auto native_isOnCamera     = (short(__stdcall *)(unsigned int* /*camIndex*/, Momentum* /*momentumObj*/))GF_IS_ON_CAMERA;
static const auto native_isOnWCamera    = (short(__stdcall *)(unsigned int* /*camIndex*/, Momentum* /*momentumObj*/))GF_IS_ON_WCAMERA;


static const auto native_killPlayer     = (void(__stdcall *)(short* /*playerIndex*/))GF_KILL_PLAYER;
static const auto native_harmPlayer     = (void(__stdcall *)(short* /*playerIndex*/))GF_HARM_PLAYER;

static const auto native_playMusic      = (void(__stdcall *)(short* /*section*/))GF_PLAY_MUSIC;
static const auto native_playSFX        = (void(__stdcall *)(short* /*soundIndex*/))GF_PLAY_SFX;

static const auto native_npcToCoins     = (void(__stdcall *)())GF_NPC_TO_COINS;
static const auto native_doPow          = (void(__stdcall *)())GF_POW;

static const auto native_runEffect      = (void(__stdcall *)(short* /*EffectID*/, Momentum* /*coor*/, float* /*EffectFrame*/, short* /*npcID*/, short* /*showOnlyMask*/))GF_RUN_ANIM;
static const auto native_addScoreEffect = (void(__stdcall *)(short* /*baseValue*/, Momentum* /*coor*/, short* /*factor*/))GF_SCORE_RELEATED;

static const auto native_msgbox         = (void(__stdcall *)(short* /*unkVal*/))GF_MSGBOX;

static const auto native_triggerEvent   = (void(__stdcall *)(VB6StrPtr* /*eventName*/, short* /*forceNoSmoke*/))GF_TRIGGER_EVENT;

static const auto native_showLayer      = (void(__stdcall *)(VB6StrPtr* /*layerName*/, short* /*noSmoke*/))GF_SHOW_LAYER;
static const auto native_hideLayer      = (void(__stdcall *)(VB6StrPtr* /*layerName*/, short* /*noSmoke*/))GF_HIDE_LAYER;

static const auto native_loadLocalGfx   = (void(__stdcall *)())GF_LOAD_LOCAL_GFX;

static const auto native_updateCheatbuf = (void(__stdcall *)(VB6StrPtr* /*addedText*/))GF_UPDATESCAN_CHEAT;

static const auto native_loadWorldList  = (void(__stdcall *)())GF_LOAD_WORLD_LIST;
static const auto native_loadSaveStates = (void(__stdcall *)())GF_LOAD_SAVE_STATES;

static const auto native_ThunRTMain     = (void(__stdcall *)(void *))GF_THUN_RT_MAIN;

static const auto native_rtcDoEvents    = (void(__stdcall *)())GF_RTC_DO_EVENTS;

/*
_O_Pub_Obj_Inf1_Event0x3                008BD770 P
_O_Pub_Obj_Inf1_Event0x4                008BD900 P
_O_Pub_Obj_Inf1_Event0x5                008BD9E0 P
_O_Pub_Obj_Inf1_Event0x6                008BDAB0 P
_O_Pub_Obj_Inf1_Event0x7                008BDB40 P
_O_Pub_Obj_Inf1_Event0x8                008BDE80 P
_O_Pub_Obj_Inf1_Event0x9                008BDF10 P
_O_Pub_Obj_Inf1_Event0xA                008BE080 P
_O_Pub_Obj_Inf1_Event0xB                008BE100 P
_O_Pub_Obj_Inf1_Event0xC                008BE1A0 P
_O_Pub_Obj_Inf1_Event0xD                008BE360 P
unkInitVals                             008BE410
Sub_Main                                008BE9C0 P
loadNPCDefaults                         008C2720
levelLoop                               008CA210
introLoop                               008CA650
exitMainGame                            008D6BB0
saveLevel                               008D6CF0
loadLevel                               008D8F40
goToWorldmap                            008DDE30
doWinAnimations                         008DE080
saveWorld                               008DEB60
loadWorld                               008DF5B0
worldLoop                               008E06B0
loadWorlds                              008E35E0
enumAllBattleLevels                     008E3D10
unkCountPercentageOfGameSave            008E41D0
saveGame                                008E47D0
loadGame                                008E4E00
unkMessageBox_8E54C0                    008E54C0
loadDefaultControls                     008E6700
loadGameConfiguration                   008E6910
unkMusicManagement_8E6F70               008E6F70
updateScanCheat                         008E7490
constructCreditsText                    008F7200
unkDoorsCount                           008F7D70
addToCredits                            008F8000
createMomentumStruct                    008F8050
funcScoreRelated                        008F80C0
unkBlockBoolInit                        008FE2A0
unkRenderWorldHUD_8FEB10                008FEB10
unkRender_909290                        00909290
initGameHDC                             0094F680
setLevelWindowDefaults                  0094F8D0
PrintText                               00951F50
unkBackgroundDraw                       00954F50
initGameWindow                          0096AD80
unkRenderHud_96BF20                     0096BF20
unkPaintNPCOnScreen                     009920D0
unkCollisionCheck                       00992F50
IsOnCamera                              00993DE0
MobBlockColl                            00994250
runGameLoop                             00995A20
HarmPlayer                              009B51E0
KillPlayer                              009B66D0
unkDuckingUpdate                        009B7BF0
unkPickupAnimationRelated               009BB2B0
unkPlayerUpdate9C0C50                   009C0C50
unkPlayerCollisionCheck                 009CAEC0
togglePSwitch                           009E33B0
doPOW                                   009E4600
updateEffect                            009E4900
runEffect                               009E7380
cleanupEffect                           009EC150
runEditorLevel                          00A02220
InitNPC                                 00A03630
unkOnInteractA24CD0                     00A24CD0
NpcKill                                 00A315A0
UpdateNPC                               00A3B680
doBombExplosion                         00A3BA90
npcs_to_coins                           00A3C580
triggerMovementOfSkullRaft              00A446A0
updateNPC                               00A448D0
updateNPCRelated                        00A52BB0
unkOnUpdateMovementNPC                  00A60AF0
PlayMusic                               00A61B40
initAssignSoundFiles                    00A624D0
playSound                               00A73FD0
unkCheckMusicState                      00A74460
updateInput                             00A74910
unkJoystick                             00A75680
_O_Pub_Obj_InfD_Event0x1                00A75970 P
_O_Pub_Obj_InfD_Event0x2                00A75A40 P
_O_Pub_Obj_InfD_Event0x3                00A75AB0 P
_O_Pub_Obj_InfD_Event0x4                00A75BE0 P
_O_Pub_Obj_InfD_Event0x5                00A75D10 P
_O_Pub_Obj_InfD_Event0x6                00A75D80 P
_O_Pub_Obj_InfD_Event0x7                00A75E00 P
_O_Pub_Obj_InfD_Event0x8                00A76100 P
_O_Pub_Obj_InfD_Event0x9                00A761B0 P
_O_Pub_Obj_InfE_Event0x1                00A76A80 P
_O_Pub_Obj_InfE_Event0x2                00A76B50 P
_O_Pub_Obj_InfE_Event0x3                00A76BC0 P
_O_Pub_Obj_InfE_Event0x4                00A76CF0 P
_O_Pub_Obj_InfE_Event0x5                00A76E90 P
_O_Pub_Obj_InfE_Event0x6                00A77060 P
_O_Pub_Obj_InfE_Event0x7                00A773D0 P
_O_Pub_Obj_InfE_Event0x8                00A77480 P
_O_Pub_Obj_InfE_Event0x9                00A77500 P
_O_Pub_Obj_InfF_Event0x1                00A77F30 P
_O_Pub_Obj_InfF_Event0x2                00A78050 P
_O_Pub_Obj_InfF_Event0x3                00A78170 P
_O_Pub_Obj_InfF_Event0x4                00A7C7B0 P
_O_Pub_Obj_InfF_Event0x5                00A7C820 P
_O_Pub_Obj_InfF_Event0x6                00A7C890 P
_O_Pub_Obj_InfF_Event0x7                00A7CB40 P
_O_Pub_Obj_InfF_Event0x8                00A7CDF0 P
_O_Pub_Obj_InfF_Event0x9                00A7CEE0 P
_O_Pub_Obj_InfF_Event0xA                00A7D0D0 P
_O_Pub_Obj_InfF_Event0xB                00A7D140 P
_O_Pub_Obj_InfF_Event0xC                00A7D570 P
_O_Pub_Obj_InfF_Event0xD                00A7D680 P
_O_Pub_Obj_InfF_Event0xE                00A7D790 P
_O_Pub_Obj_InfF_Event0xF                00A7D980 P
_O_Pub_Obj_InfF_Event0x10               00A7DBE0 P
_O_Pub_Obj_InfF_Event0x11               00A7DC70 P
_O_Pub_Obj_InfF_Event0x12               00A7DCE0 P
_O_Pub_Obj_InfF_Event0x13               00A7DE00 P
_O_Pub_Obj_InfF_Event0x14               00A7DE80 P
_O_Pub_Obj_InfF_Event0x15               00A7DF00 P
_O_Pub_Obj_InfF_Event0x16               00A7E020 P
_O_Pub_Obj_InfF_Event0x17               00A7E0A0 P
_O_Pub_Obj_InfF_Event0x18               00A7E120 P
_O_Pub_Obj_InfF_Event0x19               00A7E1A0 P
_O_Pub_Obj_InfF_Event0x1A               00A7E220 P
_O_Pub_Obj_InfF_Event0x1B               00A7E2B0 P
_O_Pub_Obj_InfF_Event0x1C               00A7E330 P
_O_Pub_Obj_InfF_Event0x1D               00A7E420 P
_O_Pub_Obj_InfF_Event0x1E               00A7E8C0 P
_O_Pub_Obj_InfF_Event0x1F               00A7E9B0 P
_O_Pub_Obj_InfF_Event0x20               00A7EF10 P
_O_Pub_Obj_Inf10_Method0x1              00A814C0 P
_O_Pub_Obj_Inf10_Event0x2               00A81530 P
_O_Pub_Obj_Inf10_Event0x3               00A81E20 P
_O_Pub_Obj_Inf10_Event0x4               00A81E90 P
_O_Pub_Obj_Inf10_Event0x5               00A82050 P
_O_Pub_Obj_Inf10_Event0x6               00A820C0 P
_O_Pub_Obj_Inf10_Event0x7               00A82140 P
_O_Pub_Obj_Inf10_Event0x8               00A82210 P
_O_Pub_Obj_Inf10_Event0x9               00A82280 P
_O_Pub_Obj_Inf10_Event0xA               00A824E0 P
_O_Pub_Obj_Inf10_Event0xB               00A829E0 P
_O_Pub_Obj_Inf11_Event0x1               00A82A60 P
_O_Pub_Obj_Inf11_Event0x2               00A82EC0 P
_O_Pub_Obj_Inf11_Event0x3               00A83300 P
_O_Pub_Obj_Inf11_Event0x4               00A83740 P
_O_Pub_Obj_Inf11_Event0x5               00A83B80 P
_O_Pub_Obj_Inf11_Event0x6               00A83FC0 P
_O_Pub_Obj_Inf11_Event0x7               00A84280 P
_O_Pub_Obj_Inf11_Event0x8               00A843A0 P
_O_Pub_Obj_Inf11_Event0x9               00A84840 P
_O_Pub_Obj_Inf11_Event0xA               00A84A00 P
_O_Pub_Obj_Inf11_Event0xB               00A84DC0 P
_O_Pub_Obj_Inf11_Event0xC               00A850C0 P
_O_Pub_Obj_Inf11_Event0xD               00A85590 P
_O_Pub_Obj_Inf11_Event0xE               00A86D30 P
_O_Pub_Obj_Inf12_Event0x2               00A86F50 P
_O_Pub_Obj_Inf12_Event0x3               00A87790 P
_O_Pub_Obj_Inf12_Event0x4               00A87900 P
_O_Pub_Obj_Inf12_Event0x5               00A87B40 P
_O_Pub_Obj_Inf12_Event0x6               00A87CB0 P
_O_Pub_Obj_Inf12_Event0x7               00A87EA0 P
_O_Pub_Obj_Inf12_Event0x8               00A87FF0 P
_O_Pub_Obj_Inf12_Event0x9               00A88140 P
_O_Pub_Obj_Inf12_Event0xA               00A89250 P
_O_Pub_Obj_Inf12_Event0xB               00A89370 P
_O_Pub_Obj_Inf12_Event0xC               00A89520 P
_O_Pub_Obj_Inf12_Event0xD               00A896D0 P
_O_Pub_Obj_Inf12_Event0xE               00A89840 P
_O_Pub_Obj_Inf12_Event0xF               00A89C00 P
_O_Pub_Obj_Inf12_Event0x10              00A89FC0 P
_O_Pub_Obj_Inf12_Event0x11              00A8A380 P
_O_Pub_Obj_Inf12_Event0x12              00A8A740 P
_O_Pub_Obj_Inf12_Event0x13              00A8AB00 P
_O_Pub_Obj_Inf12_Event0x14              00A8AEC0 P
_O_Pub_Obj_Inf12_Event0x15              00A8B0A0 P
_O_Pub_Obj_Inf12_Event0x1               00A8B150 P
_O_Pub_Obj_Inf13_Event0x2               00A8D850 P
_O_Pub_Obj_Inf13_Event0x3               00A8DD40 P
_O_Pub_Obj_Inf13_Event0x4               00A8E300 P
_O_Pub_Obj_Inf13_Event0x5               00A8E5B0 P
_O_Pub_Obj_Inf13_Event0x6               00A8E7A0 P
_O_Pub_Obj_Inf13_Event0x7               00A8E990 P
_O_Pub_Obj_Inf13_Event0x8               00A8EB80 P
_O_Pub_Obj_Inf13_Event0x9               00A8EE60 P
_O_Pub_Obj_Inf13_Event0xA               00A8F420 P
_O_Pub_Obj_Inf13_Event0xB               00A8F950 P
_O_Pub_Obj_Inf13_Event0xC               00A8FA70 P
_O_Pub_Obj_Inf13_Event0xD               00A8FD50 P
_O_Pub_Obj_Inf13_Event0xE               00A90310 P
_O_Pub_Obj_Inf13_Event0xF               00A93770 P
_O_Pub_Obj_Inf13_Event0x10              00A93830 P
_O_Pub_Obj_Inf13_Event0x11              00A93970 P
_O_Pub_Obj_Inf13_Event0x12              00A94050 P
_O_Pub_Obj_Inf13_Event0x1               00A941D0 P
_O_Pub_Obj_Inf14_Event0x1               00A943F0 P
_O_Pub_Obj_Inf14_Event0x2               00A946F0 P
_O_Pub_Obj_Inf14_Event0x3               00A94AF0 P
_O_Pub_Obj_Inf14_Event0x4               00A94C10 P
_O_Pub_Obj_Inf15_Event0x1               00A94FD0 P
_O_Pub_Obj_Inf15_Event0x2               00A95140 P
_O_Pub_Obj_Inf15_Event0x3               00A952B0 P
_O_Pub_Obj_Inf15_Event0x4               00A95590 P
_O_Pub_Obj_Inf15_Event0x5               00A95870 P
_O_Pub_Obj_Inf15_Event0x6               00A95B50 P
_O_Pub_Obj_Inf15_Event0x7               00A95C70 P
_O_Pub_Obj_Inf15_Event0x8               00A95DE0 P
_O_Pub_Obj_Inf15_Event0x9               00A95F50 P
_O_Pub_Obj_Inf15_Event0xA               00A960C0 P
_O_Pub_Obj_Inf15_Event0xB               00A966B0 P
_O_Pub_Obj_Inf16_Event0x1               00A96A40 P
_O_Pub_Obj_Inf16_Event0x2               00A96D60 P
_O_Pub_Obj_Inf16_Event0x3               00A97140 P
_O_Pub_Obj_Inf17_Event0x1               00A97440 P
_O_Pub_Obj_Inf18_Method0x1              00A975B0 P
_O_Pub_Obj_Inf18_Event0x2               00A97720 P
_O_Pub_Obj_Inf18_Event0x3               00A97890 P
_O_Pub_Obj_Inf18_Event0x4               00A97C20 P
_O_Pub_Obj_Inf19_Event0x1               00A97D90 P
_O_Pub_Obj_Inf1A_Event0x1               00A97F00 P
saveOldDisplayOptions                   00A98070
unkRestoreResolution                    00A98150
applyFullscreenResolution               00A98190
saveDisplayOptions                      00A984A0
_O_Pub_Obj_Inf1D_Event0x1               00A9A1E0 P
_O_Pub_Obj_Inf1D_Event0x2               00A9A430 P
_O_Pub_Obj_Inf1D_Event0x3               00A9A4C0 P
_O_Pub_Obj_Inf1E_Event0x1               00A9A680 P
_O_Pub_Obj_Inf1E_Event0x2               00A9A760 P
_O_Pub_Obj_Inf1E_Event0x3               00A9A840 P
_O_Pub_Obj_Inf1E_Event0x4               00A9A920 P
_O_Pub_Obj_Inf1E_Event0x5               00A9AA00 P
_O_Pub_Obj_Inf1E_Event0x6               00A9AAE0 P
_O_Pub_Obj_Inf1E_Event0x7               00A9AB70 P
_O_Pub_Obj_Inf1E_Event0x8               00A9AC00 P
_O_Pub_Obj_Inf1E_Event0x9               00A9ACC0 P
_O_Pub_Obj_Inf1E_Event0xA               00A9AD80 P
_O_Pub_Obj_Inf1E_Event0xB               00A9AE40 P
_O_Pub_Obj_Inf1E_Event0xC               00A9AF00 P
_O_Pub_Obj_Inf1E_Event0xD               00A9B850 P
_O_Pub_Obj_Inf1E_Event0xE               00A9BCF0 P
_O_Pub_Obj_Inf1E_Event0xF               00A9C190 P
_O_Pub_Obj_Inf1E_Event0x10              00A9C980 P
_O_Pub_Obj_Inf1E_Event0x11              00A9D170 P
_O_Pub_Obj_Inf1E_Event0x12              00A9D200 P
_O_Pub_Obj_Inf1E_Event0x13              00A9D2C0 P
_O_Pub_Obj_Inf1E_Event0x14              00A9D300 P
_O_Pub_Obj_Inf1E_Event0x15              00A9D390 P
_O_Pub_Obj_Inf1E_Event0x16              00A9D420 P
_O_Pub_Obj_Inf1E_Event0x17              00A9D4E0 P
_O_Pub_Obj_Inf1E_Event0x18              00A9D570 P
_O_Pub_Obj_Inf1E_Event0x19              00A9D590 P
_O_Pub_Obj_Inf1E_Event0x1A              00A9D620 P
_O_Pub_Obj_Inf1E_Event0x1B              00A9D6E0 P
_O_Pub_Obj_Inf1E_Event0x1C              00A9D7A0 P
_O_Pub_Obj_Inf1E_Event0x1D              00A9D830 P
_O_Pub_Obj_Inf1E_Event0x1E              00A9D8F0 P
_O_Pub_Obj_Inf1E_Event0x1F              00A9D9B0 P
_O_Pub_Obj_Inf1E_Event0x20              00A9DA40 P
_O_Pub_Obj_Inf1E_Event0x21              00A9DAD0 P
_O_Pub_Obj_Inf1E_Event0x22              00A9DB90 P
_O_Pub_Obj_Inf1F_Method0x1              00A9E440 P
_O_Pub_Obj_Inf1F_Event0x2               00A9E7D0 P
_O_Pub_Obj_Inf1F_Event0x3               00A9E910 P
_O_Pub_Obj_Inf20_Event0x2               00A9E9A0 P
_O_Pub_Obj_Inf20_Event0x3               00A9F260 P
_O_Pub_Obj_Inf20_Event0x4               00A9FE00 P
_O_Pub_Obj_Inf20_Event0x5               00A9FF10 P
_O_Pub_Obj_Inf20_Event0x6               00A9FFA0 P
_O_Pub_Obj_Inf20_Event0x7               00AA0F20 P
_O_Pub_Obj_Inf20_Event0x8               00AA1250 P
_O_Pub_Obj_Inf20_Event0x1               00AA2160 P
TriggerEvent                            00AA42D0
_O_Pub_Obj_Inf22_Event0x2               00AA7F30 P
_O_Pub_Obj_Inf22_Event0x3               00AA8410 P
_O_Pub_Obj_Inf22_Event0x4               00AA87A0 P
_O_Pub_Obj_Inf22_Event0x5               00AA8B30 P
_O_Pub_Obj_Inf22_Event0x6               00AA8EC0 P
_O_Pub_Obj_Inf22_Event0x7               00AA9250 P
_O_Pub_Obj_Inf22_Event0x8               00AA96A0 P
_O_Pub_Obj_Inf22_Event0x9               00AA9AF0 P
_O_Pub_Obj_Inf22_Event0xA               00AA9E80 P
_O_Pub_Obj_Inf22_Event0xB               00AAA210 P
_O_Pub_Obj_Inf22_Event0xC               00AAA560 P
_O_Pub_Obj_Inf22_Event0xD               00AAA970 P
_O_Pub_Obj_Inf22_Event0xE               00AAADE0 P
_O_Pub_Obj_Inf22_Event0xF               00AAB770 P
_O_Pub_Obj_Inf22_Event0x10              00AABDD0 P
_O_Pub_Obj_Inf22_Event0x11              00AAD210 P
_O_Pub_Obj_Inf22_Event0x12              00AADD30 P
_O_Pub_Obj_Inf22_Event0x13              00AAE1F0 P
_O_Pub_Obj_Inf22_Event0x14              00AAE660 P
_O_Pub_Obj_Inf22_Event0x15              00AAECC0 P
_O_Pub_Obj_Inf22_Event0x16              00AAF5C0 P
_O_Pub_Obj_Inf22_Event0x17              00AAFA80 P
_O_Pub_Obj_Inf22_Event0x18              00AAFEF0 P
_O_Pub_Obj_Inf22_Event0x19              00AB03C0 P
_O_Pub_Obj_Inf22_Event0x1A              00AB0840 P
_O_Pub_Obj_Inf22_Event0x1B              00AB0BD0 P
_O_Pub_Obj_Inf22_Event0x1C              00AB0F60 P
_O_Pub_Obj_Inf22_Event0x1D              00AB12F0 P
_O_Pub_Obj_Inf22_Event0x1E              00AB1680 P
_O_Pub_Obj_Inf22_Event0x1F              00AB1D30 P
_O_Pub_Obj_Inf22_Event0x20              00AB1DC0 P
_O_Pub_Obj_Inf22_Event0x21              00AB2110 P
_O_Pub_Obj_Inf22_Event0x22              00AB2250 P
_O_Pub_Obj_Inf22_Event0x23              00AB28E0 P
_O_Pub_Obj_Inf22_Event0x24              00AB2D00 P
_O_Pub_Obj_Inf22_Event0x25              00AB3120 P
_O_Pub_Obj_Inf22_Event0x26              00AB3490 P
_O_Pub_Obj_Inf22_Event0x1               00AB4A70 P
_O_Pub_Obj_Inf22_Event0x27              00AB92B0 P
_O_Pub_Obj_Inf22_Event0x28              00AB96D0 P
_O_Pub_Obj_Inf23_Method0x1              00AB9AF0 P
_O_Pub_Obj_Inf24_Event0x1               00AB9B80 P
loadDefaultGraphics                     00ABA390
clearGraphics                           00ACB440
loadLocalGraphics                       00ACD220
unkLoadGraphics                         00AD3420
unkLoadLocalGraphics                    00AD9DC0
loadCustomWorldGraphics                 00ADFF90
loadDefaultWorldGraphics                00AE2630
_O_Pub_Obj_Inf26_Event0x2               00AE8B30 P
_O_Pub_Obj_Inf26_Event0x3               00AE8C70 P
_O_Pub_Obj_Inf26_Event0x4               00AE90D0 P
_O_Pub_Obj_Inf26_Event0x5               00AE91D0 P
_O_Pub_Obj_Inf26_Event0x1               00AE9240 P
_O_Pub_Obj_Inf26_Event0x6               00AEA740 P
_O_Pub_Obj_Inf27_Event0x2               00AEAB70 P
_O_Pub_Obj_Inf27_Event0x3               00AEAC00 P
_O_Pub_Obj_Inf27_Event0x4               00AEACF0 P
_O_Pub_Obj_Inf27_Event0x5               00AEAE60 P
_O_Pub_Obj_Inf27_Method0x1              00AEB180 P
_O_Pub_Obj_Inf27_Event0x6               00AEB9E0 P
_O_Pub_Obj_Inf27_Event0x7               00AEBB50 P
_O_Pub_Obj_Inf28_Event0x1               00AEC010 P
_O_Pub_Obj_Inf28_Event0x2               00AEC1C0 P
_O_Pub_Obj_Inf28_Event0x3               00AEC250 P
_O_Pub_Obj_Inf28_Event0x4               00AEC530 P
_O_Pub_Obj_Inf29_Method0x1              00AEC690 P
_O_Pub_Obj_Inf29_Event0x2               00AEC970 P
_O_Pub_Obj_Inf29_Event0x3               00AECAE0 P
_O_Pub_Obj_Inf29_Event0x4               00AECD20 P
_O_Pub_Obj_Inf2A_Method0x1              00AECE90 P
_O_Pub_Obj_Inf2B_Event0x19              00AED060 P
_O_Pub_Obj_Inf2B_Event0x1A              00AED320 P
_O_Pub_Obj_Inf2B_Event0x1B              00AEDAC0 P
_O_Pub_Obj_Inf2B_Event0x1C              00AEE490 P
_O_Pub_Obj_Inf2B_Event0x1D              00AEE8B0 P
_O_Pub_Obj_Inf2B_Event0x1E              00AEEFE0 P
_O_Pub_Obj_Inf2B_Event0x1               00AEFA60 P
_O_Pub_Obj_Inf2B_Event0x2               00B0C0B0 P
_O_Pub_Obj_Inf2B_Event0x3               00B0C4B0 P
_O_Pub_Obj_Inf2B_Event0x4               00B0EB70 P
_O_Pub_Obj_Inf2B_Event0x5               00B0F2A0 P
_O_Pub_Obj_Inf2B_Event0x6               00B0F4B0 P
_O_Pub_Obj_Inf2B_Event0x7               00B0FB00 P
_O_Pub_Obj_Inf2B_Event0x8               00B0FEF0 P
_O_Pub_Obj_Inf2B_Event0x9               00B10250 P
_O_Pub_Obj_Inf2B_Event0xA               00B10580 P
_O_Pub_Obj_Inf2B_Event0xB               00B10890 P
_O_Pub_Obj_Inf2B_Event0xC               00B113E0 P
_O_Pub_Obj_Inf2B_Event0xD               00B11740 P
_O_Pub_Obj_Inf2B_Event0xE               00B121B0 P
_O_Pub_Obj_Inf2B_Event0xF               00B12420 P
_O_Pub_Obj_Inf2B_Event0x10              00B12690 P
_O_Pub_Obj_Inf2B_Event0x11              00B137F0 P
_O_Pub_Obj_Inf2B_Event0x12              00B13980 P
_O_Pub_Obj_Inf2B_Event0x13              00B145C0 P
_O_Pub_Obj_Inf2B_Event0x14              00B14980 P
_O_Pub_Obj_Inf2B_Event0x15              00B15680 P
_O_Pub_Obj_Inf2B_Event0x16              00B15B20 P
_O_Pub_Obj_Inf2B_Event0x1F              00B17480 P
_O_Pub_Obj_Inf2B_Event0x20              00B17890 P
_O_Pub_Obj_Inf2B_Event0x21              00B17970 P
_O_Pub_Obj_Inf2B_Event0x22              00B17AA0 P
_O_Pub_Obj_Inf2B_Event0x23              00B18270 P
_O_Pub_Obj_Inf2B_Event0x24              00B18500 P
_O_Pub_Obj_Inf2B_Event0x25              00B18980 P
_O_Pub_Obj_Inf2B_Event0x17              00B1A060 P
_O_Pub_Obj_Inf2B_Event0x18              00B1AD90 P
_O_Pub_Obj_Inf2B_Event0x26              00B1B430 P
_O_Pub_Obj_Inf2C_Event0x1               00B1B6F0 P
_O_Pub_Obj_Inf2C_Event0x2               00B1B960 P
_O_Pub_Obj_Inf2C_Event0x3               00B1D740 P
_O_Pub_Obj_Inf2C_Event0x4               00B1D820 P
_O_Pub_Obj_Inf2C_Event0x5               00B1D9A0 P
_O_Pub_Obj_Inf2C_Event0x6               00B1DA30 P
_O_Pub_Obj_Inf2C_Event0x7               00B1DC20 P
_O_Pub_Obj_Inf2C_Event0x8               00B1DD90 P
_O_Pub_Obj_Inf2C_Event0x9               00B1DE70 P
_O_Pub_Obj_Inf2D_Method0x1              00B1E510 P
_O_Pub_Obj_Inf2D_Event0x2               00B1E5A0 P
_O_Pub_Obj_Inf2E_Event0x1               00B1F790 P
_O_Pub_Obj_Inf2E_Event0x2               00B1FAE0 P
_O_Pub_Obj_Inf2E_Event0x3_tmrLoad_Timer 00B1FDF0 P
_O_Pub_Obj_Inf2F_Event0x1               00B20000 P
_O_Pub_Obj_Inf2F_Event0x2               00B20080 P
_O_Pub_Obj_Inf2F_Event0x3               00B200F0 P
_O_Pub_Obj_Inf2F_Event0x4               00B20160 P
_O_Pub_Obj_Inf2F_Event0x5               00B20230 P
saveNPCDefaults                         00B202B0
loadDefaultNPCConfiguration             00B20870
loadNPCConfigurations                   00B20E50
loadNPCConfiguration                    00B21200
_O_Pub_Obj_Inf31_Event0x1               00B22900 P
_O_Pub_Obj_Inf31_Event0x3               00B229C0 P
_O_Pub_Obj_Inf31_Event0x4               00B22B20 P
_O_Pub_Obj_Inf31_Event0x5               00B22DB0 P
_O_Pub_Obj_Inf31_Event0x2               00B22E90 P
_O_Pub_Obj_Inf31_Event0x6               00B23F40 P

*/

//DEBUG:
#define dbgbox(msg) MessageBoxW(NULL, msg, L"Dbg", NULL);
#define dbgboxA(msg) MessageBoxA(NULL, msg, "Dbg", NULL);

#endif


