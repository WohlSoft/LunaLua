#include "NPCs.h"
#include "../Misc/MiscFuncs.h"
#include <list>

// GET
NPCMOB* NPC::Get(int index) {
	if(index < 0 || index > GM_NPCS_COUNT)
		return 0;
		
	return (NPCMOB*)((GM_NPCS_PTR + 0x0000AD58) + (0x00000158 * index));
}

// GET FIRST MATCH
NPCMOB* NPC::GetFirstMatch(int ID, int section) {
	bool anyID = (ID == -1 ? true : false);
	bool anySec = (section == -1 ? true : false);
	NPCMOB* thisnpc = NULL;

	for(int i = 0; i < GM_NPCS_COUNT; i++) {
		thisnpc = Get(i);
		if(thisnpc->Identity == ID || anyID) {
			if(GetSection(thisnpc) == section || anySec) {
				return thisnpc; //matched
			}
		}
	}
	return NULL; //not matched
}

// FIND ALL
void NPC::FindAll(int ID, int section, std::list<NPCMOB*>* return_list) {
	bool anyID = (ID == -1 ? true : false);
	bool anySec = (section == -1 ? true : false);
	NPCMOB* thisnpc = NULL;	

	for(int i = 0; i < GM_NPCS_COUNT; i++) {
		thisnpc = Get(i);
		if(thisnpc->Identity == ID || anyID) {
			if(GetSection(thisnpc) == section || anySec) {
				return_list->push_back(thisnpc);
			}
		}
	}
}

// SET HITS
void NPC::SetHits(NPCMOB* npc, float hits) {
	float* ptr=(float*)((&(*(byte*)npc)) + 0x148);
	*ptr = hits;
}

// GET SECTION
short NPC::GetSection(NPCMOB* npc) {
	return *((short*)((&(*(byte*)npc)) + 0x146));
}

// FACE DIRECTION
void NPC::FaceDirection(NPCMOB* npc, float direction) {
	npc->FacingDirection = direction;
}

// MEM SET
void NPC::MemSet(int ID, int offset, double value, OPTYPE operation, FIELDTYPE ftype) {
	char* dbg =  "MemSetDbg";
	if(ftype == FT_INVALID || offset > 0x15C)
		return;
	bool anyID = (ID == -1 ? true : false);
	NPCMOB* thisnpc;

	for(int i = 0; i < GM_NPCS_COUNT; i++) {
		thisnpc = Get(i);
		if(anyID || thisnpc->Identity == ID) {
			void* ptr = ((&(*(byte*)thisnpc)) + offset);
			MemAssign((int)ptr, value, operation, ftype);
		}
	}//for
}

// ALL SET HITS
void NPC::AllSetHits(int identity, int section, float hits) {
	bool anyID = (identity == -1 ? true : false);
	bool anySec = (section == -1 ? true : false);
	NPCMOB* thisnpc;

	for(int i = 0; i < GM_NPCS_COUNT; i++) {
		thisnpc = Get(i);
		if(anyID || thisnpc->Identity == identity) {
			if(anySec || GetSection(thisnpc) == section) {
				SetHits(thisnpc, hits);
			}
		}
	}	
}

// ALL FACE
void NPC::AllFace(int identity, int section, double x) {
	bool anyID = (identity == -1 ? true : false);
	bool anySec = (section == -1 ? true : false);
	NPCMOB* thisnpc;
	for(int i = 0; i < GM_NPCS_COUNT; i++) {
		thisnpc = Get(i);
		if(anyID || thisnpc->Identity == identity) {
			if(anySec || GetSection(thisnpc) == section) {
				if(x < thisnpc->Xpos) {
					FaceDirection(thisnpc, -1);
				} else {
					FaceDirection(thisnpc, 1);
				}
			}
		}
	}	
}
