#include "LevelCodes.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../Globals.h"
#include "../SMBXInternal/Layer.h"

void TheFloorisLavaCode() {
	PlayerMOB* demo = Player::Get(1);
	LayerControl* layerSecretExit = Layer::Get(3);
	LayerControl* layerSinUpDown = Layer::Get(4);
	LayerControl* layerSinRightLeft = Layer::Get(5);
	LayerControl* layerSinUpDownAlternate = Layer::Get(6);
	LayerControl* layerSinUpDownWeak = Layer::Get(7);
	LayerControl* layerSinUpDownWeakAlt = Layer::Get(8);
	LayerControl* layerSinRightLeftAlternate = Layer::Get(9);
	
	static double layerSinUpDown_displacement = 0;
	static double layerSinRightLeft_displacement = 0;
	static char secretExitFailedStage = 0;
	
	if(demo == 0)
		return;

	// The player has touched the ground, trigger the fail sequence
	if (demo -> OnSlipperyGround && secretExitFailedStage == 0)
	{
		secretExitFailedStage = 1;
		layerSecretExit -> ySpeed = (float)-0.4;
	}
	
	//different movements to create the dropping off the world effect
	switch (secretExitFailedStage)
	{
		case 0:
			layerSecretExit -> ySpeed = (float)0;
		break;
		
		case 1:
			layerSecretExit -> ySpeed -= (float)0.2;
			
			if (layerSecretExit -> ySpeed < -4)
				secretExitFailedStage = 2;
		break;
		
		case 2:
			layerSecretExit -> ySpeed += (float)0.4;
			
			if (layerSecretExit -> ySpeed > -2.5)
				secretExitFailedStage = 3;
		break;
		
		case 3:
			layerSecretExit -> ySpeed += 1.5;
			
			if (layerSecretExit -> ySpeed > 50)
				secretExitFailedStage = 4;
		break;
		
		case 4:
			layerSecretExit -> ySpeed = 0;
			secretExitFailedStage = 5;
		break;
	}
	
	//Up Down sine wave motion
	if (layerSinUpDown -> ySpeed == 0)
	{
		secretExitFailedStage = 0;
		
		layerSinRightLeft_displacement = 0;
		layerSinRightLeft -> xSpeed = 3;
		
		layerSinUpDown_displacement = 0;
		layerSinUpDown -> ySpeed = 3;
	}
	
	layerSinUpDown -> ySpeed -= (float)layerSinUpDown_displacement / 1000;
	layerSinUpDown_displacement += layerSinUpDown -> ySpeed;
	
	layerSinUpDownAlternate -> ySpeed = -(layerSinUpDown -> ySpeed);
	layerSinUpDownWeak -> ySpeed = (layerSinUpDown -> ySpeed) / 3;
	layerSinUpDownWeakAlt -> ySpeed = -(layerSinUpDown -> ySpeed) / 3;
	
	//stop the initial trigger going off again
	if (layerSinUpDown -> ySpeed == (float)0)
		layerSinUpDown -> ySpeed = (float)0.001;
	
	//Right Left sine wave motion
	layerSinRightLeft -> xSpeed -= (float)layerSinRightLeft_displacement / 2000;
	layerSinRightLeft_displacement += layerSinRightLeft -> xSpeed;
	layerSinRightLeftAlternate -> xSpeed = -(layerSinRightLeft -> xSpeed);
}
