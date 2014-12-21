//*** NPCs.h - Definition of known NPC structure and NPC-related framework functions ***
#pragma once
#include "Defines.h"
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

struct NPCMOB {
	char padding1[0x78];

	double Xpos;			// x78~
	double Ypos;			// x80~87

	char padding2[0x5A];

	short Identity;			// E2
	short AnimationFrame;	// E4

	short UnknownE6;		// E6

	float UnknownE8;		// E8

	float FacingDirection;	// EC
};

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
};
