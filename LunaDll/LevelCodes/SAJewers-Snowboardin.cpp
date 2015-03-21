#include <vector>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "LevelCodes.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../Globals.h"
#include "../SMBXInternal/NPCs.h"

#define NPC_SHELL 195

using namespace std;

namespace SAJSnowbordin
{
	NPCMOB* FindNPC(short identity);
	int combo_start;

	void SnowbordinInitCode()
	{
		combo_start = 0;
	}

	void SnowbordinCode()
	{
		NPCMOB *shell_npc;
		shell_npc = FindNPC(NPC_SHELL);
		
		if (shell_npc == NULL)
			return;

		int *shell_kills = (int*)((char*)(shell_npc) + 0x24);

		if (*shell_kills >= 9)
		{
			if (combo_start < 6)
				combo_start += 2;

			*shell_kills = combo_start;
		}

		//gLunaRender.SafePrint(std::wstring(L"KILLS: " + std::to_wstring(*shell_kills)), 3, 0, 256);

	}

	NPCMOB* FindNPC(short identity)
	{
		NPCMOB* currentnpc = NULL;

		for(int i = 0; i < GM_NPCS_COUNT; i++)
		{
			currentnpc = NPC::Get(i);
			if (currentnpc->id == identity)
				return currentnpc;
		}

		return NULL;
	}
}