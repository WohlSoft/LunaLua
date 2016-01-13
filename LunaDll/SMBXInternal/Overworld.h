#ifndef Overworld_hhhhhh
#define Overworld_hhhhhh

#include "../Misc/VB6StrPtr.h"
#include "../Defines.h"

struct Overworld{
    char padding1[64];					
    Momentum momentum;                  //+0x40
    short currentPowerup;   			//+0x70
    short currentWalkingFrame;			//+0x72		0 / 1 = Walking frame down | 2 / 3 Walking frame right | 4 / 5 Walking frame left | 6 / 7 Walking frame up
    short currentWalkingFrameTimer;		//+0x74		0-7 is won't change the frame, if higher or equal to 8 then currentRunningFrame will be swaped
    short currentWalkingDirection;		//+0x76		0 = Not Walking (Idle) | 1 = Walking Up | 2 = Walking Left | 3 = Walking Down | 4 = Walking Right
    short currentWalkingTimer;			//+0x78		0-32	will increment by 2 each frame, if reached 32 then the target tile has been reached and movement will be stopped
    short isCurrentlyWalking;			//+0x7A		TRUE / FALSE	0 / -1
    VB6StrPtr currentLevelTitle;		//+0x7C
};

namespace SMBXOverworld{
    Overworld* get();
}

#endif
