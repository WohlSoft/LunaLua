#include "NPCs.h"
#include "../Misc/MiscFuncs.h"
#include <list>

// GET
NPCMOB* NPC::Get(int index) {
	if(index < 0 || index > GM_NPCS_COUNT)
		return 0;
		
    return &((NPCMOB*)GM_NPCS_PTR)[index + 129]; // +129 makes an offset of 0xAD58
}

NPCMOB * NPC::GetDummyNPC()
{
    return &((NPCMOB*)GM_NPCS_PTR)[128];
}

// GET FIRST MATCH
NPCMOB* NPC::GetFirstMatch(int ID, int section) {
	bool anyID = (ID == -1 ? true : false);
	bool anySec = (section == -1 ? true : false);
	NPCMOB* thisnpc = NULL;

	for(int i = 0; i < GM_NPCS_COUNT; i++) {
		thisnpc = Get(i);
		if(thisnpc->id == ID || anyID) {
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
		if(thisnpc->id == ID || anyID) {
			if(GetSection(thisnpc) == section || anySec) {
				return_list->push_back(thisnpc);
			}
		}
	}
}

// SET HITS
void NPC::SetHits(NPCMOB* npc, float hits) {
	npc->hitCount = hits;
}

// GET SECTION
short NPC::GetSection(NPCMOB* npc) {
	return npc->currentSection;
}

// FACE DIRECTION
void NPC::FaceDirection(NPCMOB* npc, float direction) {
	npc->directionFaced2 = direction; // The version at 0x118
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
		if(anyID || thisnpc->id == ID) {
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
        if (anyID || thisnpc->id == identity) {
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
        if (anyID || thisnpc->id == identity) {
			if(anySec || GetSection(thisnpc) == section) {
				if(x < thisnpc->momentum.x) {
					FaceDirection(thisnpc, -1);
				} else {
					FaceDirection(thisnpc, 1);
				}
			}
		}
	}	
}

static bool isSpinjumpSafe[NPC::MAX_ID] = { false };
static uint32_t vulnerableHarmTypes[NPC::MAX_ID] = { 0 };
void NPC::InitProperties() {
    for (int i = 1; i <= NPC::MAX_ID; i++)
    {
        SetVulnerableHarmTypes(i, 0);
        SetSpinjumpSafe(i, false);
    }

    SetSpinjumpSafe(0xB3, true);
    SetSpinjumpSafe(0x25, true);
    SetSpinjumpSafe(0xB4, true);
    SetSpinjumpSafe(0x26, true);
    SetSpinjumpSafe(0x2A, true);
    SetSpinjumpSafe(0x2B, true);
    SetSpinjumpSafe(0x2C, true);
    SetSpinjumpSafe(0x8, true);
    SetSpinjumpSafe(0xC, true);
    SetSpinjumpSafe(0x24, true);
    SetSpinjumpSafe(0x33, true);
    SetSpinjumpSafe(0x34, true);
    SetSpinjumpSafe(0x35, true);
    SetSpinjumpSafe(0x36, true);
    SetSpinjumpSafe(0x4A, true);
    SetSpinjumpSafe(0x5D, true);
    SetSpinjumpSafe(0xC8, true);
    SetSpinjumpSafe(0xCD, true);
    SetSpinjumpSafe(0xCF, true);
    SetSpinjumpSafe(0xC9, true);
    SetSpinjumpSafe(0xC7, true);
    SetSpinjumpSafe(0xF5, true);
    SetSpinjumpSafe(0x100, true);
    SetSpinjumpSafe(0x105, true);
    SetSpinjumpSafe(0x113, true);
    SetSpinjumpSafe(0x11D, true);
    SetSpinjumpSafe(0x11E, true);
    SetSpinjumpSafe(0x10E, true);
}

uint32_t NPC::GetVulnerableHarmTypes(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return 0;
    return vulnerableHarmTypes[id - 1];
}

void NPC::SetVulnerableHarmTypes(int id, uint32_t value) {
    if ((id < 1) || (id > NPC::MAX_ID)) return;
    vulnerableHarmTypes[id - 1] = value;
}

bool NPC::GetSpinjumpSafe(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return isSpinjumpSafe[id - 1];
}

void NPC::SetSpinjumpSafe(int id, bool value) {
    if ((id < 1) || (id > NPC::MAX_ID)) return;
    isSpinjumpSafe[id - 1] = value;
}
