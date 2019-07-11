#include "NPCs.h"
#include "../Misc/MiscFuncs.h"
#include <list>

// GET
NPCMOB* NPC::Get(int index) {
    if(index < 0 || index > GM_NPCS_COUNT)
        return nullptr;
        
    return &((NPCMOB*)GM_NPCS_PTR)[index + 129]; // +129 makes an offset of 0xAD58
}

// GET where 0 is the Dummy NPC
NPCMOB * NPC::GetRaw(int index)
{
    if (index < 0 || index > GM_NPCS_COUNT)
        return nullptr;

    return &((NPCMOB*)GM_NPCS_PTR)[index + 128];
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

// Declerations of inbuilt NPC property arrays
static uint32_t npcprop_vulnerableharmtypes[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_spinjumpsafe[NPC::MAX_ID+1] = { 0 };
static int16_t npcprop_nowaterphysics[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_harmlessgrab[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_harmlessthrown[NPC::MAX_ID + 1] = { 0 };
static int16_t npcprop_mountcolor[NPC::MAX_ID + 1] = { 0 };

// Initialization of inbuilt NPC property arrays
void NPC::InitProperties() {
    for (int i = 1; i <= NPC::MAX_ID; i++)
    {
        npcprop_vulnerableharmtypes[i] = 0;
        npcprop_spinjumpsafe[i] = 0;
        npcprop_nowaterphysics[i] = 0;
        npcprop_harmlessgrab[i] = 0;
        npcprop_harmlessthrown[i] = 0;
		npcprop_mountcolor[i] = 0;
	}

    // Set built-in spinjump safe IDs
    npcprop_spinjumpsafe[0xB3]  = -1;
    npcprop_spinjumpsafe[0x25]  = -1;
    npcprop_spinjumpsafe[0xB4]  = -1;
    npcprop_spinjumpsafe[0x26]  = -1;
    npcprop_spinjumpsafe[0x2A]  = -1;
    npcprop_spinjumpsafe[0x2B]  = -1;
    npcprop_spinjumpsafe[0x2C]  = -1;
    npcprop_spinjumpsafe[0x8]   = -1;
    npcprop_spinjumpsafe[0xC]   = -1;
    npcprop_spinjumpsafe[0x24]  = -1;
    npcprop_spinjumpsafe[0x33]  = -1;
    npcprop_spinjumpsafe[0x34]  = -1;
    npcprop_spinjumpsafe[0x35]  = -1;
    npcprop_spinjumpsafe[0x36]  = -1;
    npcprop_spinjumpsafe[0x4A]  = -1;
    npcprop_spinjumpsafe[0x5D]  = -1;
    npcprop_spinjumpsafe[0xC8]  = -1;
    npcprop_spinjumpsafe[0xCD]  = -1;
    npcprop_spinjumpsafe[0xCF]  = -1;
    npcprop_spinjumpsafe[0xC9]  = -1;
    npcprop_spinjumpsafe[0xC7]  = -1;
    npcprop_spinjumpsafe[0xF5]  = -1;
    npcprop_spinjumpsafe[0x100] = -1;
    npcprop_spinjumpsafe[0x105] = -1;
    npcprop_spinjumpsafe[0x113] = -1;
    npcprop_spinjumpsafe[0x11D] = -1;
    npcprop_spinjumpsafe[0x11E] = -1;
    npcprop_spinjumpsafe[0x10E] = -1;

    // Set built-in harmless grab IDs
    npcprop_harmlessgrab[0xC3] = -1;
    npcprop_harmlessgrab[0x16] = -1;
    npcprop_harmlessgrab[0x1A] = -1;
    npcprop_harmlessgrab[0x20] = -1;
    npcprop_harmlessgrab[0xEE] = -1;
    npcprop_harmlessgrab[0xEF] = -1;
    npcprop_harmlessgrab[0xC1] = -1;
    npcprop_harmlessgrab[0x23] = -1;
    npcprop_harmlessgrab[0xBF] = -1;
    npcprop_harmlessgrab[0x31] = -1;
    npcprop_harmlessgrab[0x86] = -1;
    npcprop_harmlessgrab[0x9A] = -1;
    npcprop_harmlessgrab[0x9B] = -1;
    npcprop_harmlessgrab[0x9C] = -1;
    npcprop_harmlessgrab[0x9D] = -1;
    npcprop_harmlessgrab[0x1F] = -1;
    npcprop_harmlessgrab[0xF0] = -1;
    npcprop_harmlessgrab[0x116] = -1;
    npcprop_harmlessgrab[0x117] = -1;
    npcprop_harmlessgrab[0x124] = -1;

	// Set built-in boot / yoshi colors
	npcprop_mountcolor[0x23] = 1;
	npcprop_mountcolor[0xBF] = 2;
	npcprop_mountcolor[0xC1] = 3;
	npcprop_mountcolor[0x5F] = 1;
	npcprop_mountcolor[0x62] = 2;
	npcprop_mountcolor[0x63] = 3;
	npcprop_mountcolor[0x64] = 4;
	npcprop_mountcolor[0x94] = 5;
	npcprop_mountcolor[0x95] = 6;
	npcprop_mountcolor[0x96] = 7;
	npcprop_mountcolor[0xE4] = 8;
}

// Internal C++ getters for inbuilt NPC property arrays
uint32_t NPC::GetVulnerableHarmTypes(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return 0;
    return npcprop_vulnerableharmtypes[id];
}

bool NPC::GetSpinjumpSafe(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_spinjumpsafe[id] != 0);
}

bool NPC::GetNoWaterPhysics(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_nowaterphysics[id] != 0);
}

bool NPC::GetHarmlessGrab(int id) {
    if ((id < 1) || (id > NPC::MAX_ID)) return false;
    return (npcprop_harmlessgrab[id] != 0);
}

bool NPC::GetHarmlessThrown(int id) {
	if ((id < 1) || (id > NPC::MAX_ID)) return false;
	return (npcprop_harmlessthrown[id] != 0);
}

int16_t NPC::GetMountColor(int id) {
	if ((id < 1) || (id > NPC::MAX_ID)) return 0;
	return (npcprop_mountcolor[id]);
}

// Getter for address of NPC property arrays
uintptr_t NPC::GetPropertyTableAddress(const std::string& s)
{
    if (s == "vulnerableharmtypes")
    {
        return reinterpret_cast<uintptr_t>(npcprop_vulnerableharmtypes);
    }
    else if (s == "spinjumpsafe")
    {
        return reinterpret_cast<uintptr_t>(npcprop_spinjumpsafe);
    }
    else if (s == "nowaterphysics")
    {
        return reinterpret_cast<uintptr_t>(npcprop_nowaterphysics);
    }
    else if (s == "harmlessgrab")
    {
        return reinterpret_cast<uintptr_t>(npcprop_harmlessgrab);
    }
	else if (s == "harmlessthrown")
	{
		return reinterpret_cast<uintptr_t>(npcprop_harmlessthrown);
	}
	else if (s == "mountcolor")
	{
		return reinterpret_cast<uintptr_t>(npcprop_mountcolor);
	}
    else
    {
        return 0;
    }
}
