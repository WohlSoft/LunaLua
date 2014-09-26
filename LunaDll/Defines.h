//*** Defines.h - It has defines in it, also basically the known RAM map. ***
#pragma once

#include <windows.h>

#define LUNA_VERSION		8

;
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
	CMPT_LESS = 2
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

// General
#define GM_SCRN_HDC			*(DWORD*)0x00B25028

// Frame counters
#define GM_TRANS_FRAMECT	*(DWORD*)0x00B2C670
//#define GM_ACTIVE_FRAMECT	*(DWORD*)0x00B2C67C  Float?

// MOB Related memory
#define GM_NPCS_PTR			*(DWORD*)0x00B259E8	// +0xAD58 + 0x20  to NPCs
#define GM_NPCS_COUNT		*(WORD*)0x00B2595A 
#define GM_PLAYERS_PTR		*(DWORD*)0x00B25A20

// HUD stuff
#define GM_STAR_COUNT		*(DWORD*)0x00B251E0
#define GM_COINS			*(WORD*)0x00B2C5A8
#define GM_PLAYER_LIVES		*(FLOAT*)0x00B2C5AC

// States
#define GM_FREEZWITCH_ACTIV *(WORD*)0x00B2C8B4
#define GM_PAUSE_OPEN		*(WORD*)0x00B250E2

// Camera
#define GM_CAMERA_X			*(DWORD*)0x00B2B984
#define GM_CAMERA_Y			*(DWORD*)0x00B2B9A0

// Level related memory
#define GM_LVL_BOUNDARIES	*(DWORD*)0x00B257D4		// 6 doubles each section, L/U/R/D/?/?
#define GM_LVLNAME_PTR		*(DWORD*)0x00B2C5A4	
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
#define GM_CHEATED			*(WORD*)0x00B2C8C4		// 0xFFFF = cheated


/////////////////////
///  -Functions-  ///
/////////////////////

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