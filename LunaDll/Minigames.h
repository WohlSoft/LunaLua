#pragma once

#include <queue>
#include "Defines.h"
#include "Globals.h"
#include "CGUI.h"
#include "Rendering.h"

using namespace std;

class Minigame {
	virtual bool Initialize() =0;
	virtual void Process() =0;
	virtual void Draw(Renderer g) =0;
	virtual void End() =0;

	virtual void OnPause();				// Should be called if user tries to pause SMBX, to disable pausing

	Minigame* m_ChildGame;
	list<CGUIElement*> m_GUIElements;	
};

//class MinigameController {
//	queue<Minigame*> m_Games;
//};