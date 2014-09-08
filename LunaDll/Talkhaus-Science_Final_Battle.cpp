#include <vector>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "LevelCodes.h"
#include "PlayerMOB.h"
#include "Globals.h"
#include "NPCs.h"

#define NPC_FIREBAR 260
#define NPC_DOUGHNUT 210
#define NPC_SCIENCE 209

using namespace std;

namespace ScienceBattle
{
	NPCMOB* FindNPC(short identity);
	vector<NPCMOB*> FindAllNPC(short identity);
	bool TriggerBox(double x1, double y1, double x2, double y2);
	void HurtPlayer();

	bool init_doonce;
	NPCMOB* hurt_npc, *science_npc, *friendly_doughnut;
	vector<NPCMOB*> doughnuts;
	PlayerMOB* demo;
	int hurt_timer;
	int grace_timer;
	int throw_timer;

	void ScienceInitCode()
	{
		init_doonce = false;
	}

	void ScienceCode()
	{
		if (!init_doonce)
		{
			init_doonce		= true;
			hurt_timer		= 0;
			throw_timer		= 0;
			demo			= Player::Get(1);
		}

		hurt_npc = FindNPC(NPC_FIREBAR);
		
		if (hurt_npc == NULL)
			return;


		if (hurt_timer <= 0)
			hurt_npc ->Ypos = demo->CurYPos - 128;
		else
		{
			hurt_timer--;
			hurt_npc ->Ypos = demo->CurYPos;
		}
		hurt_npc ->Xpos = demo->CurXPos;

		doughnuts = FindAllNPC(NPC_DOUGHNUT);

		if (demo->HeldNPCIndex > 0)
			throw_timer = 30;

		//gLunaRender.SafePrint(std::wstring(L"ID: " + std::to_wstring(demo->HeldNPCIndex)), 3, 0, 256);

		
		if (grace_timer >= 0)
		{
			for each (NPCMOB* doughnut in doughnuts)
			{
				double x_diff, y_diff, m;

				x_diff = doughnut->Xpos - demo->CurXPos;
				y_diff = doughnut->Ypos - demo->CurYPos;
				m = sqrt(x_diff * x_diff + y_diff * y_diff);

				if (m == 0)
					continue;

				x_diff /= m;
				y_diff /= m;

				doughnut->Xpos += x_diff * 15;
				doughnut->Ypos += y_diff * 15;
			}
			grace_timer--;
		}
		else
		{
			if (throw_timer <= 0)
			{
				for each (NPCMOB* doughnut in doughnuts)
				{
					//Ignore generators
					if ((*((int*)doughnut + 16)) != 0)
						continue;
				
					double x1, x2, y1, y2;
			
					x1 = doughnut->Xpos + 28 * 0.42;
					y1 = doughnut->Ypos + 32 * 0.42;
					x2 = doughnut->Xpos + 28 * 0.57;
					y2 = doughnut->Ypos + 32 * 0.57;

					if (TriggerBox(x1, y1, x2, y2))
						HurtPlayer();
				}
			}
		}
		

		if (throw_timer > 0)
			throw_timer--;
	}

	NPCMOB* FindNPC(short identity)
	{
		NPCMOB* currentnpc = NULL;

		for(int i = 0; i < GM_NPCS_COUNT; i++)
		{
			currentnpc = NPC::Get(i);
			if (currentnpc->Identity == identity)
				return currentnpc;
		}

		return NULL;
	}

	vector<NPCMOB*> FindAllNPC(short identity)
	{
		vector<NPCMOB*> npcs_found = vector<NPCMOB*>();
		NPCMOB* currentnpc = NULL;

		for(int i = 0; i < GM_NPCS_COUNT; i++)
		{
			currentnpc = NPC::Get(i);
			if (currentnpc->Identity == identity)
				npcs_found.push_back(currentnpc);
		}

		return npcs_found;
	}

	void HurtPlayer()
	{
		hurt_timer = 3;
		grace_timer = 120;
	}

	bool TriggerBox(double x1, double y1, double x2, double y2)
	{
		return (demo->CurXPos + demo->Width		> x1 &&
				demo->CurXPos					< x2 &&
				demo->CurYPos + demo->Height	> y1 &&
				demo->CurYPos					< y2);
	}
}