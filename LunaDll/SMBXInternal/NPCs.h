//*** NPCs.h - Definition of known NPC structure and NPC-related framework functions ***
#ifndef NPCs_hhhh
#define NPCs_hhhh

#include "../Defines.h"
#include "../Misc/VB6StrPtr.h"
#include <list>
#include <vector>
#include <string>

// -- NPC structure -- ( 0x158 bytes )
// 0x+00	pt	= wchar_t* Attached layer name
// 0x+04	w	= Unknown
// 0x+06	w	= Unknown decrementing timer

// +0x2C	pt  = wchar_t* Activate event layer name
// +0x30	pt	= wchar_t* Death event layer name
// +0x34	pt	= wchar_t* Talk event name?
// +0x38	pt	= wchar_t* No More Objs event layer name
// +0x3C	pt	= wchar_t* Layer name

// 0x+40	w	= Unknown (some sort of hide value if -1)
// 0x+44	w	= Activated / interacted with player flag
// 0x+46	w	= Friendly (on = 0xFFFF)
// 0x+48	w	= Don't Move (on = 0xFFFF)

// 0x+4C	pt	= wchar_t* NPC Message text

// 0x+64	w	= Is a generator
// 0x+68	f	= Generator delay setting
// 0x+6A	w	- Generator firing rate
// 0x+6C	f	= Generator delay countdown
// 0x+70	w	= Direction to generate NPC?
// 0x+72	w	= Which layer to spawn NPC on
// 0x+74	w	= Invalidity or offscreen flag?

// 0x+78	qw	= X position
// 0x+80	qw	= Y position
// 0x+88	qw	= w/h?
// 0x+90	qw	= h/w?
// 0x+98	qw	= X speed
// 0x+A0	qw	= Y speed
// 0x+AC	dw	= Spawn X
// 0x+B4	dw	= Spawn Y

// 0x+E2	w	= Sprite GFX index/Identity   // x011C = lakitu
// 0x+E4	w	= Animation frame

// 0x+E8	f	= Animation timer

// 0x+F4	?	= Related to lakitu throw identity?

// 0x+FC	w	= Grabbable gun projectile timer

// 0x+110	f	= Lakitu throw timer

// 0x+118	f	= Direction faced
// 0x+122	w	= Kill/Kill effect (0 = No kill/ 1-? Kill effect with kill)
// 0x+124	w	= Unknown (same effect as 0x+128; -1 = offscreen)
// 0x+128	dw	= Ofscreen flag (0xFFFFFFFF = offscreen)
// 0x+12A	w	= Ofscreen countdown timer?
// 0x+12C	w	= Unknown grabbing-related
// 0x+12E	w	= 

// 0x+136	w	= FFFF on spawn
//
// 0x+146	w	= Current section this NPC is on
// 0x+148	f	= Hit count

#pragma pack(push, 4)
struct NPCMOB {
	VB6StrPtr attachedLayerName;            //+0x00
    short unknown_04;                       //+0x04
    short unknown_06;                       //+0x06 Unknown decrementing Timer
    short unknown_08;                       //+0x08 
    short collidesBelow;                    //+0x0A
    short collidesLeft;                     //+0x0C
    short collidesAbove;                    //+0x0E
    short collidesRight;                    //+0x10
    short unknown_12;                       //+0x12
    short unknown_14;                       //+0x14
    short unknown_16;                       //+0x16
    float unknown_18;                       //+0x18 NPC x-axis movement related, maybe temporary?
    short unknown_1C;                       //+0x1C
    short unknown_1E;                       //+0x1E
    short unknown_20;                       //+0x20
    short unknown_22;                       //+0x22
    short unknown_24;                       //+0x24
    short invincibilityToSword;             //+0x26
    short unknown_28;                       //+0x28
    short unknown_2A;                       //+0x2A
	VB6StrPtr activateEventLayerName;       //+0x2C
	VB6StrPtr deathEventName;               //+0x30
	VB6StrPtr talkEventName;                //+0x34
	VB6StrPtr noMoreObjInLayerEventName;    //+0x38
	VB6StrPtr layerName;                    //+0x3C
    short unknown_40;                       //+0x40
    short legacyBoss;                       //+0x42
    short activated;                        //+0x44
    short friendly;                         //+0x46
    short dontMove;                         //+0x48
    short unknown_4A;                       //+0x4A
	VB6StrPtr talkMsg;                      //+0x4C
    short unknown_50;                       //+0x50
    short unknown_52;                       //+0x52
    short unknown_54;                       //+0x54
    short unknown_56;                       //+0x56
    short unknown_58;                       //+0x58
    short unknown_5A;                       //+0x5A
    short unknown_5C;                       //+0x5C
    short unknown_5E;                       //+0x5E
    short unknown_60;                       //+0x60
    short unknown_62;                       //+0x62
    short isGenerator;                      //+0x64
    short unknown_66;                       //+0x66
    short generatorDelaySetting;            //+0x68  NOTE: In the wiki we have float noted, but it does not match with the 4-Byte size! Needs more investigating.
    short generatorFiringRate;              //+0x6A
    float generatorDelayCountdown;          //+0x6C
    short directionToGenerate;              //+0x70
    short layerToSpawn;                     //+0x72  NOTE: Missing information, is it an index or a ptr to text?
    short offscreenFlag;                    //+0x74
    short unknown_76;                       //+0x76
    Momentum momentum;                      //+0x78
    double spawnX;                          //+0xA8
    double spawnY;                          //+0xB0
    double gfxHeight;                       //+0xB8
    double gfxWidth;                        //+0xC0
    short unknown_C8;                       //+0xC8
    short unknown_CA;                       //+0xCA
    short unknown_CC;                       //+0xCC
    short unknown_CE;                       //+0xCE
    short unknown_D0;                       //+0xD0
    short unknown_D2;                       //+0xD2
    short unknown_D4;                       //+0xD4
    short unknown_D6;                       //+0xD6
    short unknown_D8;                       //+0xD8
    short unknown_DA;                       //+0xDA
    short respawnID;                        //+0xDC
    short unknown_DE;                       //+0xDE
    short unknown_E0;                       //+0xE0
    short id;                               //+0xE2
    short animationFrame;                   //+0xE4
    short unknown_E6;                       //+0xE6
    float animationTimer;                   //+0xE8
    float directionFaced;                   //+0xEC
    double ai1;                             //+0xF0
    double ai2;                             //+0xF8
    double ai3;                             //+0x100
    double ai4;                             //+0x108
    double ai5;                             //+0x110
    float directionFaced2;                  //+0x118
    short unknown_11C;                      //+0x11C
    short unknown_11E;                      //+0x11E
    short bounceOffBlock;                   //+0x120
    short killFlag;                         //+0x122
    short unknown_124;                      //+0x124
    short unknown_126;                      //+0x126
    short offscreenFlag2;                   //+0x128  NOTE: In the wiki we have double noted, but it does not match with the 8-Byte size!
    short offscreenCountdownTimer;          //+0x12A
    short unknown_12C;                      //+0x12C
    short unknown_12E;                      //+0x12E
    short unknown_130;                      //+0x130
    short unknown_132;                      //+0x132
    short unknown_134;                      //+0x134
    short collidesWithNPC;                  //+0x136
    short containedWithin;                  //+0x138
    short unknown_13A;                      //+0x13A
    short unknown_13C;                      //+0x13C
    short unknown_13E;                      //+0x13E
    short unknown_140;                      //+0x140
    short unknown_142;                      //+0x142
    short unknown_144;                      //+0x144
    short currentSection;                   //+0x146
    float hitCount;                         //+0x148
    short unknown_14C;                      //+0x14C
    short unknown_14E;                      //+0x14E
    short unknown_150;                      //+0x150
    short unknown_152;                      //+0x152
    short unknown_154;                      //+0x154
    short invincibilityFrames;              //+0x156
};
#pragma pack(pop)

/* Verify NPC struct is correctly sized */
static_assert(sizeof(NPCMOB) == 0x158, "sizeof(NPCMOB) must be 0x158");

namespace NPC {

	/// Player functions ///

	// NPC ACCESS
	NPCMOB* Get(int index); //Get ptr to an NPC
	NPCMOB* GetFirstMatch(int ID, int section); // Get first NPC found with the set ID and section, or 0 if no matches. -1 for ANY

	void FindAll(int target, int internal_section, std::list<NPCMOB*>* return_list);

	short GetSection(NPCMOB* npc);

	void MemSet(int ID, int offset, double value, OPTYPE operation, FIELDTYPE ftype); // ID -1 for ALL

	// STATES
	void SetHits(NPCMOB* npc, float hits); // Set hits of passed NPC
	void FaceDirection(NPCMOB* npc, float direction);

	// ITERATORS
	void AllSetHits(int identity, int section, float hits);		// Set all specified NPC hits
	void AllFace(int identity, int section, double x);	// All specified NPCs face the supplied x/y point
}

#endif

