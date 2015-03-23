#include "LevelCodes.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../Globals.h"
#include "../SMBXInternal/Layer.h"
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CLAMP(x, l, h) (((x) > (h)) ? (h) : (((x) < (l)) ? (l) : (x)))

void AbstractAssaultCode() {
	PlayerMOB* demo = Player::Get(1);
	LayerControl* layerDefault = Layer::Get(0);
	LayerControl* layerStartingPlatform = Layer::Get(3);
	
	static float hspeed = 0, vspeed = 0;
	static char gameStarted = 0;
	static unsigned short noControlTimer = 0;
	
    char press_up = (demo->keymap.UKeyState != 0);
    char press_left = (demo->keymap.LKeyState != 0);
    char press_down = (demo->keymap.DKeyState != 0);
    char press_right = (demo->keymap.RKeyState != 0);
	
	short *powerup = (short*)((char*)(demo) + 0x112);
	
	if (layerStartingPlatform -> ySpeed == 0)
	{

		demo -> Identity = 1; //Demo
		//player_id_set = 1;
		gameStarted = 0;
		*powerup = 1;
	}
	else if (!gameStarted)
	{
		*powerup = 6;
		demo -> IsSpinjumping = 0;
		demo -> Hearts = 3;
		vspeed = -16;
		hspeed = -4;
		gameStarted = 1;
		noControlTimer = 30;
		demo -> Identity = 5; //Sheath
	}
	
	if (gameStarted)
	{
		if (layerStartingPlatform -> ySpeed == 0)
			gameStarted = 0;
		
		demo -> IsSpinjumping = 0;
		
		layerDefault -> xSpeed =(float) MAX(layerDefault -> xSpeed - 0.015, -2.5);
		
		if (demo -> Hearts > 1)
		*powerup = 6;
		
		demo -> FacingDirection = 1;
		demo -> CurrentKillCombo %= 9;
		
		if (noControlTimer == 0)
		{
			vspeed =(float) CLAMP(vspeed + (press_down - press_up) * 0.5, -10, 10);
			hspeed =(float) CLAMP(hspeed + (press_right - press_left) * 0.5, -10, 10);
		}
		else
			noControlTimer--;
		
		demo->momentum.speedY = -0.4 + vspeed;
        demo->momentum.speedX = hspeed;
		
		hspeed *=(float) 0.9;
		vspeed *=(float) 0.9;
	}
}
