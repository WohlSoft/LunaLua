//*** Defines.h - It has defines in it, also basically the known RAM map. ***
#ifndef Defines_hhhh
#define Defines_hhhh

#include <windows.h>

#define LUNA_VERSION		8

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

#define GM_BASE				0x00B25000
#define GM_END				0x00B2E000

#define GM_FILE_START		0x00400000
#define FN_OFFSET2ADDR(offs) (void*)(GM_FILE_START + (DWORD)offs)

#define GM_PLAYER_KEY_UP	0
#define GM_PLAYER_KEY_DOWN	1
#define GM_PLAYER_KEY_LEFT	2
#define GM_PLAYER_KEY_RIGHT	3
#define GM_PLAYER_KEY_JUMP	4
#define GM_PLAYER_KEY_SJUMP	5
#define GM_PLAYER_KEY_X		6
#define GM_PLAYER_KEY_RUN	7
#define GM_PLAYER_KEY_SEL	8
#define GM_PLAYER_KEY_STR	9


// General
#define GM_SCRN_HDC			*(DWORD*)0x00B25028
#define GM_MODE_INTRO		*(WORD*)0x00B2C89C

// NPC Settings
#define GM_CONF_WIDTH		*(DWORD*)0x00B25BA8
#define GM_CONF_HEIGHT		*(DWORD*)0x00B25BC4
#define GM_CONF_GFXWIDTH	*(DWORD*)0x00B25BE0
#define GM_CONF_GFXHEIGHT	*(DWORD*)0x00B25BFC

// Frame counters
#define GM_TRANS_FRAMECT	*(DWORD*)0x00B2C670
//#define GM_ACTIVE_FRAMECT	*(DWORD*)0x00B2C67C  Float?

// MOB Related memory
#define GM_NPCS_PTR			*(DWORD*)0x00B259E8	// +0xAD58 + 0x20  to NPCs
#define GM_NPCS_COUNT		*(WORD*)0x00B2595A 
#define GM_PLAYERS_PTR		*(DWORD*)0x00B25A20
#define GM_PLAYERS_COUNT    *(WORD*)0x00B2595E
#define GM_EDIT_PLAYERS_PTR *(DWORD*)0x00CF74D8 // Editor Template player

// HUD stuff
#define GM_STAR_COUNT		*(DWORD*)0x00B251E0
#define GM_COINS			*(WORD*)0x00B2C5A8
#define GM_PLAYER_LIVES		*(FLOAT*)0x00B2C5AC

// States
#define GM_FREEZWITCH_ACTIV *(WORD*)0x00B2C8B4
#define GM_PAUSE_OPEN		*(WORD*)0x00B250E2
#define GM_CUR_MENUCHOICE   *(WORD*)0x00B2C880      // Current menu choice

// Camera
#define GM_CAMERA_X			*(DWORD*)0x00B2B984
#define GM_CAMERA_Y			*(DWORD*)0x00B2B9A0

// Overworld base struct
#define GM_OVERWORLD_PTR	*(DWORD*)0x00B2C5C8

// Overworld Level Array
#define GM_LEVEL_COUNT		*(DWORD*)0x00B25960
#define GM_LEVEL_BASE		*(DWORD*)0x00B25994


// Level related memory
#define GM_LVL_BOUNDARIES	*(DWORD*)0x00B257D4		// 6 doubles each section, L/U/R/D/?/?
#define GM_LVLFILENAME_PTR	*(DWORD*)0x00B2C5A4		// Lvl filename
#define GM_LVLNAME_PTR		*(DWORD*)0x00B2D764
#define GM_FULLPATH			*(DWORD*)0x00B2C618		// Full path to current .lvl file
#define GM_FULLDIR			*(DWORD*)0x00B2C61C		// Full path to current world dir
#define GM_CUR_LVL			*(WORD*)0x00B2C6D8

// Layers
#define GM_LAYER_UNK_PTR	*(DWORD*)0x00B259E8
#define GM_LAYER_EX_PTR		*(DWORD*)0x00B259B0
#define GM_LAYER_ARRAY_PTR	*(DWORD*)0x00B2C6B0

// Events
#define GM_EVENTS_PTR		*(DWORD*)0x00B2C6CC
#define GM_EVENT_TIMES_PTR	*(DWORD*)0x00B2D104		// array of 100 shorts
#define GM_EVENT_COUNT		*(WORD*)0x00B2D710

// Blocks
#define GM_BLOCK_COUNT		*(WORD*)0x00B25956
#define GM_BLOCKS_PTR		*(DWORD*)0x00B25A04

// Backgrounds
#define GM_SEC_BG_ARRAY		*(DOUBLE*)0x00B258B8
#define GM_BG_XPOS_PTR		*(DOUBLE*)0x00B2B984

// Animations
#define GM_ANIM_COUNT		*(WORD*)0x00B2595C
#define GM_ANIM_PTR			*(DWORD*)0x00B259CC

// Sound
#define GM_MUSIC_PATHS_PTR	*(DWORD*)0x00B257B8 
#define GM_SEC_MUSIC_TBL_P	*(DWORD*)0x00B25828		// 21 shorts containing music # for each section

// Input
#define GM_VKEY_TABLE_PTR	*(DWORD*)0x00B25068 
#define GM_INPUTSTR_BUF_PTR	*(DWORD*)0x00B2C898

// Saves
#define GM_CUR_SAVE_SLOT	*(WORD*)0x00B2C62A		// 1 2 or 3

// Cheats
#define GM_PLAYER_INVULN	*(WORD*)0x00B2C8C0		// 0xFFFF = invuln
#define GM_PLAYER_INFJUMP   *(WORD*)0x00B2C8AC      // 0xFFFF = infinite jumps
#define GM_CHEATED			*(WORD*)0x00B2C8C4		// 0xFFFF = cheated

// Miscs
#define GM_GRAVITY          *(WORD*)0x00B2C6F4      // 12 = default, 0 = non-falling
#define GM_JUMPHIGHT        *(WORD*)0x00B2C6DC      // 20 = default, higher number = higher jumps
#define GM_JUMPHIGHT_BOUNCE *(WORD*)0x00B2C6E2      // Same as normal jumphight, but from NPC
#define GM_EARTHQUAKE       *(WORD*)0x00B250AC      // 0 = default, higher number = bigger shaking, slowly resets to 0

#define GM_MARIO_VS_LUIGI_T *(WORD*)0x00B2D760      // 0 = default, if higher than 0 then display text "Mario VS Luigi"
#define GM_WINS_T           *(WORD*)0x00B2D762      // 0 = default, if higher than 0 then display text "WINS!"

#define GM_WINNING          *(WORD*)0x00B2C59E      // 0 = not winning, if higher than 0 then winning by this win-type
#define GM_WORLD_MODE       *(WORD*)0x00B2C5B4      // 0xFFFF = leave current level
#define GM_INTRO_MODE       *(WORD*)0x00B2C620

#define GM_UNK_OV_DATABLOCK *(DWORD*)0x00B25164     // Pointer to some kind of overworld data block involving locked character selection (not 100% sure)

//Hitbox
#define GM_HITBOX_H_PTR     *(WORD*)0x00B2C6FC      // player hitbox height for each character/power-up state (starts with small mario through small link, then cycles same way through each power up)
#define GM_HITBOX_H_D_PTR   *(WORD*)0x00B2C742      // hitbox heights while ducking
#define GM_HITBOX_W_PTR     *(WORD*)0x00B2C788      // hitbox widths

//Startup Config:
#define GM_ISLEVELEDITORMODE	*(WORD*)0x00B25134
#define GM_ISGAME				*(WORD*)0x00B25046
#define GM_NOSOUND				*(WORD*)0x00B2D734
#define GM_FRAMESKIP			*(WORD*)0x00B2C684

/////////////////////
///  -Assembly-   ///
/////////////////////

//VASM = value is assembly code
//npcToCoins ending Animation Settings
#define VASM_END_ANIM       *(BYTE*)0x00A3C86E      // = 11
#define VASM_END_COINSOUND  *(BYTE*)0x00A3C87F      // = 14
#define VASM_END_COINVAL    *(BYTE*)0x00A3C891      // = 1


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

////////////////////////
///    -Imports-     ///
////////////////////////

#define IMP_vbaStrCmp		*(DWORD*)0x004010F8
#define IMP_vbaStrCopy		*(DWORD*)0x004011b0
#define IMP_vbaFreeStr		*(DWORD*)0x00401248

////////////////////////
///    -Functions-   ///
////////////////////////

// Print using game's text function
#define GF_PRINT			0x00951F50			// Arg4 = float* y, Arg3 = float* x, Arg2 = Int* fonttype, Arg1 = WCSTR* string

// Start kill event for a player
#define GF_KILL_PLAYER		0x009B66D0			// Arg1 = int* to Index of player

#define GF_HARM_PLAYER		0x009B51E0			// Arg1 = int* Index of player

#define GF_INIT_NPC			0x00A03630			// Arg1 = int* Sprite ID, Arg2 = int* Unk flags, Arg3 = int* Unk

#define GF_UPDATE_NPC		0x00A3B680			// Arg1 = int* Index of NPC in NPC list

#define GF_PLAY_MUSIC		0x00A61B40			// Arg1 = int* Index of section containing music settings to play now

#define GF_PLAY_SFX			0x00A73FD0			// Arg1 = int* SoundIndex

#define GF_MOB_BLOCK_COL	0x00994250			// Arg1 = POS* structure of player, POS* structure of block
												// 1=Collision from top, 2=From right, 3=From bottom, 4=From left, 5=?

#define GF_IS_ON_CAMERA		0x00993DE0			// Arg1 = int* (1)  Arg2 = POS* structure

#define GF_TRIGGER_EVENT	0x00AA42D0			// Arg1 = wchar_t** name of event to start, Arg2 = Int* Unknown (0 or -1)

#define GF_BITBLT_CALL		0x004242D0			// Arg1

#define GF_NPC_KILL			0x00A315A0			// Arg1 = int* Killed NPC index  Arg2 = NPCMOB* Killer NPC

#define GF_NPC_TO_COINS		0x00A3C580			// No Args, Does convert every NPC to coins (Like player would win)

#define GF_RUN_ANIM			0x009E7380			// Arg1 = int* Animation ID, Arg2 = coorStruct* Struct pointer to x,y, Arg3 = int* some sort of adress, Arg4 = int* Unknown (mostly 0), Arg5 = int* Unknown (mostly 0)

#endif


//DEBUG:
#define dbgbox(msg) MessageBoxW(NULL, msg, L"Dbg", NULL);