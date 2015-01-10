#ifndef GameboyRGP_Hhh
#define GameboyRGP_Hhh
#include "Minigames.h"

class GameboyRPG : Minigame {
	bool Initialize();
	void Process();
	void Draw(Renderer g);
	void End();
};

#endif
