#include "LevelCodes.h"
#include "../SMBXInternal/PlayerMOB.h"
#include "../Globals.h"

void QraestoliaCavernsCode() {
	static int lastDownPress = 0;
	PlayerMOB* demo = Player::Get(1);
	if(demo == 0)
		return;
	// Player pressed down, what do we do?
	if(Player::PressingDown(demo)) {

		// Else, see if pressed down in the last 7 frames
		if(gFrames < lastDownPress + 10 && gFrames > lastDownPress + 1) {	
			if(demo->Identity == 2 && demo->MountType != 0) {
				Player::CycleLeft(demo);
			} else {
				Player::CycleRight(demo);
			}
			lastDownPress = gFrames - 9;
			return;
		}

		// Else, set last press frame as this one
		lastDownPress = gFrames;
	}
}