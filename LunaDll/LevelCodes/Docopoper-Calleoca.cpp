#include <vector>
#include <stdlib.h>
#include <time.h>
#include "LevelCodes.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../Globals.h"
#include "../SMBXInternal/NPCs.h"

#define PI 3.1415926535897932384626433832795

#define NPC_SIGN 151
#define NPC_COIN 10
#define NPC_FIREBAR 260
#define NPC_GOAL 16

#define OFFSCREEN_DIST 400
#define MIDPOINT_X -194620
#define END_X -188792

using namespace std;

NPCMOB* FindNPC(short identity);
void HurtPlayer();
bool TriggerBox(double x1, double y1, double x2, double y2);
int AngleDifference(int angle1, int angle2);

bool init_doonce;

PlayerMOB* demo;

NPCMOB* calleoca_npc1 = NULL;
NPCMOB* calleoca_npc2 = NULL;
NPCMOB* hurt_npc = NULL;
NPCMOB* goal_npc = NULL;

int win_timer;
int freeze_timer = 0;
int phase = 0;
double calleoca_x, calleoca_y, storage_x, storage_y;

double thwomp_hspeed = 0;
double thwomp_vspeed = 0;
double thwomp_height, thwomp_bottom;

double missile_direction = 0;
double missile_hspeed = 0;
double missile_vspeed = 0;
double missile_top, missile_bottom;
int missile_fuel = 0;

double fishingboo_hspeed = 0;
double fishingboo_vspeed = 0;
double fishingboo_ferocity = 1;

template <typename T> T Clamp(const T& value, const T& low, const T& high) 
{
  return value < low ? low : (value > high ? high : value); 
}

void CalleocaInitCode()
{
	init_doonce = false;

	phase = 0;

	win_timer = 100;

	thwomp_hspeed = 0;
	thwomp_vspeed = 0;

	freeze_timer = 0;

	missile_direction = 0;
	missile_hspeed = 0;
	missile_vspeed = 0;

	fishingboo_hspeed = 0;
	fishingboo_vspeed = 0;
	fishingboo_ferocity = 1;
}

void Phase0()
{
	calleoca_npc1->animationFrame = 0;

    if (demo->momentum.x > calleoca_x + 128)
	{
		freeze_timer = 45;
		phase = 1;
	}

    if (demo->momentum.x >= MIDPOINT_X)
	{
		phase = 3;
		calleoca_x = MIDPOINT_X - 256;
		calleoca_y -= 256;
	}
}

/*************************************************************************************************/
//Standing there
void Phase1()
{
    thwomp_hspeed += (demo->momentum.x - calleoca_x) * 0.001;
	if (abs(thwomp_height - calleoca_y) < 8)
	{
		thwomp_hspeed = Clamp<double>(thwomp_hspeed, -9, 9);

		if (TriggerBox(calleoca_x, calleoca_y + 64, calleoca_x + 64, calleoca_y + 512))
			phase = 2;
	}
	else
		thwomp_hspeed = 0;

	thwomp_vspeed = (thwomp_height - calleoca_y) * 0.1;
	thwomp_vspeed = Clamp<double>(thwomp_vspeed, -6.0, 6.0);

	if (freeze_timer > 0)
	{
		thwomp_vspeed = 0;
		freeze_timer--;
	}
		

	calleoca_x += thwomp_hspeed;
	calleoca_y += thwomp_vspeed;

	calleoca_npc1->animationFrame = 2;
	if (TriggerBox(calleoca_x + 10, calleoca_y + 0, calleoca_x + 54, calleoca_y + 64))
		HurtPlayer();

    if (demo->momentum.x >= MIDPOINT_X)
	{
		freeze_timer = 150;
		phase = 3;
	}
}

/*************************************************************************************************/
//Thwomp rising / moving
void Phase2()
{
	thwomp_hspeed *= 0.9;
    if (demo->momentum.y <= calleoca_y)
		thwomp_hspeed *= 0.9;

	thwomp_vspeed += (thwomp_bottom - calleoca_y) * 0.05;
	thwomp_vspeed = Clamp<double>(thwomp_vspeed, -8.0, 8.0);
	
	calleoca_x += thwomp_hspeed;
	calleoca_y += thwomp_vspeed;
    calleoca_npc1->animationFrame = 1;
	if (TriggerBox(calleoca_x + 10, calleoca_y + 0, calleoca_x + 54, calleoca_y + 64))
		HurtPlayer();

	if (calleoca_y >= thwomp_bottom)
	{
		freeze_timer = 22;
		phase = 1;
	}

    if (demo->momentum.x >= MIDPOINT_X)
	{
		freeze_timer = 150;
		phase = 3;
	}
}

/*************************************************************************************************/
//Missile
void Phase3()
{
    calleoca_npc1->animationFrame = 3 + int(0.5 + missile_direction / 45) % 8;

    int dir = (int)(atan2((demo->momentum.y + demo->momentum.height / 2) - (calleoca_y + 32),
        -(demo->momentum.x + demo->momentum.width / 2) + (calleoca_x + 32))
					 * 180 / PI);

	missile_direction += AngleDifference((int)missile_direction, dir) * 0.015;

	if (missile_direction >= 360)
		missile_direction -= 360;
	else
	if (missile_direction < 0)
		missile_direction += 360;

	missile_hspeed += cos(missile_direction * PI / 180) * 0.25;
	missile_vspeed -= sin(missile_direction * PI / 180) * 0.25;

	missile_hspeed = Clamp<double>(missile_hspeed * 0.975, -15, 15);
	missile_vspeed = Clamp<double>(missile_vspeed * 0.975, -15, 15);

	if (freeze_timer > 0)
	{
		freeze_timer--;
		missile_hspeed = 0;
		missile_vspeed = 0;
		missile_fuel = 600;
	}
	else
	{
		missile_fuel--;
		if (missile_fuel <= 0)
			if (!TriggerBox(calleoca_x - 100, calleoca_y - 100, calleoca_x + 164, calleoca_y + 164))
				freeze_timer = 110;
	}

	
    calleoca_x = Clamp<double>(calleoca_x + missile_hspeed, demo->momentum.x - 464, demo->momentum.x + 464);
	calleoca_y = Clamp<double>(calleoca_y + missile_vspeed, missile_top, missile_bottom);

	if (TriggerBox(calleoca_x + 18, calleoca_y + 18, calleoca_x + 46, calleoca_y + 46))
		HurtPlayer();

    if (demo->momentum.x >= END_X)
	{
		if (calleoca_x > END_X - 64)
			calleoca_x = END_X - 64;
		freeze_timer = 150;
		phase = 4;
	}
}

void Phase4()
{
	if (freeze_timer > 0)
	{
		freeze_timer--;
	}
	else
	{
        fishingboo_hspeed += (demo->momentum.x - calleoca_x) * 0.001 * fishingboo_ferocity;
        fishingboo_vspeed += (demo->momentum.y - calleoca_y) * 0.001 * fishingboo_ferocity;
		fishingboo_hspeed = Clamp<double>(fishingboo_hspeed, -7 * fishingboo_ferocity, 7 * fishingboo_ferocity);
		fishingboo_vspeed = Clamp<double>(fishingboo_vspeed, -0.5 * fishingboo_ferocity, 0.5 * fishingboo_ferocity);

		calleoca_x += fishingboo_hspeed;
		calleoca_y += fishingboo_vspeed;

		fishingboo_ferocity += 0.0002;
	}

    calleoca_x = Clamp<double>(calleoca_x, demo->momentum.x - 512, demo->momentum.y + 464);
}

void CalleocaCode()
{
	demo = Player::Get(1);
	
	demo->Identity = CHARACTER_MARIO;

	if (calleoca_npc2 == NULL && init_doonce)
	{
		if (win_timer > 0)
			win_timer--;
		else
		{
			goal_npc = FindNPC(NPC_GOAL);
			if (goal_npc != NULL)
			{
                goal_npc->momentum.x = demo->momentum.x;
                goal_npc->momentum.y = demo->momentum.y;
			}
		}
		
		return; //boss beaten
	}
	
	if (!init_doonce)
	{
		calleoca_npc1 = FindNPC(NPC_SIGN);
		calleoca_npc2 = FindNPC(NPC_COIN);
		hurt_npc	  = FindNPC(NPC_FIREBAR);

		calleoca_x = calleoca_npc1->momentum.x;
		calleoca_y = calleoca_npc1->momentum.y;
		storage_x  = calleoca_npc2->momentum.x;
		storage_y  = calleoca_npc2->momentum.y;

		thwomp_height = calleoca_y - 64 * 6 + 16;
		thwomp_bottom = calleoca_y + 128;

		missile_top = calleoca_y - 64 * 7 - 16;
		missile_bottom = calleoca_y + 180;

		init_doonce = true;
	}

	if (calleoca_npc1->id != NPC_SIGN)
		calleoca_npc1 = FindNPC(NPC_SIGN);

    if (calleoca_npc2->id != NPC_COIN)
		calleoca_npc2 = FindNPC(NPC_COIN);

    if (hurt_npc->id != NPC_FIREBAR)
		hurt_npc = FindNPC(NPC_FIREBAR);

    hurt_npc->momentum.x = demo->momentum.x;
    hurt_npc->momentum.y = demo->momentum.y - 128;

	switch (phase)
	{
		case 0: //Standing there
			Phase0();
		break;

		case 1: //Thwomp rising
			Phase1();
		break;

		case 2: //Thwomp attacking
			Phase2();
		break;

		case 3: //Missile
			Phase3();
		break;

		case 4: //Fishing Boo
			Phase4();
		break;
	}

	if (calleoca_npc2 == NULL)
		return; //boss beaten

	if (phase < 4)
	{
		calleoca_npc1->momentum.x = calleoca_x;
		calleoca_npc1->momentum.y = calleoca_y;
		calleoca_npc2->momentum.x = storage_x;
		calleoca_npc2->momentum.y = storage_y;
	}
	else
	{
		calleoca_npc2->momentum.x = calleoca_x;
		calleoca_npc2->momentum.y = calleoca_y;
		calleoca_npc1->momentum.x = storage_x;
		calleoca_npc1->momentum.y = storage_y;
	}

	//gLunaRender.SafePrint(std::wstring(L"FUEL: " + std::to_wstring(missile_fuel)), 3, 0, 256);
	//gLunaRender.SafePrint(std::wstring(L"DEMO X: " + std::to_wstring(demo->CurYPos)), 3, 0, 256 + 32);
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

bool TriggerBox(double x1, double y1, double x2, double y2)
{
    return (demo->momentum.x + demo->momentum.width		> x1 &&
        demo->momentum.x					< x2 &&
        demo->momentum.y + demo->momentum.height	> y1 &&
        demo->momentum.y					< y2);
}

void HurtPlayer()
{
    hurt_npc->momentum.y = demo->momentum.y;
}

int AngleDifference(int angle1, int angle2)
{
	return ((((angle1 - angle2) % 360) + 540) % 360) - 180;
}